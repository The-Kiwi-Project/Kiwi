#include "./layoutwidget.h"
#include "./layoutscene.h"
#include "./layoutview.h"
#include "./layoutinfowidget.h"

#include <QSplitter>
#include <QVBoxLayout>
#include <QDebug>

namespace kiwi::widget {

    using namespace layout;

    LayoutWidget::LayoutWidget(hardware::Interposer* interposer, circuit::BaseDie* basedie, QWidget* parent):
        QWidget{parent},
        _interposer{interposer},
        _basedie{basedie}
    {
        this->_scene = new LayoutScene {this->_interposer, this->_basedie, this};
        // MARK
        this->_scene->setItemIndexMethod(QGraphicsScene::NoIndex);

        auto* layout = new QVBoxLayout(this);
        layout->setContentsMargins(10, 10, 10, 10);

        this->_splitter = new QSplitter{Qt::Horizontal, this};
        layout->addWidget(this->_splitter);

        // View
        this->_view = new LayoutView {this->_splitter};
        this->_view->setScene(this->_scene);
        this->_view->adjustSceneRect();
        this->_view->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        this->_splitter->addWidget(this->_view);

        // Info
        this->_infoWidget = new LayoutInfoWidget {this->_interposer, this->_basedie, this->_scene, this->_splitter};
        this->_infoWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
        this->_infoWidget->setMinimumWidth(300);
        this->_splitter->addWidget(this->_infoWidget);

        connect(this->_scene, &LayoutScene::layoutChanged, [this]() {
                this->_scene->choiseSourcePort();
                this->_infoWidget->updateInfo();
            }
        );

        this->setWindowTitle("Layout Editor");
        this->resize(1000, 800);
    }

    void LayoutWidget::reload() {
        this->_scene->reloadItems();
        this->_view->adjustSceneRect();
        this->_infoWidget->updateInfo();
    }

}