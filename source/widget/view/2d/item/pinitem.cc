#include "./pinitem.h"
#include "qcolor.h"
#include "qglobal.h"
#include "qnamespace.h"

namespace kiwi::widget {

    const QColor Pin::COLOR = Qt::black;
    const QColor Pin::HOVERED_COLOR = Qt::red;

    Pin::Pin(const QString &name, QPointF position, PinSide side, QGraphicsItem *parent)
        : QGraphicsItem(parent), 
        _name{name}, 
        _side{side},
        _position{position}, 
        _hovered{false} 
    {
        this->setAcceptHoverEvents(true);
    }

    auto Pin::boundingRect() const -> QRectF {
        return QRectF(this->_position.x() - PIN_RADIUS, this->_position.y() - PIN_RADIUS, PIN_DIAMETER, PIN_DIAMETER);
    }

    void Pin::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) {
        painter->setBrush(this->_hovered ? HOVERED_COLOR : COLOR);
        painter->drawEllipse(this->_position, PIN_RADIUS, PIN_RADIUS);

        auto length = this->_name.size() * CHAR_WIDTH_;
        painter->setPen(Qt::blue);
        switch (this->_side) {
            case PinSide::Top: {
                auto textRect = QRectF {
                    this->_position.x() - CHAR_HEIGHT / 2., 
                    this->_position.y() + NAME_INTERVAL, 
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
                    this->_position.x() - CHAR_HEIGHT / 2, 
                    this->_position.y() - length - NAME_INTERVAL, 
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
                    this->_position.x() + NAME_INTERVAL, 
                    this->_position.y() - CHAR_HEIGHT / 2., 
                    length, 
                    CHAR_HEIGHT
                };
                painter->drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter, this->_name);
                break;
            }
            case PinSide::Right: {
                auto textRect = QRectF {
                    this->_position.x() - length - NAME_INTERVAL, 
                    this->_position.y() - CHAR_HEIGHT / 2., 
                    length, 
                    CHAR_HEIGHT
                };
                painter->drawText(textRect, Qt::AlignRight | Qt::AlignVCenter, this->_name);
                break;
            }
        }
    }

    void Pin::hoverEnterEvent(QGraphicsSceneHoverEvent *) {
        this->_hovered = true;
        update();
    }

    void Pin::hoverLeaveEvent(QGraphicsSceneHoverEvent *) {
        this->_hovered = false;
        update();
    }

}