#pragma once

#include "./bumpitem.h"
#include <hardware/tob/tob.hh>

#include <QGraphicsItem>
#include <QPainter>

namespace kiwi::widget::view2d {

    class TOBItem : public QGraphicsItem {
    public:
        static constexpr qreal WIDTH  = 250.;
        static constexpr qreal HEIGHT = 150.;
        static constexpr qreal BUMP_AREA_WIDTH  = 220.;
        static constexpr qreal BUMP_AREA_HEIGHT = 120.;
        
        static constexpr int BUMP_ARRAY_COL = 16;
        static constexpr int BUMP_ARRAY_ROW = 8;

        static constexpr qreal BUMP_HORI_INTERVAL = BUMP_AREA_WIDTH  / (BUMP_ARRAY_COL - 1);
        static constexpr qreal BUMP_VERT_INTERVAL = BUMP_AREA_HEIGHT / (BUMP_ARRAY_ROW - 1);

        static const    QColor COLOR;
    
        static_assert(WIDTH > BUMP_AREA_WIDTH);
        static_assert(HEIGHT > BUMP_AREA_HEIGHT);
        static_assert(BUMP_ARRAY_COL * BUMP_ARRAY_ROW == hardware::TOB::INDEX_SIZE);
        static_assert(BUMP_HORI_INTERVAL > BumpItem::DIAMETER);
        static_assert(BUMP_VERT_INTERVAL > BumpItem::DIAMETER);

    public:
        TOBItem(hardware::TOB* tob);

    public:
        auto boundingRect() const -> QRectF override;
        void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) override;

    private:
        hardware::TOB* _tob;

    };

}