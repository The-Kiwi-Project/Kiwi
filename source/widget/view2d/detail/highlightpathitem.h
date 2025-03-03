#include <QApplication>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsPathItem>
#include <QPen>

namespace kiwi::widget::view2d {

    class HighLightPathItem : public QGraphicsPathItem {
    public:
        explicit HighLightPathItem(QGraphicsItem* parent = nullptr)
            : QGraphicsPathItem(parent) {
            setPen(QPen(Qt::black, 2));
            setAcceptHoverEvents(true);
        }

    protected:
        void hoverEnterEvent(QGraphicsSceneHoverEvent* event) override {
            setPen(QPen(Qt::red, 5));
            QGraphicsPathItem::hoverEnterEvent(event);
        }

        void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override {
            setPen(QPen(Qt::black, 2));
            QGraphicsPathItem::hoverLeaveEvent(event);
        }
    };

}
