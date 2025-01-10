#pragma once

#include "./griditem.h"
#include "./pinitem.h"
#include "qchar.h"
#include <QColor>
#include <QGraphicsItem>
#include <QPainter>

namespace kiwi::widget::schematic {

    enum class SourcePortType {
        VDD, GND
    };

    class SourcePortItem : public GridItem {
    public:
        static constexpr int WIDTH_SIZE = 2;
        static constexpr int HEIGHT_SIZE = 2;
    
        static constexpr qreal  WIDTH  = GridItem::gridLength(WIDTH_SIZE);
        static constexpr qreal  HEIGHT = GridItem::gridLength(HEIGHT_SIZE);
        static constexpr qreal  PIN_SIDE_INTERVAL = HEIGHT / 2;
        static const     QColor VDD_COLOR;
        static const     QColor GND_COLOR;

        static_assert(HEIGHT > PinItem::CHAR_HEIGHT);

        enum { Type = UserType + 7 };
        int type() const override { return Type; }
        
    public:
        SourcePortItem(const QString& name, SourcePortType type);

    protected:
        auto boundingRect() const -> QRectF override;
        void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) override;
        
    public: 
        auto name() const -> const QString& 
        { return this->_pin->name(); }
        
        auto isVDD() const -> bool 
        { return this->_type == SourcePortType::VDD; }

        auto isGND() const -> bool 
        { return this->_type == SourcePortType::GND; }

        auto portType() const -> SourcePortType
        { return this->_type; } 

        auto pin() const -> PinItem* 
        { return this->_pin; }

        auto width() const -> qreal 
        { return this->_width; }

    private:
        PinItem* _pin;
        SourcePortType _type;

        qreal _width;
    };


}