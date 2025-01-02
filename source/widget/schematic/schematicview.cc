#include "./schematicview.h"

#include "./item/pinitem.h"
#include "./item/topdieinstitem.h"
#include "./item/exportitem.h"
#include "circuit/pin/pin.hh"
#include "std/utility.hh"
#include "widget/schematic/schematicscene.h"
#include <QPainter>
#include <QBrush>
#include <cassert>
#include <hardware/interposer.hh>
#include <circuit/basedie.hh>
#include <QDebug>
#include <QWheelEvent>
#include <QScrollBar>
#include <cmath>

namespace kiwi::widget {

    using namespace schematic;

    SchematicView::SchematicView(
        hardware::Interposer* interposer, 
        circuit::BaseDie* basedie,
        QWidget *parent
    ) :
        GraphicsView{parent},
        _interposer{interposer},
        _basedie{basedie}
    {
        this->_scene = new SchematicScene {};
        this->setScene(this->_scene);
        this->setDragMode(QGraphicsView::RubberBandDrag);
        this->setInteractive(true);
        this->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);

        for (auto& [name, topdie] : this->_basedie->topdie_insts()) {
            auto t = this->_scene->addTopDieInst(topdie.get());
            this->_topdieInstItems.push_back(t);
        }

        auto spacing = 20.0;

        int cols = std::ceil(std::sqrt(this->_topdieInstItems.size()));
        int rows = std::ceil(static_cast<double>(this->_topdieInstItems.size()) / cols);

        int startX = spacing;
        int startY = spacing;

        for (size_t i = 0; i < this->_topdieInstItems.size(); ++i) {
            int row = i / cols;
            int col = i % cols;

            int x = startX + col * (this->_topdieInstItems[i]->width() + spacing);
            int y = startY + row * (this->_topdieInstItems[i]->width() + spacing);

            this->_topdieInstItems[i]->setPos(x, y);
        }


        for (auto& [sync, connections] : this->_basedie->connections()) {
            for (const auto& connection : connections) {
                PinItem* beginPin = nullptr;
                PinItem* endPin = nullptr;

                std::match(connection.input,
                    [&beginPin, this](const circuit::ConnectExPort& eport) {
                        auto eportItem = this->_scene->addExPort(QString::fromStdString(eport.name));
                        beginPin = eportItem->pin();
                    },
                    [&beginPin, this](const circuit::ConnectBump& bump) {
                        auto top = this->_scene->topdieinstMap().value(bump.inst);
                        beginPin = top->pinitems().value(QString::fromStdString(bump.name));
                    }
                );

                std::match(connection.output,
                    [&endPin, this](const circuit::ConnectExPort& eport) {
                        auto eportItem = this->_scene->addExPort(QString::fromStdString(eport.name));
                        endPin = eportItem->pin();
                    },
                    [&endPin, this](const circuit::ConnectBump& bump) {
                        auto top = this->_scene->topdieinstMap().value(bump.inst);
                        endPin = top->pinitems().value(QString::fromStdString(bump.name));
                    }
                );

                this->_scene->connectPins(beginPin, endPin);
            }
        }
    }

    SchematicView::~SchematicView() noexcept {}

}