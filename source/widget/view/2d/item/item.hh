#pragma once

#include "qpoint.h"
#include <QPainter>

namespace kiwi::widget {

    class Item {
    public:
        Item(const QPoint& center, int width, int height);
    
    public:
        virtual void paintSelf(QPainter& painter) const = 0;
        virtual void paintPointed(QPainter& painter) const;
        virtual void onClicked() {};

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