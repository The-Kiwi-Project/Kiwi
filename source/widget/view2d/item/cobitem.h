#pragma once

#include <QGraphicsItem>
#include <QPainter>

namespace kiwi::hardware {
    class COB;
}

namespace kiwi::widget::view2d {

    class COBItem : public QGraphicsItem {
    public:
        static constexpr qreal CORE_WIDTH = 200.;
        static constexpr qreal TRANFORM_LENGTH = 180.;
        static constexpr qreal TRANFORM_WIDTH = 10.;
        static constexpr qreal WIDTH = CORE_WIDTH + 2 * TRANFORM_WIDTH;

        static_assert(CORE_WIDTH > TRANFORM_LENGTH);

        static const    QColor CORE_COLOR;
        static const    QColor TRANFORM_COLOR;
    
    public:
        COBItem(hardware::COB* cob);

    public:
        auto boundingRect() const -> QRectF override;
        void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) override;

    private:
        hardware::COB* _cob;

    };

}