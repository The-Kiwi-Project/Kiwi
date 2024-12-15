#include "./pinitem.h"
#include "qcolor.h"
#include "qglobal.h"
#include "qnamespace.h"
#include "qobject.h"
#include "qpoint.h"
#include "./netitem.h"

namespace kiwi::widget::schematic {

    const QColor PinItem::COLOR = Qt::black;
    const QColor PinItem::HOVERED_COLOR = Qt::red;

    PinItem::PinItem(const QString &name, QPointF position, PinSide side, QGraphicsItem *parent)
        : QGraphicsItem(parent), 
        _name{name}, 
        _side{side},
        _hovered{false} 
    {
        this->setPos(position);
        this->setAcceptHoverEvents(true);
        this->setFlags(ItemSendsScenePositionChanges);
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

    auto PinItem::connectTo(PinItem* other) -> NetItem* {
        // Make two point to wrap two pin
        auto net = new NetItem {this, other};
        return net;
    }

    auto PinItem::itemChange(GraphicsItemChange change, const QVariant& value) -> QVariant {
        if (change == QGraphicsItem::ItemScenePositionHasChanged) {
            if (this->_connectedNetPoint) {
                this->_connectedNetPoint->setPos(this->scenePos());
            }
        }
        return QGraphicsItem::itemChange(change, value);
    }

    void PinItem::hoverEnterEvent(QGraphicsSceneHoverEvent *) {
        this->_hovered = true;
        this->update();
    }

    void PinItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *) {
        this->_hovered = false;
        this->update();
    }

}