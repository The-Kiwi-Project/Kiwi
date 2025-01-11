#include "./layoutview.h"
#include "./layoutscene.h"

namespace kiwi::widget {

    using namespace layout;

    const QColor LayoutView::BACK_COLOR = QColor::fromRgb(100, 200, 100);

    LayoutView::LayoutView(
        hardware::Interposer* interposer, 
        circuit::BaseDie* basedie,
        QWidget *parent
    ) :
        GraphicsView{parent},
        _interposer{interposer},
        _basedie{basedie}
    {
        this->setDragMode(QGraphicsView::RubberBandDrag);
        this->setInteractive(true);
        this->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    }

    LayoutView::~LayoutView() noexcept {}

}