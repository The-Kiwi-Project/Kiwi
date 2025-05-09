#pragma once

#include "qglobal.h"
#include "qgraphicsitem.h"
#include "qpoint.h"
#include <QPointF>
#include <cmath>
#include <QDebug>

namespace kiwi::widget::schematic {

    class GridItem : public QGraphicsItem {
    public:
        static constexpr qreal GRID_SIZE = 20.;

        static constexpr auto round(qreal value) -> qreal {
            return (value > 0.0) ? std::floor(value + 0.5) : std::ceil(value - 0.5);
        }

        static constexpr auto snapToGrid(qreal x) -> qreal {
            return round(x / GRID_SIZE) * GRID_SIZE;
        }

        static constexpr auto gridLength(int size) -> qreal {
            return size * GRID_SIZE;
        }

        static auto snapToGrid(const QPointF& point) -> QPointF {
            qreal x = std::round(point.x() / GRID_SIZE) * GRID_SIZE;
            qreal y = std::round(point.y() / GRID_SIZE) * GRID_SIZE;
            return QPointF(x, y);
        }

    public:
        explicit GridItem(QGraphicsItem *parent = nullptr) : 
            QGraphicsItem{parent} 
        {
            this->setAcceptHoverEvents(true);
            this->setFlags(QGraphicsItem::ItemSendsScenePositionChanges);
        }
        
        virtual ~GridItem() override {

        }

    protected:
        QVariant itemChange(GraphicsItemChange change, const QVariant& value) override {
            if (change == GraphicsItemChange::ItemPositionChange) {
                QPointF pos = value.toPointF();
                return GridItem::snapToGrid(pos);  // 对齐到网格
            }
            return QGraphicsItem::itemChange(change, value);
        }
    };

}