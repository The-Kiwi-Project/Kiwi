#include "./layoutscene.h"
#include "./item/netitem.h"
#include "./item/pinitem.h"
#include "./item/tobitem.h"
#include "./item/topdieinstitem.h"
#include "circuit/connection/pin.hh"
#include "hardware/track/trackcoord.hh"
#include "qglobal.h"
#include "qpoint.h"
#include "widget/layout/item/exportitem.h"
#include "widget/layout/item/sourceportitem.h"
#include <cassert>
#include <circuit/basedie.hh>

#include <cmath>
#include <hardware/interposer.hh>
#include <circuit/basedie.hh>
#include <widget/frame/itemtypecheck.h>

#include <debug/debug.hh>
#include <QDebug>

namespace kiwi::widget {

    static_assert(AllUnique<
        (int)layout::NetItem::Type,
        (int)layout::PinItem::Type,
        (int)layout::TopDieInstanceItem::Type,
        (int)layout::ExternalPortItem::Type,
        (int)layout::SourcePortItem::Type
    >::value);

    using namespace layout;

    const QPointF LayoutScene::INTERPOSER_LEFT_DOWN_POSITION = 
        LayoutScene::tobPosition(hardware::TOBCoord{0, 0}) - 
        QPointF{TOBItem::WIDTH/2 + INTERPOSER_SIDE_GAP, -TOBItem::HEIGHT/2 - INTERPOSER_SIDE_GAP};

    const QPointF LayoutScene::INTERPOSER_RIGHT_UP_POSITION = 
        LayoutScene::tobPosition(hardware::TOBCoord{hardware::Interposer::TOB_ARRAY_WIDTH-1, hardware::Interposer::TOB_ARRAY_HEIGHT-1}) + 
        QPointF{TOBItem::WIDTH/2 + INTERPOSER_SIDE_GAP, -TOBItem::HEIGHT/2 - INTERPOSER_SIDE_GAP};

    const QPointF LayoutScene::EXPORT_LEFT_DOWN_POSITION = 
        LayoutScene::tobPosition(hardware::TOBCoord{0, 0}) - 
        QPointF{TOBItem::WIDTH/2 + EXPORT_SIDE_GAP, -TOBItem::HEIGHT/2 - EXPORT_SIDE_GAP};

    const QPointF LayoutScene::EXPORT_RIGHT_UP_POSITION = 
        LayoutScene::tobPosition(hardware::TOBCoord{hardware::Interposer::TOB_ARRAY_WIDTH-1, hardware::Interposer::TOB_ARRAY_HEIGHT-1}) + 
        QPointF{TOBItem::WIDTH/2 + EXPORT_SIDE_GAP, -TOBItem::HEIGHT/2 - EXPORT_SIDE_GAP};


    LayoutScene::LayoutScene(hardware::Interposer* interposer, circuit::BaseDie* basedie, QObject* parent) :
        QGraphicsScene{parent},
        _basedie{basedie},
        _interposer{interposer}
    {
        this->addSceneItems();
    }

    void LayoutScene::addSceneItems() {
        this->addTOBItems();
        this->addTopDieInstanceItems();
        this->addExternalPortItems();
        this->addNetItems();

        this->choiseSourcePort();
    }

    void LayoutScene::addTOBItems() {
        for (auto& [coord, tob] : this->_interposer->tobs()) {
            auto t = this->addTOB(tob.get());
            t->setPos(LayoutScene::tobPosition(coord));
        }

        auto r = new QGraphicsRectItem {QRectF{
            INTERPOSER_LEFT_DOWN_POSITION, 
            INTERPOSER_RIGHT_UP_POSITION 
        }};
        
        r->setBrush(Qt::gray);
        r->setZValue(layout::TOBItem::Z_VALUE - 1);
        this->addItem(r);
    }

    void LayoutScene::addTopDieInstanceItems() {
        // Call after addTOBItems!!
        for (auto& [name, topdieInst] : this->_basedie->topdie_insts()) {
            auto item = this->addTopDieInstance(topdieInst.get());
            auto tob = topdieInst->tob();
            debug::check_fmt(tob != nullptr, "No placed tob of topdie instance!");
            // Find the tobitem object
            auto tobItem = this->_tobsMaps.value(tob);
            item->placeInTOB(tobItem);

            // Connect the tob changed signal
            connect(item, &TopDieInstanceItem::placedTOBChanged, 
                [this, item] (TOBItem *originTOB, TOBItem *newTOB) {
                    // MARK, maybe better..
                    this->choiseSourcePort();
                    emit this->topdieInstancePlacedTOBChanged(item, originTOB, newTOB);
                }
            );
        }
    }

