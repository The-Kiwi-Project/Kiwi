#include "./pinitem.h"
#include "./netitem.h"
#include "./exportitem.h"
#include "./sourceportitem.h"
#include "./topdieinstitem.h"
#include <cassert>
#include <hardware/bump/bump.hh>

namespace kiwi::widget::layout {

    const QColor PinItem::COLOR = Qt::black;
    const QColor PinItem::HOVERED_COLOR = Qt::red;

    PinItem::PinItem(QPointF position, QGraphicsItem *parent) :
        QGraphicsEllipseItem{parent}
    {
        this->setRect(-RADIUS, -RADIUS, DIAMETER, DIAMETER);
        this->setPos(position);
        this->setBrush(COLOR); 
        this->setAcceptHoverEvents(true);
        this->setFlags(ItemSendsScenePositionChanges | ItemIsSelectable);
        this->setZValue(Z_VALUE);
    }

    auto PinItem::isExternalPortPin() const -> bool {
        assert(this->parentItem() != nullptr);
        return this->parentItem()->type() == ExternalPortItem::Type;
    }

    auto PinItem::isSourcePortPin() const -> bool {
        assert(this->parentItem() != nullptr);
        return this->parentItem()->type() == SourcePortItem::Type;
    }

    auto PinItem::isTopDieInstancePin() const -> bool {
        assert(this->parentItem() != nullptr);
        return this->parentItem()->type() == TopDieInstanceItem::Type;
    }

    auto PinItem::parentExternalPort() const -> ExternalPortItem* {
        assert(this->isExternalPortPin());
        return dynamic_cast<ExternalPortItem*>(this->parentItem());
    }

    auto PinItem::parentSourcePort() const -> SourcePortItem* {
        assert(this->isSourcePortPin());
        return dynamic_cast<SourcePortItem*>(this->parentItem());
    }

    auto PinItem::parentTopDieInsance() const -> TopDieInstanceItem* {
        assert(this->isTopDieInstancePin());
        return dynamic_cast<TopDieInstanceItem*>(this->parentItem());
    }

    void PinItem::hoverEnterEvent(QGraphicsSceneHoverEvent * e) {
        this->setBrush(HOVERED_COLOR); 
        QGraphicsEllipseItem::hoverEnterEvent(e);
    }

    void PinItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *e) {
        this->setBrush(COLOR); 
        QGraphicsEllipseItem::hoverLeaveEvent(e);
    }
    
    auto PinItem::itemChange(GraphicsItemChange change, const QVariant& value) -> QVariant {
        if (change == QGraphicsItem::ItemScenePositionHasChanged) {
            for (auto net : this->_nets) {
                net->updateLine();
            }
        }
        return QGraphicsItem::itemChange(change, value);
    }
    
}