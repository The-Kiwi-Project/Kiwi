#pragma once

#include "./pinitem.h"
#include "./tobitem.h"
#include <circuit/topdieinst/topdieinst.hh>
#include <hardware/tob/tob.hh>
#include <hardware/bump/bump.hh>

#include <QGraphicsItem>
#include <QPainter>

namespace kiwi::widget::layout {
    
    class TopDieInstanceItem : public QObject, public QGraphicsItem {
        Q_OBJECT

    public:
        static constexpr qreal WIDTH  = 500.;
        static constexpr qreal HEIGHT  = 500.;
        static constexpr int   Z_VALUE = TOBItem::Z_VALUE + 1;
        static constexpr int   MOVING_Z_VALUE = Z_VALUE + 1;

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
        static constexpr int FONT_SIZE = 30;
    
        static_assert(WIDTH > BUMP_AREA_WIDTH);
        static_assert(HEIGHT > BUMP_AREA_HEIGHT);
        static_assert(WIDTH > BUMP_AREA_BEGIN_X + BUMP_AREA_WIDTH);
        static_assert(HEIGHT > BUMP_AREA_BEGIN_Y + BUMP_AREA_HEIGHT);

        static_assert(BUMP_ARRAY_COL * BUMP_ARRAY_ROW == hardware::TOB::INDEX_SIZE);
        static_assert(BUMP_HORI_INTERVAL > PinItem::DIAMETER);
        static_assert(BUMP_VERT_INTERVAL > PinItem::DIAMETER);

        enum { Type = UserType + 7 };
        int type() const override { return Type; }

    public:
        TopDieInstanceItem(circuit::TopDieInstance* topdieInst, TOBItem* tob);

    public:
        auto boundingRect() const -> QRectF override;
        void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) override;

    protected:
        void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
        void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;

    private:
        void tryPlaceInTOB(TOBItem* tobItem);
        void placeToIdleTOB(TOBItem* tob); 
        void swapTOBWith(TopDieInstanceItem* other);
        void updatePos();

    public:
        auto pins() const -> const QVector<PinItem*> 
        { return this->_pins; }

    private:
        circuit::TopDieInstance* const _topdieInst;
        QString _name;

        TOBItem* _currentTOB {nullptr};

        QVector<PinItem*> _pins {};
        QPointF _originPos {};
    };

}