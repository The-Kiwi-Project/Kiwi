#pragma once

#include <QGraphicsView>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsItem>

namespace kiwi::widget {

    class GraphicsView : public QGraphicsView {
        Q_OBJECT

    public:
        explicit GraphicsView(QWidget* parent = nullptr);

    public:
        void adjustSceneRect();

    protected:
        void wheelEvent(QWheelEvent* event) override;
        void mousePressEvent(QMouseEvent* event) override;
        void mouseMoveEvent(QMouseEvent* event) override;
        void mouseReleaseEvent(QMouseEvent* event) override;

    protected:
        bool _isPanning; 
        QPoint _lastMousePos;
    };

}