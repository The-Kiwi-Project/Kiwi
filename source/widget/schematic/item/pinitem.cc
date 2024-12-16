#include "./pinitem.h"
#include "qcolor.h"
#include "qglobal.h"
#include "qnamespace.h"
#include "qobject.h"
#include "qpoint.h"
#include "./netitem.h"
#include "../schematicscene.h"
#include <QGraphicsSceneMouseEvent>

namespace kiwi::widget::schematic {

    extern SchematicScene* scene;

    const QColor PinItem::COLOR = Qt::black;
    const QColor PinItem::HOVERED_COLOR = Qt::red;

    PinItem::PinItem(
        const QString &name, QPointF position, PinSide side, 
        SchematicScene* scene, QGraphicsItem *parent
    )
        : QGraphicsItem(parent), 
        _name{name}, 
        _side{side},
        _hovered{false},
        _scene{scene}
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

    auto PinItem::itemChange(GraphicsItemChange change, const QVariant& value) -> QVariant {
        if (change == QGraphicsItem::ItemScenePositionHasChanged) {
            if (this->_connectedNetPoint != nullptr) {
                this->_connectedNetPoint->setPos(this->scenePos());
            }
        }
        return QGraphicsItem::itemChange(change, value);
    }

    void PinItem::mousePressEvent(QGraphicsSceneMouseEvent* event) {
        auto net = this->_scene->floatingNet();
        if (net != nullptr) {
            auto endPoint = new NetPointItem {this};
            this->_scene->addItem(endPoint);
            net->setEndPoint(endPoint);
            endPoint->setNetItem(net);

            this->_scene->setFloatingNet(nullptr);
        } else {
            auto beginPoint = new NetPointItem {this};
            this->_scene->addItem(beginPoint);
        
            auto fnet = new NetItem {beginPoint};
            this->_scene->addItem(fnet);

            this->_scene->setFloatingNet(fnet);
        }

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

}