#include "./pinitem.h"
#include "qcolor.h"
#include "qglobal.h"
#include "qnamespace.h"
#include "qobject.h"
#include "qpoint.h"
#include "./netitem.h"
#include "./netpointitem.h"
#include "./topdieinstitem.h"
#include "../schematicscene.h"
#include "./exportitem.h"
#include <QGraphicsSceneMouseEvent>
#include <cassert>

namespace kiwi::widget::schematic {

    extern SchematicScene* scene;

    const QColor PinItem::COLOR = Qt::black;
    const QColor PinItem::HOVERED_COLOR = Qt::red;

    PinItem::PinItem(
        const QString &name, 
        QPointF position, 
        PinSide side, 
        QGraphicsItem *parent
    )
        : QGraphicsItem(parent), 
        _name{name},
        _side{side}
    {
        this->setPos(position);
        this->setAcceptHoverEvents(true);
        this->setFlags(ItemSendsScenePositionChanges | ItemIsSelectable);
    }

    auto PinItem::boundingRect() const -> QRectF {
        return QRectF(-this->_raduis,  -this->_raduis, 2 * this->_raduis, 2 * this->_raduis);
    }

    void PinItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) {
        painter->setBrush(this->_hovered ? HOVERED_COLOR : COLOR);
        painter->drawEllipse(QPointF{0., 0.}, this->_raduis, this->_raduis);

        auto length = this->_name.size() * CHAR_WIDTH_;
        painter->setPen(Qt::blue);
        switch (this->_side) {
            case PinSide::Top: {
                auto textRect = QRectF {
                    -CHAR_HEIGHT / 2., 
                    NAME_INTERVAL, 
                    CHAR_HEIGHT,
                    length
                };

                painter->save();
                painter->translate(textRect.center());
                painter->rotate(90);
                painter->drawText(QRectF(-textRect.height() / 2, -textRect.width() / 2, textRect.height(), textRect.width()), 
                                  Qt::AlignLeft, this->_name);
                painter->restore();
                break;
            }
            case PinSide::Bottom: {
                auto textRect = QRectF {
                    - CHAR_HEIGHT / 2, 
                    - length - NAME_INTERVAL, 
                    CHAR_HEIGHT,
                    length
                };
                
                painter->save();
                painter->translate(textRect.center());
                painter->rotate(-90);
                painter->drawText(QRectF(-textRect.height() / 2, -textRect.width() / 2, textRect.height(), textRect.width()), 
                                  Qt::AlignLeft, this->_name);
                painter->restore();
                break;
            }
            case PinSide::Left: {
                auto textRect = QRectF {
                    NAME_INTERVAL, 
                    - CHAR_HEIGHT / 2., 
                    length, 
                    CHAR_HEIGHT
                };
                painter->drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter, this->_name);
                break;
            }
            case PinSide::Right: {
                auto textRect = QRectF {
                    - length - NAME_INTERVAL, 
                    - CHAR_HEIGHT / 2., 
                    length, 
                    CHAR_HEIGHT
                };
                painter->drawText(textRect, Qt::AlignRight | Qt::AlignVCenter, this->_name);
                break;
            }
        }
    }

    auto PinItem::isExportPin() const -> bool {
        return this->parentItem()->type() == ExternalPortItem::Type;
    }

    auto PinItem::isTopdieInstPin() const -> bool {
        return !this->isExportPin();
    }

    auto PinItem::toString() const -> QString {
        if (this->isTopdieInstPin()) {
            return QString{"%1.%2"}.arg(this->topdieInstItem()->name()).arg(this->_name);
        }
        return this->_name;
    }

    auto PinItem::toCircuitPin() const -> circuit::Pin {
        assert(this->parentItem() != nullptr);
        auto circuitPin = this->isExportPin() ? (
            circuit::Pin::connect_export(
                dynamic_cast<ExternalPortItem*>(this->parentItem())->exPort()
            )
        ) : (
            circuit::Pin::connect_bump(
                dynamic_cast<TopDieInstanceItem*>(this->parentItem())->topdieInst(),
                this->name().toStdString()
            )
        );

        return circuitPin;
    }

    auto PinItem::itemChange(GraphicsItemChange change, const QVariant& value) -> QVariant {
        if (change == QGraphicsItem::ItemScenePositionHasChanged) {
            for (auto point : this->_connectedNetPoints) {
                point->updatePos();
            }
        }
        return QGraphicsItem::itemChange(change, value);
    }

    void PinItem::mousePressEvent(QGraphicsSceneMouseEvent* event) {
        dynamic_cast<SchematicScene*>(this->scene())->headleCreateNet(this, event);
        QGraphicsItem::mousePressEvent(event);
    }

    void PinItem::hoverEnterEvent(QGraphicsSceneHoverEvent * event) {
        this->_hovered = true;
        this->update();
        QGraphicsItem::hoverEnterEvent(event);
    }

    void PinItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event) {
        this->_hovered = false;
        this->update();
        QGraphicsItem::hoverLeaveEvent(event);
    }

    auto PinItem::exportItem() const -> ExternalPortItem* {
        assert(this->parentItem() != nullptr);
        return dynamic_cast<ExternalPortItem*>(this->parentItem());
    }

    auto PinItem::topdieInstItem() const -> TopDieInstanceItem* {
        assert(this->parentItem() != nullptr);
        return dynamic_cast<TopDieInstanceItem*>(this->parentItem());   
    }

}