#pragma once

#include "./pinitem.h"
#include <QColor>
#include <QGraphicsItem>
#include <QPainter>

namespace kiwi::widget {
   class SchematicScene;
}

namespace kiwi::widget::schematic {

    class ExPortItem : public QGraphicsItem {
    public:
        static constexpr qreal WIDTH  = 50.;
        static constexpr qreal HEIGHT = 50.;
        static constexpr qreal PIN_SIDE_INTERVAL = HEIGHT / 2.;
        static const     QColor COLOR;

        static_assert(HEIGHT > PinItem::CHAR_HEIGHT);
        
    public:
        ExPortItem(const QString name, SchematicScene* scene);

    protected:
        auto boundingRect() const -> QRectF override;
        void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) override;
        
    public: 
        auto pin() -> PinItem* { return this->_pinietm; }

    private:
        PinItem* _pinietm;
        qreal _width;
    };


}