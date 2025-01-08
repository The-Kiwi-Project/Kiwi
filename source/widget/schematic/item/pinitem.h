#pragma once

#include <QColor>
#include <QGraphicsItem>
#include <QPainter>
#include <QDebug>
#include <circuit/connection/pin.hh>

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
    class TopDieInstanceItem;

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
            const QString &name, 
            QPointF position, 
            PinSide side, 
            SchematicScene* scene, 
            TopDieInstanceItem* topdie = nullptr,
            QGraphicsItem *parent = nullptr
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
        auto isExportPin() const -> bool;
        auto isTopdieInstPin() const -> bool;
        auto toString() const -> QString;
        auto toCircuitPin() const -> circuit::Pin;

    public: 
        auto name() const -> const QString& { return this->_name; }
        void setName(const QString& name) { this->_name = name; }

        void addConnectedPoint(NetPointItem* point) 
        { this->_connectedNetPoints.push_back(point); }

        void removeConnectedPoint(NetPointItem* point)
        { this->_connectedNetPoints.removeOne(point); }

        auto topDieInstanceItem() const -> TopDieInstanceItem* 
        { return this->_topdieinst; }

        void setRaduis(qreal radius) { this->_raduis = radius; }
        void resetRaduis() { this->_raduis = PIN_RADIUS; }

    private:
        QString _name;
        PinSide _side;
        TopDieInstanceItem* _topdieinst;
        SchematicScene* _scene;

        qreal _raduis {PIN_RADIUS};
        bool _hovered {false};

        QVector<NetPointItem*> _connectedNetPoints {};
    };


}