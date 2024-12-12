#pragma once

#include "./item.hh"
#include "qcolor.h"
#include "qpoint.h"
#include <QPainter>

namespace kiwi::widget {

    class TOBItem : public Item {
    public:
        enum {
            WIDTH = 40,
        };

        static QColor color;

    public: 
        TOBItem(const QPoint& offset);

    public: 
        virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) override;
    };

}