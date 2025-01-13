#pragma once

#include <QGraphicsItem>
#include <QPainter>
#include "./pinitem.h"

namespace kiwi::circuit {
    class ExternalPort;
}

namespace kiwi::widget::layout {

    class ExternalPortItem : public QGraphicsItem {
    public:
        static constexpr qreal WIDTH = PinItem::DIAMETER + 2;
        static const    QColor COLOR;
        static constexpr int   Z_VALUE = 6;

        enum { Type = UserType + 2 };
        int type() const override { return Type; }

    public:
        ExternalPortItem(circuit::ExternalPort* eport);

    protected:
        auto boundingRect() const -> QRectF override;
        void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) override;

    public:
        auto pin() const -> PinItem* 
        { return this->_pin; }

    private:
        circuit::ExternalPort* _eport {nullptr};
        PinItem* _pin {nullptr};
    };

}