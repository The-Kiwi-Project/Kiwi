#pragma once

#include "qpoint.h"
#include <QPainter>
#include <QGraphicsItem>

namespace kiwi::widget {

    class Item : public QGraphicsItem {
    public:
        Item(const QPoint& center, int width, int height);
    
    public:
        virtual void paintPointed(QPainter& painter) const;
        virtual auto boundingRect() const -> QRectF override;

    public:
        auto isPointed(const QPoint& pointed) -> bool;
        void moveTo(const QPoint& offset);

        auto leftdown() const -> QPoint;
        auto rightup() const -> QPoint;

    protected:
        QPoint _center;
        int _width;
        int _height;
    };

}