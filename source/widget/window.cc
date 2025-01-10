#include "./window.h"
#include "./view3d/view3dwidget.h"
#include "./view2d/view2dview.h"
#include "./layout/layoutview.h"
#include "./layout/layoutview.h"
#include "./schematic/schematicview.h"
#include "./schematic/schematicwidget.h"


#include <QDebug>
#include <QResizeEvent>
#include <QVBoxLayout>

namespace kiwi::widget {

    Window::Window(hardware::Interposer* i, circuit::BaseDie* b, QWidget *parent)
        : QWidget{parent}
    {
        this->resize(1000, 800);
        auto layout = new QVBoxLayout{this};
        layout->setContentsMargins(10, 10, 10, 10);

        // this->_widget = new View3DWidget {i, b, this};
        // this->_widget = new View2DView {i, b, this};
        this->_widget = new LayoutView {i, b, this};
        // this->_widget = new SchematicWidget {i, b, this};
        
        layout->addWidget(this->_widget);
        this->setLayout(layout);
    }

    Window::~Window() {}

}
