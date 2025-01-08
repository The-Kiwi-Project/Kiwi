#include "./tpdinfowidget.h"
#include "../item/topdieinstitem.h"
#include <cassert>
#include <circuit/topdieinst/topdieinst.hh>
#include "qchar.h"
#include "widget/frame/lineeditwithbutton.h"

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
#include <QGroupBox>

namespace kiwi::widget::schematic {

    constexpr int MIN_HEIGHT = 30;

    TopDieInstanceInfoWidget::TopDieInstanceInfoWidget(QWidget* parent) :
        QWidget{parent}
    {
        auto thisLayout = new QVBoxLayout {this};
        auto widget = new QGroupBox {"TopDieInst Infomation", this};
        widget->setStyleSheet("background-color: white;");
        thisLayout->setMargin(0);
        thisLayout->addWidget(widget);
        thisLayout->addStretch();

        auto layout = new QGridLayout{widget};
        layout->setSpacing(10);

        // Name
        layout->addWidget(new QLabel {"Begin ", widget}, 0, 0);
        this->_nameEdit = new LineEditWithButton {widget};
        this->_nameEdit->setMinimumHeight(MIN_HEIGHT);
        layout->addWidget(this->_nameEdit, 0, 1);

        // IO Map
        auto label = new QLabel {"Pin Map ", widget};
        label->setMinimumHeight(MIN_HEIGHT);
        layout->addWidget(label, 1, 0, 1, 2);
        this->_pinMapView = new QTableView {widget};
        this->_pinMapView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        layout->addWidget(this->_pinMapView, 2, 0, 1, 2);

        layout->setColumnMinimumWidth(0, 50);
        layout->setColumnStretch(0, 0);

        connect(this->_nameEdit, &LineEditWithButton::textConfirmed, [this] (const QString& name) {
            assert(this->_inst != nullptr);
            this->topDieInstRename(this->_inst, name);
        });
    }

    void TopDieInstanceInfoWidget::loadInst(TopDieInstanceItem* inst) {
        this->_inst = inst;
        if (this->_inst == nullptr) {
            debug::exception("Load a empty net into TopDieInstanceInfoWidget");
        }

        this->_nameEdit->setText(this->_inst->name());

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