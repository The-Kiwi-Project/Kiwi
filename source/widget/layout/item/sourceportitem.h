#pragma once

#include <QGraphicsItem>
#include <QPainter>
#include "./pinitem.h"

namespace kiwi::widget::layout {

    enum class SourcePortType {
        VDD, GND
    };

    class SourcePortItem : public QGraphicsItem {
    public:
        static constexpr qreal WIDTH = PinItem::DIAMETER + 2;
        static const     QColor VDD_COLOR;
        static const     QColor GND_COLOR;
        static constexpr int   Z_VALUE = 6;

        enum { Type = UserType + 5 };
        int type() const override { return Type; }

    public:
        SourcePortItem(SourcePortType type);

    protected:
        auto boundingRect() const -> QRectF override;
        void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) override;

    public:
        auto isVDD() -> bool
        { return this->_type == SourcePortType::VDD; }

        auto isGND() -> bool 
        { return this->_type == SourcePortType::GND; }

        auto pin() const -> PinItem* 
        { return this->_pin; }

    private:
        SourcePortType _type;
        PinItem* _pin {nullptr};
    };

}