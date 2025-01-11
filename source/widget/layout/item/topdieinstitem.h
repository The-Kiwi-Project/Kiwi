#pragma once

#include "./pinitem.h"
#include "circuit/topdieinst/topdieinst.hh"
#include "qchar.h"
#include "qpoint.h"
#include "qvector.h"
#include <hardware/tob/tob.hh>

#include <QGraphicsItem>
#include <QPainter>

namespace kiwi::widget::layout {
    
    class TOBItem;

    class TopDieInstanceItem : public QGraphicsItem {
    public:
        static constexpr qreal WIDTH  = 500.;
        static constexpr qreal HEIGHT  = 500.;

        static constexpr qreal BUMP_AREA_WIDTH  = 220.;
        static constexpr qreal BUMP_AREA_HEIGHT = 220.;

        static constexpr qreal BUMP_AREA_BEGIN_X = 250.;
        static constexpr qreal BUMP_AREA_BEGIN_Y = 250.;

        static constexpr qreal NAME_AREA_BEGIN_X = 100.;
        static constexpr qreal NAME_AREA_BEGIN_Y = 100.;
        
        static constexpr int BUMP_ARRAY_COL = 16;
        static constexpr int BUMP_ARRAY_ROW = 8;

        static constexpr qreal BUMP_HORI_INTERVAL = BUMP_AREA_WIDTH  / (BUMP_ARRAY_COL - 1);
        static constexpr qreal BUMP_VERT_INTERVAL = BUMP_AREA_HEIGHT / (BUMP_ARRAY_ROW - 1);

        static const    QColor COLOR;
    
        static_assert(WIDTH > BUMP_AREA_WIDTH);
        static_assert(HEIGHT > BUMP_AREA_HEIGHT);
        static_assert(WIDTH > BUMP_AREA_BEGIN_X + BUMP_AREA_WIDTH);
        static_assert(HEIGHT > BUMP_AREA_BEGIN_Y + BUMP_AREA_HEIGHT);

        static_assert(BUMP_ARRAY_COL * BUMP_ARRAY_ROW == hardware::TOB::INDEX_SIZE);
        static_assert(BUMP_HORI_INTERVAL > PinItem::DIAMETER);
        static_assert(BUMP_VERT_INTERVAL > PinItem::DIAMETER);

    public:
        TopDieInstanceItem(circuit::TopDieInstance* topdieInst);

    public:
        auto boundingRect() const -> QRectF override;
        void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) override;

    protected:
        void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
        void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
        // auto itemChange(GraphicsItemChange change, const QVariant& value) -> QVariant override;

    public:
        auto placeInTOB(TOBItem* tob) -> bool; 

    public:
        auto pins() const -> const QVector<PinItem*> 
        { return this->_pins; }

    private:
        circuit::TopDieInstance* _topdieInst;
        QString _name;

        TOBItem* _currentTOB {nullptr};

        QVector<PinItem*> _pins {};
        QPointF _originPos {};
    };

}