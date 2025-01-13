#include "./layoutinfowidget.h"
#include "./layoutscene.h"
#include "qlineedit.h"
#include <hardware/interposer.hh>
#include <circuit/basedie.hh>

#include <QLabel>
#include <QComboBox>
#include <QSpinBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QTableView>
#include <QGridLayout>
#include <QStandardItemModel>
#include <QHeaderView>
#include <QGroupBox>
#include <QDebug>
#include <QMessageBox>
#include <QLineEdit>

namespace kiwi::widget {

    constexpr int MIN_HEIGHT = 30;

    LayoutInfoWidget::LayoutInfoWidget(hardware::Interposer* interposer, circuit::BaseDie* basedie, LayoutScene* scene, QWidget* parent):
        QWidget{parent},
        _interposer{interposer},
        _basedie{basedie},
        _scene{scene}
    {
        auto thisLayout = new QVBoxLayout {this};
        auto widget = new QGroupBox {"Layout Infomation", this};
        widget->setStyleSheet("background-color: white;");
        thisLayout->addWidget(widget);
        thisLayout->addStretch();

        auto layout = new QGridLayout{widget};
        layout->setSpacing(10);

        // Size
        layout->addWidget(new QLabel {"TopDie Instance Size ", widget}, 0, 0);
        this->_topdieInstSizeSpinBox = new QSpinBox {this};
        this->_topdieInstSizeSpinBox->setMinimum(0);
        this->_topdieInstSizeSpinBox->setReadOnly(true);
        this->_topdieInstSizeSpinBox->setMaximum(hardware::Interposer::TOB_ARRAY_HEIGHT * hardware::Interposer::TOB_ARRAY_WIDTH);
        layout->addWidget(this->_topdieInstSizeSpinBox, 0, 1);

        // Layout Map
        auto label = new QLabel {"Layout Place Map ", widget};
        label->setMinimumHeight(MIN_HEIGHT);
        layout->addWidget(label, 1, 0, 1, 2);
        this->_instPlaceView = new QTableView {widget};
        this->_instPlaceView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        layout->addWidget(this->_instPlaceView, 2, 0, 1, 2);

        // Path length
        layout->addWidget(new QLabel {"Path Length", widget}, 3, 0);
        this->_pathLengthEdit = new QLineEdit {this};
        this->_pathLengthEdit->setMinimumHeight(MIN_HEIGHT);
        this->_pathLengthEdit->setReadOnly(true);
        layout->addWidget(this->_pathLengthEdit, 4, 0, 1, 2);

        layout->setColumnMinimumWidth(0, 50);
        layout->setColumnStretch(0, 0);

        this->updateInfo();
    }

    void LayoutInfoWidget::updateInfo() {
        // Instance size
        auto instSize = this->_basedie->topdie_insts().size();
        this->_topdieInstSizeSpinBox->setValue(instSize);
        this->_topdieInstSizeSpinBox->setEnabled(true);

        // Coords
        auto model = new QStandardItemModel {static_cast<int>(instSize), 2};
        model->setHorizontalHeaderLabels(QStringList{"TopDie Instance", "TOB Coord"}); 
        // Add items
        auto itemRoot = model->invisibleRootItem();
        int row = 0;
        for (auto& [name, inst] : this->_basedie->topdie_insts()) {
            model->setItem(row, 0, new QStandardItem {QString::fromStdString(name.data())});
            auto tob = inst->tob();
            if (tob == nullptr) {
                model->setItem(row, 1, new QStandardItem {"No Placed"});
            } else {
                model->setItem(row, 1, new QStandardItem {
                    QString::fromStdString(std::format("{}", inst->tob()->coord()))
                });
            }
            row += 1;
        }

        auto originModel = this->_instPlaceView->model();
        if (originModel != nullptr) {
            delete originModel;
        }

        this->_instPlaceView->setModel(model);

        // Length
        this->_pathLengthEdit->setText(QString{"%1"}.arg(this->_scene->totalNetLenght()));
    }

}