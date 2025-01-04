#include "./netinfowidget.h"
#include "../item/netitem.h"
#include "../item/netpointitem.h"
#include "../item/pinitem.h"
#include "qspinbox.h"
#include <widget/frame/colorpickbutton.h>
#include <debug/debug.hh>

#include <QLabel>
#include <QComboBox>
#include <QSpinBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>

namespace kiwi::widget::schematic {

    NetInfoWidget::NetInfoWidget(QWidget* parent) : 
        QWidget{parent}
    {
        this->setStyleSheet("background-color: white;");
        auto layout = new QGridLayout{this};
        layout->setSpacing(10);
        
        auto titleLabel = new QLabel {"Net Infomation", this};
        layout->addWidget(titleLabel, 0, 0, 1, 2);

        // Begin
        layout->addWidget(new QLabel {"Begin ", this}, 1, 0);
        this->_beginPinLabel = new QLabel {"", this};
        this->_beginPinLabel->setStyleSheet(
            "border-radius: 5px;"        // 圆角半径
            "padding: 5px;"             // 内边距
            "border: 1px solid #A9A9A9;" // 边框
        );
        layout->addWidget(this->_beginPinLabel, 1, 1);

        // End
        layout->addWidget(new QLabel {"End   ", this}, 2, 0);
        this->_endPinLabel = new QLabel {"", this};
        this->_endPinLabel->setStyleSheet(
            "border-radius: 5px;"        // 圆角半径
            "padding: 5px;"             // 内边距
            "border: 1px solid #A9A9A9;" // 边框
        );
        layout->addWidget(this->_endPinLabel, 2, 1);

        // Sync
        layout->addWidget(new QLabel {"Sync  ", this}, 3, 0);
        this->_syncSpinBox = new QSpinBox{this};
        this->_syncSpinBox->setMinimum(-1);
        this->_syncSpinBox->setMaximum(32);
        this->_syncSpinBox->setMinimumHeight(30);
        layout->addWidget(this->_syncSpinBox, 3, 1);

        // Color
        layout->addWidget(new QLabel {"Color ", this}, 4, 0);
        this->_colorButton = new ColorPickerButton {this};
        this->_colorButton->setMinimumHeight(30);
        layout->addWidget(this->_colorButton, 4, 1);

        // Width


        layout->setColumnMinimumWidth(0, 50);
        layout->setColumnStretch(0, 0);

        connect(this->_colorButton, &ColorPickerButton::colorChanged, this, &NetInfoWidget::colorChanged);
        connect(this->_syncSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &NetInfoWidget::syncChanged);
    }

    void NetInfoWidget::loadNet(NetItem* net) {
        this->_net = net;
        if (this->_net == nullptr) {
            debug::exception("Load a empty net into NetInfoWidget");
        }

        auto beginPoint = this->_net->beginPoint();
        auto beginPin = beginPoint->connectedPin();
        this->_beginPinLabel->setText(beginPin->toString());

        auto endPoint = this->_net->endPoint();
        auto endPin = endPoint->connectedPin();
        this->_endPinLabel->setText(endPin->toString());

        this->_syncSpinBox->setValue(this->_net->sync());

        this->_colorButton->setColor(this->_net->color());
    }

    void NetInfoWidget::syncChanged(int sync) {
        if (this->_net == nullptr) {
            debug::exception("Change sync for nullptr net");
        }

        this->_net->setSync(sync);
        this->_net->update();
    }
        
    void NetInfoWidget::colorChanged(const QColor& color) {
        if (this->_net == nullptr) {
            debug::exception("Change color for nullptr net");
        }

        this->_net->setColor(color);
        this->_net->update();
    }

}