#pragma once

#include "./item.hh"
#include "qcolor.h"
#include "qpoint.h"
#include <QPainter>

namespace kiwi::widget {

    class COBItem : public Item {
    public:
        enum {
            WIDTH = 30,
        };

        static QColor color;

    public: 
        COBItem(const QPoint& offset);

    public: 
        virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) override;
    };

}