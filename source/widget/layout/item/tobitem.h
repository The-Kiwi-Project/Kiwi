#pragma once

#include "qgraphicsitem.h"
#include <hardware/tob/tob.hh>

#include <QGraphicsItem>
#include <QPainter>

namespace kiwi::widget::layout {

    class TopDieInstanceItem;

    class TOBItem : public QGraphicsRectItem {
    public:
        static constexpr int    Z_VALUE = 0;
        static constexpr int    FONT_SIZE = 30;

        enum { Type = UserType + 6 };
        int type() const override { return Type; }

    public:
        static constexpr qreal WIDTH  = 510.;
        static constexpr qreal HEIGHT = 510.;
        static const    QColor COLOR;
        static const    QColor HIGHLIGHT_COLOR;
        static const    QColor WITH_TOPDIE_INST_COLOR;

    public:
        TOBItem(hardware::TOB* tob);

    protected:
        void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) override;

    public:
        void setTopDieInstance(TopDieInstanceItem* topdieinst);
        void removeTopDieInstance();
        bool hasTopDieInstance() const;
        auto placedTobDieInstance() const -> TopDieInstanceItem*;

        void highlight(bool active);

    private:
        void updateAppearance();
        
    public:
        auto unwrap() const -> hardware::TOB*
        { return this->_tob; }

    private:
        hardware::TOB* const _tob;

        TopDieInstanceItem* _topdieInstItem {nullptr};

    };

}