#include "./topdieinstitem.h"
#include "./pinitem.h"
#include "qchar.h"
#include "qpoint.h"
#include "./tobitem.h"
#include <hardware/bump/bump.hh>

#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QDebug>

namespace kiwi::widget::layout {

    /*
    
        +----------------------------------------------------+
        |                                                    |
        |                                                    |
        |                                                    |
        |                                                    |
        |                                                    |
        |                                                    |
        |                                                    |
        |                                                    |
        |                                                    |
        |                                                    |
        |                                                    |
        |                                                    |
        |                 * * * * * * * * * * * * * * * *    |
        |                 * * * * * * * * * * * * * * * *    |
        |                 * * * * * * * * * * * * * * * *    |
        |                 * * * * * * * * * * * * * * * *    |
        |                 * * * * * * * * * * * * * * * *    |
        |                 * * * * * * * * * * * * * * * *    |
        |                 * * * * * * * * * * * * * * * *    |
        |                 * * * * * * * * * * * * * * * *    |
        |                 * * * * * * * * * * * * * * * *    |
        |                 * * * * * * * * * * * * * * * *    |
        |                 * * * * * * * * * * * * * * * *    |
        |                                                    |
        +----------------------------------------------------+
    
    */

    const QColor TopDieInstItem::COLOR = QColor::fromRgb(84, 139, 84);

    TopDieInstItem::TopDieInstItem(circuit::TopDieInstance* topdieInst): 
        _topdieInst{topdieInst},
        _name{ topdieInst != nullptr ? QString::fromStdString(topdieInst->name().data()) : "" }
    {   
        this->setFlags(ItemIsSelectable | ItemIsMovable);

        auto basePos = QPointF{BUMP_AREA_BEGIN_X - WIDTH / 2, BUMP_AREA_BEGIN_Y - HEIGHT / 2.};
        for (int r = 0; r < BUMP_ARRAY_ROW; ++r) {
            for (int c = 0; c < BUMP_ARRAY_COL; ++c) {
                auto pinItem = new PinItem {
                    nullptr, 
                    basePos + QPointF{
                    c * BUMP_HORI_INTERVAL, r * BUMP_VERT_INTERVAL
                    },
                    this
                };
                this->_pins.push_back(pinItem);
            }
        }
    }

    auto TopDieInstItem::boundingRect() const -> QRectF {
        return QRectF(-WIDTH / 2., -HEIGHT / 2., WIDTH, HEIGHT);
    }
    
    void TopDieInstItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) {
        painter->setBrush(COLOR);
        painter->drawRect(-WIDTH / 2., -HEIGHT / 2., WIDTH, HEIGHT);
        painter->drawText(QPointF{
            NAME_AREA_BEGIN_X - WIDTH/2, 
            NAME_AREA_BEGIN_Y - HEIGHT/2,
            }, 
            this->_name);
    }

    void TopDieInstItem::mousePressEvent(QGraphicsSceneMouseEvent* event) {
        if (event->button() == Qt::LeftButton) {
            this->_originPos = event->scenePos();
        }
        QGraphicsItem::mousePressEvent(event);
    }

    void TopDieInstItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event) {
        QGraphicsItem::mouseReleaseEvent(event);

        // 检测是否与某个 TOBItem 碰撞
        auto collidingItems = scene()->collidingItems(this);
        for (auto item : collidingItems) {
            if (item->type() == TOBItem::Type) {
                auto tob = dynamic_cast<TOBItem*>(item);
                if (!tob->hasTopDieInst()) {
                    // 如果这个 tob 是空的，吸附到这个 tob 上
                    if (this->_currentTOB != nullptr) {
                        this->_currentTOB->removeTopDieInst();
                    }
                    tob->setTopDieInst(this);
                    this->setPos(tob->pos());
                    this->_currentTOB = tob;
                } else {
                    // 如果这个 tob 不是空的，吸附不了，把 TopDieInst 归位
                    if (this->_currentTOB != nullptr) {
                        this->setPos(this->_currentTOB->pos());
                    } else {
                        this->setPos(this->_originPos);
                    }
                }
                return;
            }
        }

        // 没有碰到任何的 TOBItem，是否是将自己移出？
        if (this->_currentTOB != nullptr) {
            this->_currentTOB->removeTopDieInst();
            this->_currentTOB = nullptr;
        }
    }

    // auto TopDieInstItem::itemChange(GraphicsItemChange change, const QVariant& value) -> QVariant {
    //     if (change == QGraphicsItem::ItemPositionChange) {
    //         // 在拖动过程中检测是否与 TOBItem 碰撞
    //         QList<QGraphicsItem*> collidingItems = scene()->collidingItems(this);
    //         for (QGraphicsItem* item : collidingItems) {
    //             if (item->type() == TOBItem::Type) {
    //                 auto tob = dynamic_cast<TOBItem*>(item);
    //                 if (!tob->hasTopDieInst()) {
    //                     tob->highlight(true);
    //                 } else {
    //                     tob->highlight(false);
    //                 }
    //             }
    //         }
    //     }
    //     return QGraphicsItem::itemChange(change, value);
    // }

}