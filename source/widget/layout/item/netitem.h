#pragma once

#include <QColor>
#include <QGraphicsItem>
#include <QPainter>

namespace kiwi::widget::layout {

    class PinItem;

    class NetItem : public QGraphicsLineItem {
    public:
        static const     QColor COLOR;
        static const     QColor HOVER_COLOR;
        static constexpr qreal  WIDTH = 2;
        static constexpr qreal  HOVER_WIDTH = 3;
        static constexpr int    Z_VALUE = 5;

        enum { Type = UserType + 3 };
        int type() const override { return Type; }

    public:
        NetItem(PinItem* beginPin, PinItem* endPin, QGraphicsLineItem *parent = nullptr);

    protected:
        void hoverEnterEvent(QGraphicsSceneHoverEvent* event) override;
        void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override;

    public:
        void updateLine();

    public:
        void moveToBeginPin(PinItem* pin);
        void moveToEndPin(PinItem* pin);

    public:
        auto beginPin() const -> PinItem*
        { return this->_beginPin; }

        auto endPin() const -> PinItem*
        { return this->_endPin; }

    private:
        PinItem* _beginPin;
        PinItem* _endPin;
    };


}