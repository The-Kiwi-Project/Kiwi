#pragma once

#include "./griditem.h"
#include "./pinitem.h"
#include <QColor>
#include <QGraphicsItem>
#include <QPainter>

namespace kiwi::widget {
   class SchematicScene;
}

namespace kiwi::widget::schematic {

    class ExPortItem : public GridItem {
    public:
        static constexpr int WIDTH_SIZE = 2;
        static constexpr int HEIGHT_SIZE = 2;
    
        static constexpr qreal WIDTH  = GridItem::gridLength(WIDTH_SIZE);
        static constexpr qreal HEIGHT = GridItem::gridLength(HEIGHT_SIZE);
        static constexpr qreal PIN_SIDE_INTERVAL = HEIGHT / 2;
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