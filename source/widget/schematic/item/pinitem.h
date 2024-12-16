#pragma once

#include "qglobal.h"
#include "qvariant.h"
#include "qvector.h"
#include <QColor>
#include <QGraphicsItem>
#include <QPainter>
#include <QDebug>

namespace kiwi::widget {
   class SchematicScene;
}

namespace kiwi::widget::schematic {

    enum class PinSide {
        Top,
        Left,
        Right,
        Bottom,
    };

    class NetItem;
    class NetPointItem;

    class PinItem : public QGraphicsItem {
    public:
    
        static constexpr qreal PIN_RADIUS = 5.;
        static constexpr qreal PIN_DIAMETER = 2 * PIN_RADIUS;
        static constexpr qreal NAME_INTERVAL = 10.;
        static constexpr qreal CHAR_WIDTH_ = 10.;
        static constexpr qreal CHAR_HEIGHT = 20.;
        static const    QColor COLOR;
        static const    QColor HOVERED_COLOR;

        enum { Type = UserType + 2 };
        int type() const override { return Type; }
    
    public:
        PinItem(
            const QString &name, QPointF position, PinSide side, 
            SchematicScene* scene, QGraphicsItem *parent = nullptr
        );

    public:        
        auto boundingRect() const -> QRectF override;
        void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) override;
        auto itemChange(GraphicsItemChange change, const QVariant& value) -> QVariant override;

    protected:
        // void mouseDoubleClickEvent(QGraphicsSceneMouseEvent*) override;
        void mousePressEvent(QGraphicsSceneMouseEvent*) override;
        void hoverEnterEvent(QGraphicsSceneHoverEvent *) override;
        void hoverLeaveEvent(QGraphicsSceneHoverEvent *) override;

    public: 
        auto name() const -> const QString& { return this->_name; }

        void addConnectedPoint(NetPointItem* point) 
        { this->_connectedNetPoints.push_back(point); }

        void removeConnectedPoint(NetPointItem* point)
        { this->_connectedNetPoints.removeOne(point); }

        void setRaduis(qreal radius) { this->_raduis = radius; }
        void resetRaduis() { this->_raduis = PIN_RADIUS; }

    private:
        QString _name;
        PinSide _side;

        qreal _raduis {PIN_RADIUS};

        QVector<NetPointItem*> _connectedNetPoints {};

        bool _hovered {false};

        SchematicScene* _scene;
    };


}