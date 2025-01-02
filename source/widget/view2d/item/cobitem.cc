#include "./cobitem.h"
#include "qcolor.h"
#include "qnamespace.h"

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
    }

    auto COBItem::boundingRect() const -> QRectF {
        return QRectF(-CORE_WIDTH / 2., -CORE_WIDTH / 2., CORE_WIDTH, CORE_WIDTH);
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


}