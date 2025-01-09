#include "./graphicsview.h"
#include "qnamespace.h"
#include <QScrollBar>

namespace kiwi::widget {

    GraphicsView::GraphicsView(QWidget* parent)
        : QGraphicsView{parent},
        _isPanning{false} 
    {
        this->setDragMode(QGraphicsView::RubberBandDrag);
        this->setInteractive(true);
        this->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    }

    void GraphicsView::adjustSceneRect() {
        if (this->items().isEmpty()) {
            this->setSceneRect(0, 0, 1000, 1000); // 如果没有 Item，设置一个默认大小
            return;
        }

        qreal minX = std::numeric_limits<qreal>::max();
        qreal minY = std::numeric_limits<qreal>::max();
        qreal maxX = std::numeric_limits<qreal>::lowest();
        qreal maxY = std::numeric_limits<qreal>::lowest();

        for (QGraphicsItem* item : this->items()) {
            QRectF itemBounds = item->sceneBoundingRect(); // 获取 Item 在场景中的边界
            minX = std::min(minX, itemBounds.left());
            minY = std::min(minY, itemBounds.top());
            maxX = std::max(maxX, itemBounds.right());
            maxY = std::max(maxY, itemBounds.bottom());
        }

        qreal margin = 3000;
        this->setSceneRect(minX - margin, minY - margin, (maxX - minX) + 2 * margin, (maxY - minY) + 2 * margin);
    }

    void GraphicsView::wheelEvent(QWheelEvent* event) {
        if (event->modifiers() & Qt::ControlModifier) {
            const double scaleFactor = 1.15;
            if (event->angleDelta().y() > 0) {
                scale(scaleFactor, scaleFactor);  // 放大
            } else {
                scale(1.0 / scaleFactor, 1.0 / scaleFactor);  // 缩小
            }
        } 
        else {
            auto vScrollBar = this->verticalScrollBar();
            if (vScrollBar) {
                int step = event->angleDelta().y() / 8 / 15; // 每次滚动的步进
                vScrollBar->setValue(vScrollBar->value() - step * vScrollBar->singleStep());
            }
        }
    }

    void GraphicsView::mousePressEvent(QMouseEvent* event) {
        if (event->button() == Qt::MiddleButton || event->button() == Qt::RightButton) {
            this->_isPanning = true;
            this->_lastMousePos = event->pos();
            this->setDragMode(QGraphicsView::NoDrag);
            this->setCursor(Qt::ClosedHandCursor);
        }

        QGraphicsView::mousePressEvent(event);
    }

    void GraphicsView::mouseMoveEvent(QMouseEvent* event) {
        if (this->_isPanning) {
            auto delta = event->pos() - this->_lastMousePos;
            this->_lastMousePos = event->pos();
            this->translate(delta.x(), delta.y());
        }

        QGraphicsView::mouseMoveEvent(event);
    }

    void GraphicsView::mouseReleaseEvent(QMouseEvent* event) {
        if (this->_isPanning == true) {
            this->_isPanning = false;
            this->setDragMode(QGraphicsView::RubberBandDrag);
            this->setCursor(Qt::ArrowCursor);
        }

        QGraphicsView::mouseReleaseEvent(event);
    }

}