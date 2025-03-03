#include "./cobitem.h"
#include "../detail/cob.h"
#include "qboxlayout.h"
#include "qdialog.h"
#include "qgraphicsitem.h"
#include <widget/frame/graphicsview.h>
#include <QDebug>

namespace kiwi::widget::view2d {

    /*
    
           +------------------------------+
           |                              |
        +--+------------------------------+--+
        |                                    |
    +---+                                    +---+
    |   |                                    |   | 
    |   |                                    |   | 
    |   |                                    |   | 
    |   |                                    |   | 
    |   |                                    |   | 
    |   |               C O B                |   | 
    |   |                                    |   | 
    |   |                                    |   | 
    |   |                                    |   | 
    |   |                                    |   | 
    |   |                                    |   | 
    +---+                                    +---+
        |                                    |
        +------------------------------------+
           |                              |
           +------------------------------+
    
    */

    const QColor COBItem::CORE_COLOR = QColor::fromRgb(205, 173, 0);
    const QColor COBItem::TRANFORM_COLOR = Qt::black;

    COBItem::COBItem(hardware::COB* cob): 
        _cob{cob} 
    {
        this->setFlags(this->flags() | QGraphicsItem::ItemIsMovable);
    }

    auto COBItem::boundingRect() const -> QRectF {
        return QRectF(-WIDTH / 2., -WIDTH / 2., WIDTH, WIDTH);
    }
    
    void COBItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) {
        painter->setBrush(COBItem::CORE_COLOR);
        painter->drawRect(-CORE_WIDTH / 2., -CORE_WIDTH / 2., CORE_WIDTH, CORE_WIDTH);
        
        painter->setBrush(COBItem::TRANFORM_COLOR);
        // Right
        painter->drawRect(
            CORE_WIDTH / 2., -TRANFORM_LENGTH / 2., 
            TRANFORM_WIDTH, TRANFORM_LENGTH
        );

        // Left
        painter->drawRect(
            -CORE_WIDTH / 2. - TRANFORM_WIDTH, -TRANFORM_LENGTH / 2., 
            TRANFORM_WIDTH, TRANFORM_LENGTH
        );

        // Up
        painter->drawRect(
            -TRANFORM_LENGTH / 2., -CORE_WIDTH / 2. - TRANFORM_WIDTH, 
            TRANFORM_LENGTH, TRANFORM_WIDTH
        );

        // Down
        painter->drawRect(
            -TRANFORM_LENGTH / 2., CORE_WIDTH / 2., 
            TRANFORM_LENGTH, TRANFORM_WIDTH
        );

    }

    void COBItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event) {
        auto scene = widget::view2d::COBScene{this->_cob};

        auto dialog = QDialog {};
        auto layout = new QVBoxLayout{&dialog};

        auto view = new widget::GraphicsView {};
        view->setScene(&scene);
        view->setRenderHint(QPainter::Antialiasing); // 抗锯齿
        view->setWindowTitle("COB Unit Layout");
        view->resize(820, 820);
        view->adjustSceneRect();

        layout->addWidget(view);

        dialog.exec();
    }

}