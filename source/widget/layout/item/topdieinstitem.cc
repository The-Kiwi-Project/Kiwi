#include "./topdieinstitem.h"
#include "./pinitem.h"
#include "qchar.h"
#include "qpoint.h"
#include "./tobitem.h"
#include "widget/layout/layoutscene.h"
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

    TopDieInstanceItem::TopDieInstanceItem(circuit::TopDieInstance* topdieInst, TOBItem* tob): 
        _topdieInst{topdieInst},
        _name{ topdieInst != nullptr ? QString::fromStdString(topdieInst->name().data()) : "" },
        _currentTOB{tob}
    {   
        assert(this->_topdieInst != nullptr);
        assert(this->_currentTOB != nullptr);

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

        tob->setTopDieInstance(this);
        this->updatePos();
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
                this->tryPlaceInTOB(tob);
                return;
            }
        }

        // Nothing happend!
        assert(this->_currentTOB != nullptr);
        this->setPos(this->_currentTOB->pos());
    }

    void TopDieInstanceItem::tryPlaceInTOB(TOBItem* tob) {
        // This method try to place this topdie instance to tobitem
        assert(tob != nullptr);

        // Target tob has no topdie instance!
        if (!tob->hasTopDieInstance()) {
            this->placeToIdleTOB(tob);
        }
        // Target tob has topdie instance!
        else {
            auto otherInstance = tob->placedTobDieInstance();
            assert(otherInstance->_currentTOB == tob);
            this->swapTOBWith(otherInstance);
        }

        assert(this->scene() != nullptr);
        emit dynamic_cast<LayoutScene*>(this->scene())->layoutChanged();
    }

    void TopDieInstanceItem::swapTOBWith(TopDieInstanceItem* other) {
        auto thisTOB = this->_currentTOB;
        auto otherTOB = other->_currentTOB;

        assert(thisTOB != nullptr && otherTOB != nullptr);
        assert(thisTOB->placedTobDieInstance() == this);
        assert(otherTOB->placedTobDieInstance() == other);

        this->_currentTOB = otherTOB;
        other->_currentTOB = thisTOB;

        thisTOB->setTopDieInstance(other);
        otherTOB->setTopDieInstance(this);

        this->updatePos();
        other->updatePos();

        this->_topdieInst->swap_tob_with_(other->_topdieInst);
    }

    void TopDieInstanceItem::placeToIdleTOB(TOBItem* tob) {
        auto originTOB = this->_currentTOB;
        assert(originTOB != nullptr);

        // Origin TOB become empty!
        originTOB->removeTopDieInstance();

        // Move to new TOB
        this->_currentTOB = tob;
        tob->setTopDieInstance(this);

        this->updatePos();

        this->_topdieInst->place_to_idle_tob(tob->unwrap());
    }

    void TopDieInstanceItem::updatePos() {
        assert(this->_currentTOB != nullptr);
        this->setPos(this->_currentTOB->scenePos());
    }

}