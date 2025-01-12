#include "./topdieinstitem.h"
#include "./pinitem.h"
#include "qchar.h"
#include "qpoint.h"
#include "./tobitem.h"
#include <cassert>

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

    const QColor TopDieInstanceItem::COLOR = QColor::fromRgb(84, 139, 84);

    TopDieInstanceItem::TopDieInstanceItem(circuit::TopDieInstance* topdieInst): 
        _topdieInst{topdieInst},
        _name{ topdieInst != nullptr ? QString::fromStdString(topdieInst->name().data()) : "" }
    {   
        this->setFlags(ItemIsSelectable | ItemIsMovable);
        this->setZValue(Z_VALUE);

        auto basePos = QPointF{BUMP_AREA_BEGIN_X - WIDTH / 2, BUMP_AREA_BEGIN_Y - HEIGHT / 2.};
        for (int r = 0; r < BUMP_ARRAY_ROW; ++r) {
            for (int c = 0; c < BUMP_ARRAY_COL; ++c) {
                auto pinItem = new PinItem {
                    basePos + QPointF{
                    c * BUMP_HORI_INTERVAL, r * BUMP_VERT_INTERVAL
                    },
                    this
                };
                this->_pins.push_back(pinItem);
            }
        }
    }

    auto TopDieInstanceItem::boundingRect() const -> QRectF {
        return QRectF(-WIDTH / 2., -HEIGHT / 2., WIDTH, HEIGHT);
    }
    
    void TopDieInstanceItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) {
        painter->setBrush(COLOR);
        QFont font = painter->font();
        font.setPointSize(FONT_SIZE);
        painter->setFont(font);
        painter->drawRect(-WIDTH / 2., -HEIGHT / 2., WIDTH, HEIGHT);
        painter->drawText(QPointF{
            NAME_AREA_BEGIN_X - WIDTH/2, 
            NAME_AREA_BEGIN_Y - HEIGHT/2,
            }, 
            this->_name);
    }

    void TopDieInstanceItem::mousePressEvent(QGraphicsSceneMouseEvent* event) {
        this->setZValue(MOVING_Z_VALUE);
        if (event->button() == Qt::LeftButton) {
            this->_originPos = event->scenePos();
        }
        QGraphicsItem::mousePressEvent(event);
    }

    void TopDieInstanceItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event) {
        this->setZValue(Z_VALUE);
        QGraphicsItem::mouseReleaseEvent(event);

        // With any TOBItem ?
        auto collidingItems = scene()->collidingItems(this);
        for (auto item : collidingItems) {
            if (item->type() == TOBItem::Type) {
                auto tob = dynamic_cast<TOBItem*>(item);
                this->placeInTOB(tob);
                return;
            }
        }

        // No touch any tobitem
        this->removeCurrentTOB();
        emit this->placedTOBChanged(this->_currentTOB, nullptr);
    }

    auto TopDieInstanceItem::placeInTOB(TOBItem* tob) -> bool {
        assert(tob != nullptr);
        // Target tob has no topdie instance!
        if (!tob->hasTopDieInst()) {
            auto originTOB = this->_currentTOB;
            this->removeCurrentTOB();
            
            // Link to this tob
            tob->setTopDieInst(this);
            this->setPos(tob->pos());
            this->_currentTOB = tob;

            // Set basedie data
            assert(this->_topdieInst != nullptr);
            assert(tob->tob() != nullptr);
            this->_topdieInst->set_placed_tob(tob->tob());

            emit this->placedTOBChanged(originTOB, tob);

            return true;

        // Target tob has placed with other topdie instance!
        } else {
            if (this->_currentTOB != nullptr) {
                this->setPos(this->_currentTOB->pos());
            } else {
                this->setPos(this->_originPos);
            }

            return false;
        }
    }

    void TopDieInstanceItem::removeCurrentTOB() {
        if (this->_currentTOB != nullptr) {
            this->_currentTOB->removeTopDieInstance();
            this->_currentTOB = nullptr;
            this->_topdieInst->set_placed_tob(nullptr);
        }
    }
}