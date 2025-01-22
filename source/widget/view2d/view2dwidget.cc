#include "./view2dwidget.h"
#include "./view2dview.h"
#include "./view2dscene.hh"
#include "qboxlayout.h"

namespace kiwi::widget {

    View2DWidget::View2DWidget(
        hardware::Interposer* interposer, 
        circuit::BaseDie* basedie,
        QWidget* parent
    ) :
        QWidget{parent},
        _interposer{interposer},
        _basedie{basedie}
    {
        this->_scene = new View2DScene {this->_interposer, this->_basedie};
        this->_view = new View2DView {this};
        this->_view->setScene(this->_scene);
        this->_view->adjustSceneRect();

        auto layout = new QVBoxLayout{this};
        layout->addWidget(this->_view);
    }

    void View2DWidget::reload() {
        this->_scene->reloadItems();
        this->_view->adjustSceneRect();
    }

}