    void LayoutScene::addExternalPortItems() {
        const auto width = EXPORT_RIGHT_UP_POSITION.x() - EXPORT_LEFT_DOWN_POSITION.x();
        const auto height = EXPORT_LEFT_DOWN_POSITION.y() - EXPORT_RIGHT_UP_POSITION.y();
        const auto width_interval = width / (hardware::Interposer::COB_ARRAY_WIDTH * (int)hardware::COB::INDEX_SIZE);
        const auto height_interval = height / (hardware::Interposer::COB_ARRAY_WIDTH * (int)hardware::COB::INDEX_SIZE);

        auto get_port_position = [width_interval, height_interval] (const hardware::TrackCoord& coord) -> QPointF {
            switch (coord.dir) {
                case hardware::TrackDirection::Horizontal: {
                    auto index = coord.row * hardware::COB::INDEX_SIZE + coord.index;
                    auto y = EXPORT_LEFT_DOWN_POSITION.y() - height_interval * index;
                    if (coord.col == 0) {
                        return QPointF { EXPORT_LEFT_DOWN_POSITION.x(), y };
                    } 
                    else {
                        return QPointF { EXPORT_RIGHT_UP_POSITION.x(), y };
                    }
                }
                case hardware::TrackDirection::Vertical: {
                    auto index = coord.col * hardware::COB::INDEX_SIZE + coord.index;
                    auto x = EXPORT_LEFT_DOWN_POSITION.x() + index * width_interval;
                    if (coord.row == 0) {
                        return QPointF { x, EXPORT_LEFT_DOWN_POSITION.y() };
                    } 
                    else {
                        return QPointF { x, EXPORT_RIGHT_UP_POSITION.y()};
                    }
                }
            }
            debug::unreachable();
        };

        for (auto& [name, eport] : this->_basedie->external_ports()) {
            auto item = this->addExternalPort(eport.get());
            const auto coord = eport->coord();
            assert(hardware::Interposer::is_external_port_coord(coord));
            item->setPos(get_port_position(coord));
        }

        for (auto& coord : this->_basedie->pose_ports()) {
            auto port = this->addVDDSourcePort();
            port->setPos(get_port_position(coord));
        }

        for (auto& coord : this->_basedie->nege_ports()) {
            auto port = this->addGNDSourcePort();
            port->setPos(get_port_position(coord));
        }
    }

    void LayoutScene::addNetItems() {
        for (auto& [_, connections] : this->_basedie->connections()) {
            for (const auto& connection : connections) {
                auto beginPin = this->circuitPinToPinItem(connection->input_pin());
                auto endPin = this->circuitPinToPinItem(connection->output_pin());
                
                assert(beginPin != nullptr);
                assert(endPin != nullptr);

                this->addNet(beginPin, endPin);
            }
        }
    }

    auto LayoutScene::addNet(layout::PinItem* beginPin, layout::PinItem* endPin) -> layout::NetItem* {
        auto n = new layout::NetItem {beginPin, endPin};
        // MARK: Check source to source
        assert(!(beginPin->isSourcePortPin() && endPin->isSourcePortPin()));
        if (beginPin->isSourcePortPin() || endPin->isSourcePortPin()) {
            this->_netsWithSourcePorts.push_back(n);
        }
        this->_nets.push_back(n);
        this->addItem(n);
        return n;
    }

    auto LayoutScene::addTOB(hardware::TOB* tob) -> layout::TOBItem* {
        auto t = new layout::TOBItem {tob};
        this->_tobsMaps.insert(tob, t);
        this->addItem(t);
        return t;
    }

    auto LayoutScene::addTopDieInstance(circuit::TopDieInstance* topdieInst) -> layout::TopDieInstanceItem* {
        auto ti = new layout::TopDieInstanceItem {topdieInst};
        this->_topdieinstMap.insert(topdieInst, ti);
        this->addItem(ti);
        return ti;
    }

