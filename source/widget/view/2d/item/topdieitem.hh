#pragma once

#include "./item.hh"
#include "qcolor.h"
#include "qpoint.h"
#include <QPainter>

namespace kiwi::widget {

    class TopDieInstItem : public Item {
    public:
        enum {
            WIDTH = 150,
        };

        static QColor color;

    public: 
        TopDieInstItem(const QPoint& offset);

    public: 
        virtual void paintSelf(QPainter& painter) const override;
    };

}