#pragma once

#include "./griditem.h"
#include "./pinitem.h"
#include "qchar.h"
#include <QColor>
#include <QGraphicsItem>
#include <QPainter>

namespace kiwi::circuit {
    class ExternalPort;
}

namespace kiwi::widget::schematic {

    class ExternalPortItem : public GridItem {
    public:
        static constexpr int WIDTH_SIZE = 2;
        static constexpr int HEIGHT_SIZE = 2;
    
        static constexpr qreal WIDTH  = GridItem::gridLength(WIDTH_SIZE);
        static constexpr qreal HEIGHT = GridItem::gridLength(HEIGHT_SIZE);
        static constexpr qreal PIN_SIDE_INTERVAL = HEIGHT / 2;
        static const     QColor COLOR;

        static_assert(HEIGHT > PinItem::CHAR_HEIGHT);

        enum { Type = UserType + 2 };
        int type() const override { return Type; }
        
    public:
        ExternalPortItem(circuit::ExternalPort* eport);

    protected:
        auto boundingRect() const -> QRectF override;
        void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) override;
        
    public: 
        auto name() const -> const QString& 
        { return this->_pin->name(); }
        
        void setName(const QString& name) 
        { this->_pin->setName(name); }

        auto pin() const -> PinItem* 
        { return this->_pin; }

        auto unwrap() const -> circuit::ExternalPort* { 
            assert(this->_externalPort != nullptr);
            return this->_externalPort;
        }

    private:
        circuit::ExternalPort* _externalPort;
        PinItem* _pin;

        qreal _width;
    };


}