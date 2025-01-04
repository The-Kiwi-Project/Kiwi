#include "./tpdinfowidget.h"
#include "../item/topdieinstitem.h"
#include <circuit/topdieinst/topdieinst.hh>
#include "qtableview.h"
#include "qwidget.h"

#include <debug/debug.hh>

#include <QLabel>
#include <QComboBox>
#include <QSpinBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QTableView>
#include <QGridLayout>
#include <QStandardItemModel>
#include <QHeaderView>

namespace kiwi::widget::schematic {

    constexpr int MIN_HEIGHT = 30;

    TopdieInstInfoWidget::TopdieInstInfoWidget(QWidget* parent) :
        QGroupBox{"TopDieInst Infomation", parent}
    {
        this->setStyleSheet("background-color: white;");
        auto layout = new QGridLayout{this};
        layout->setSpacing(10);

        // Name
        layout->addWidget(new QLabel {"Begin ", this}, 0, 0);
        this->_nameLabel = new QLabel {"", this};
        this->_nameLabel->setStyleSheet(
            "border-radius: 5px;"        // 圆角半径
            "padding: 5px;"             // 内边距
            "border: 1px solid #A9A9A9;" // 边框
        );
        layout->addWidget(this->_nameLabel, 0, 1);

        // IO Map
        auto label = new QLabel {"Pin Map ", this};
        label->setMinimumHeight(MIN_HEIGHT);
        layout->addWidget(label, 1, 0, 1, 2);
        this->_pinMapView = new QTableView {this};
        this->_pinMapView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        layout->addWidget(this->_pinMapView, 2, 0, 1, 2);

        layout->setColumnMinimumWidth(0, 50);
        layout->setColumnStretch(0, 0);
    }

    void TopdieInstInfoWidget::loadInst(TopDieInstanceItem* inst) {
        this->_inst = inst;
        if (this->_inst == nullptr) {
            debug::exception("Load a empty net into TopdieInstInfoWidget");
        }

        this->_nameLabel->setText(this->_inst->name());

        // Create model
        const auto& pinmap = this->_inst->topdieInst()->topdie()->pins_map();
        auto pinMapModel = new QStandardItemModel {static_cast<int>(pinmap.size()), 2};
        
        // Init header
        pinMapModel->setHorizontalHeaderLabels(QStringList{"Pin Name", "Bump Index"}); 
        // Add items
        auto itemRoot = pinMapModel->invisibleRootItem();
        int row = 0;
        for (auto& [pinName, bumpIndex] : pinmap) {
            auto nameItem = new QStandardItem {pinName.c_str()};
            auto indexItem = new QStandardItem {QString{"%1"}.arg(bumpIndex)};
            pinMapModel->setItem(row, 0, nameItem);
            pinMapModel->setItem(row, 1, indexItem);
            row += 1;
        }
        
        delete this->_pinMapView->model();
        this->_pinMapView->setModel(pinMapModel);
    }

}