#pragma once

#include "qgraphicsitem.h"
#include <hardware/tob/tob.hh>

#include <QGraphicsItem>
#include <QPainter>

namespace kiwi::widget::layout {

    class TopDieInstItem;

    class TOBItem : public QGraphicsRectItem {
    public:
        enum { Type = UserType + 3 };
        int type() const override { return Type; }

    public:
        static constexpr qreal WIDTH  = 510.;
        static constexpr qreal HEIGHT = 510.;
        static const    QColor COLOR;
        static const    QColor HIGHLIGHT_COLOR;
        static const    QColor WITH_TOPDIE_INST_COLOR;

    public:
        TOBItem(hardware::TOB* tob);

    public:
        void setTopDieInst(TopDieInstItem* topdieinst);
        void removeTopDieInst();
        bool hasTopDieInst() const;
        void highlight(bool active);

    private:
        void updateAppearance();

    private:
        hardware::TOB* _tob;

        TopDieInstItem* _topdieInstItem {nullptr};

    };

}