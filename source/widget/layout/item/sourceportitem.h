#pragma once

#include <QGraphicsItem>
#include <QPainter>
#include "./pinitem.h"

namespace kiwi::widget::layout {

    enum class SoucePortType {
        VDD, GND
    };

    class SourcePortItem : public QGraphicsItem {
    public:
        static constexpr qreal WIDTH = PinItem::DIAMETER + 2;
        static const     QColor VDD_COLOR;
        static const     QColor GND_COLOR;
        static constexpr int   Z_VALUE = 6;

    public:
        SourcePortItem(SoucePortType type);

    protected:
        auto boundingRect() const -> QRectF override;
        void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) override;

    public:
        auto pin() const -> PinItem* 
        { return this->_pin; }

    private:
        SoucePortType _type;
        PinItem* _pin {nullptr};
    };

}