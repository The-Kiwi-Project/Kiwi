#pragma once

#include "./griditem.h"
#include "./pinitem.h"
#include "qchar.h"
#include "qglobal.h"
#include "qmap.h"
#include <QGraphicsItem>

namespace kiwi::circuit {
    class TopDieInstance;
}

namespace kiwi::widget {
    class SchematicScene;
}

namespace kiwi::widget::schematic {

    class TopDieInstanceItem : public GridItem {
    public:
        static constexpr int PIN_INTERVAL_SIZE = 1;
        static constexpr int SPACE_LENGTH_SIZE = 4;

        static constexpr qreal PIN_INTERVAL = GridItem::gridLength(PIN_INTERVAL_SIZE);
        static constexpr qreal SPACE_LENGTH = GridItem::gridLength(SPACE_LENGTH_SIZE);
        static constexpr qreal NAME_CHAR_HEIGHT = 20.;
        static constexpr qreal NAME_CHAR_WIDTH = 10.;

        static const     QColor COLOR;

        enum { Type = UserType + 6 };
        int type() const override { return Type; }

    public:
        TopDieInstanceItem(circuit::TopDieInstance* topdieinst);

        
    protected:
        auto boundingRect() const -> QRectF override;
        void paint(QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget) override;

    protected:
        void createPins(int n, qreal side_length, qreal x_offset, qreal y_offset, QVector<QString>::iterator& iter, PinSide side);
    
    public: 
        auto name() const -> const QString 
        { return this->_name; }
        
        auto width() const -> qreal 
        { return this->_width; }
        
        auto height() const -> qreal 
        { return this->_height; }

        auto pins() const -> const QMap<QString, PinItem*>& 
        { return this->_pins; }

        auto unwrap() const -> circuit::TopDieInstance* { return this->_topdieinstance; }

        void setName(const QString& name) { this->_name = name; }

    protected:
        QString _name {};
        qreal _width {};
        qreal _height {};

        circuit::TopDieInstance* const _topdieinstance {nullptr};
        QMap<QString, PinItem*> _pins {};
    };


}