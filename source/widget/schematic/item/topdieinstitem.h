#pragma once

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

    class TopDieInstanceItem : public QGraphicsItem {
    public:
        static constexpr qreal PIN_INTERVAL = 20.;
        static constexpr qreal SPACE_LENGTH = 70.;
        static constexpr qreal NAME_CHAR_HEIGHT = 20.;
        static constexpr qreal NAME_CHAR_WIDTH = 10.;

        static const     QColor COLOR;

    public:
        TopDieInstanceItem(circuit::TopDieInstance* topdieinst, SchematicScene* scene);

        QRectF boundingRect() const override;
        void paint(QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget) override;

    protected:
        void createPins(int n, qreal side_length, qreal x_offset, qreal y_offset, QVector<QString>::iterator& iter, PinSide side);
    
    public: 
        auto width() const -> qreal { return this->_width; }
        auto height() const -> qreal { return this->_height; }
        auto pinitems() const -> const QMap<QString, PinItem*>& { return this->_pinitems; }

    protected:
        circuit::TopDieInstance* _topdieinst {nullptr};

        QMap<QString, PinItem*> _pinitems {};

        QString _name {};
        qreal _width {};
        qreal _height {};

        SchematicScene* const _scene {};
    };


}