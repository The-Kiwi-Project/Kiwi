#pragma once

#include "./pinitem.h"
#include "qchar.h"
#include <QGraphicsItem>

namespace kiwi::circuit {
    class TopDieInstance;
}

namespace kiwi::widget {

    class TopDieInstanceItem : public QGraphicsItem {
    public:
        static constexpr qreal PIN_INTERVAL = 20.;
        static constexpr qreal SPACE_LENGTH = 70.;
        static constexpr qreal NAME_CHAR_HEIGHT = 20.;
        static constexpr qreal NAME_CHAR_WIDTH = 10.;

    public:
        TopDieInstanceItem(circuit::TopDieInstance* topdieinst);

        QRectF boundingRect() const override;
        QPainterPath shape() const override;
        void paint(QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget) override;

    protected:
        void createPins(int n, qreal side_length, qreal x_offset, qreal y_offset, int &index, PinSide side);
    
    protected:
        circuit::TopDieInstance* _topdieinst {nullptr};
        QVector<QString> _pinNames {};

        QString _name {};
        qreal _width {};
        qreal _height {};
    };


}