    auto LayoutScene::addExternalPort(circuit::ExternalPort* eport) -> layout::ExternalPortItem* {
        auto eportItem = new layout::ExternalPortItem {eport};
        this->_externalPortsMap.insert(eport, eportItem);
        this->addItem(eportItem);
        return eportItem;
    }

    auto LayoutScene::addVDDSourcePort() -> layout::SourcePortItem* {
        auto portItem = new layout::SourcePortItem {layout::SourcePortType::VDD};
        this->_vddPorts.push_back(portItem);
        this->addItem(portItem);
        return portItem;
    }

    auto LayoutScene::addGNDSourcePort() -> layout::SourcePortItem* {
        auto portItem = new layout::SourcePortItem {layout::SourcePortType::GND};
        this->_gndPorts.push_back(portItem);
        this->addItem(portItem);
        return portItem;
    }

    auto LayoutScene::totalNetLenght() -> qreal {
        auto sum = 0.0;
        for (auto net : this->_nets) {
            sum += LayoutScene::pinDistance(net->beginPin(), net->endPin());
        }
        return sum;
    }

    void LayoutScene::choiseSourcePort() {
        for (auto net : this->_netsWithSourcePorts) {
            if (net->beginPin()->isSourcePortPin()) {
                auto endPinPos = net->endPin()->scenePos();
                
                auto originPort = net->beginPin()->parentSourcePort();
                auto& ports = originPort->isVDD() ? this->_vddPorts : this->_gndPorts;

                auto newBeginPort = 
                    LayoutScene::getMinDistancePort(ports, endPinPos);
                
                net->moveToBeginPin(newBeginPort->pin());
            }
            else if (net->endPin()->isSourcePortPin()) {
                auto beginPinPos = net->beginPin()->scenePos();
                
                auto originPort = net->endPin()->parentSourcePort();
                auto& ports = originPort->isVDD() ? this->_vddPorts : this->_gndPorts;

                auto newEndPort = 
                    LayoutScene::getMinDistancePort(ports, beginPinPos);
                
                net->moveToEndPin(newEndPort->pin());
            } 
            else {
                debug::unreachable();
            }
        }
    }

    auto LayoutScene::circuitPinToPinItem(const circuit::Pin& pin) -> layout::PinItem* {
        return std::match(pin.connected_point(),
            [this](const circuit::ConnectVDD& vdd) -> PinItem* {
                return this->_vddPorts.front()->pin();
            },
            [this](const circuit::ConnectGND& vdd) -> PinItem* {
                return this->_gndPorts.front()->pin();
            },
            [this](const circuit::ConnectExPort& eport) -> PinItem* {
                auto eportItem = this->_externalPortsMap.value(eport.port);
                return eportItem->pin();
            },
            [this](const circuit::ConnectBump& bump) -> PinItem* {
                auto inst = this->_topdieinstMap.value(bump.inst);
                auto pin = inst->pins()[bump.inst->topdie()->pins_map().at(bump.name)];
                return pin;
            }
        );
    }

    auto LayoutScene::getMinDistancePort(
        const QVector<layout::SourcePortItem*> ports, 
        const QPointF& targetPos) -> layout::SourcePortItem* 
    {
        auto minDistance = std::numeric_limits<qreal>::max();
        layout::SourcePortItem* minPort = nullptr;

        for (auto sport : ports) {
            auto distance = LayoutScene::pointDistance(sport->pin()->scenePos(), targetPos);
            if (distance < minDistance) {
                minDistance = distance;
                minPort = sport;
            }
        }

        assert(minPort != nullptr);
        return minPort;
    }

    auto LayoutScene::pinDistance(layout::PinItem* pin1, layout::PinItem* pin2) -> qreal {
        assert(pin1 != nullptr && pin2 != nullptr);
        return LayoutScene::pointDistance(pin1->scenePos(), pin2->scenePos());
    }

    auto LayoutScene::pointDistance(const QPointF& p1, const QPointF& p2) -> qreal {
        return (p1 - p2).manhattanLength();
    }

    auto LayoutScene::tobPosition(const hardware::TOBCoord& coord) -> QPointF {
        return QPointF{coord.col * (TOBItem::WIDTH + TOB_INTERVAL), -coord.row * (TOBItem::HEIGHT + TOB_INTERVAL)};
    }

}