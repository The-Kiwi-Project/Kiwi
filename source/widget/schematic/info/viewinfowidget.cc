#include "./viewinfowidget.h"
#include "qchar.h"
#include "qcheckbox.h"
#include "qcolor.h"
#include "qcombobox.h"
#include "qlineedit.h"
#include "qwidget.h"
#include "widget/schematic/item/griditem.h"
#include <widget/frame/colorpickbutton.h>
#include "../schematicview.h"
#include <debug/debug.hh>

#include <QLabel>
#include <QComboBox>
#include <QSpinBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QLineEdit>
#include <QCheckBox>
#include <QGroupBox>

namespace kiwi::widget::schematic {

    constexpr int MIN_HEIGHT = 30;

    ViewInfoWidget::ViewInfoWidget(SchematicView* view, QWidget* parent) :
        _view{view},
        QWidget{parent}
    {   
        if (this->_view == nullptr) {
            debug::exception("Nullptr view");
        }

        auto thisLayout = new QVBoxLayout {this};
        auto widget = new QGroupBox {"View Infomation", this};
        widget->setStyleSheet("background-color: white;");
        thisLayout->addWidget(widget);
        thisLayout->addStretch();

        auto layout = new QGridLayout{widget};
        layout->setSpacing(10);
        
        // Back color
        layout->addWidget(new QLabel {"Back Color", widget}, 0, 0);
        auto backColorButton = new ColorPickerButton {widget};
        backColorButton->setMinimumHeight(MIN_HEIGHT);
        backColorButton->setColor(this->_view->backColor());
        layout->addWidget(backColorButton, 0, 1);

        // Grid view
        layout->addWidget(new QLabel {"Grid Visible", widget}, 1, 0);
        auto gridVisibleCheckBox = new QCheckBox {widget};
        gridVisibleCheckBox->setMinimumHeight(MIN_HEIGHT);
        gridVisibleCheckBox->setChecked(this->_view->gridVisible());
        layout->addWidget(gridVisibleCheckBox, 1, 1);

        // Grid color
        layout->addWidget(new QLabel {"Grid Color", widget}, 2, 0);
        auto gridColorButton = new ColorPickerButton {widget};
        gridColorButton->setMinimumHeight(MIN_HEIGHT);
        gridColorButton->setColor(this->_view->gridColor());
        layout->addWidget(gridColorButton, 2, 1);

        // Grid size
        layout->addWidget(new QLabel {"Grid Size", widget}, 3, 0);
        auto gridSizeEdit = new QLineEdit {widget};
        auto validator = new QIntValidator(GridItem::GRID_SIZE, 100, gridSizeEdit);
        gridSizeEdit->setValidator(validator);
        gridSizeEdit->setMinimumHeight(MIN_HEIGHT);
        gridSizeEdit->setText(QString("%1").arg(this->_view->gridSize()));
        layout->addWidget(gridSizeEdit, 3, 1);
        
        layout->setColumnMinimumWidth(0, 50);
        layout->setColumnStretch(0, 0);

        // Connections
        connect(backColorButton, &ColorPickerButton::colorChanged, [this] (const QColor& color) {
            this->_view->setBackColor(color);
        });

        connect(gridVisibleCheckBox, &QCheckBox::stateChanged, [this] (int state) {
            if (state == Qt::Checked) {
                this->_view->setGridVisible(true);
                this->_view->updateBack();
            }
            else if(state == Qt::Unchecked) {
                this->_view->setGridVisible(false);
                this->_view->updateBack();
            }
            else {
                debug::unreachable();
            }
        });

        connect(gridColorButton, &ColorPickerButton::colorChanged, [this] (const QColor& color) {
            this->_view->setGridColor(color);
            this->_view->updateBack();
        });

        connect(gridSizeEdit, &QLineEdit::textChanged, [this] (const QString &text) {
            bool ok;
            auto size = text.toInt(&ok);
            if (ok) {
                this->_view->setGridSize(size);
                this->_view->updateBack();
            }
        });
    }

}