#include "./netinfowidget.h"
#include "../item/netitem.h"
#include "../item/netpointitem.h"
#include "../item/pinitem.h"
#include "qboxlayout.h"
#include "qgroupbox.h"
#include "qspinbox.h"
#include "qwidget.h"
#include <widget/frame/colorpickbutton.h>
#include <circuit/connection/connection.hh>
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
        auto thisLayout = new QVBoxLayout {this};
        auto widget = new QGroupBox {"Net Infomation", this};
        widget->setStyleSheet("background-color: white;");
        thisLayout->setMargin(0);
        thisLayout->addWidget(widget);
        thisLayout->addStretch();

        auto layout = new QGridLayout{widget};
        layout->setSpacing(10);
        
        // Begin
        layout->addWidget(new QLabel {"Begin ", widget}, 0, 0);
        this->_beginPinLabel = new QLabel {"", widget};
        this->_beginPinLabel->setStyleSheet(
            "border-radius: 5px;"        // 圆角半径
            "padding: 5px;"             // 内边距
            "border: 1px solid #A9A9A9;" // 边框
        );
        layout->addWidget(this->_beginPinLabel, 0, 1);

        // End
        layout->addWidget(new QLabel {"End   ", widget}, 1, 0);
        this->_endPinLabel = new QLabel {"", widget};
        this->_endPinLabel->setStyleSheet(
            "border-radius: 5px;"        // 圆角半径
            "padding: 5px;"             // 内边距
            "border: 1px solid #A9A9A9;" // 边框
        );
        layout->addWidget(this->_endPinLabel, 1, 1);

        // Sync
        layout->addWidget(new QLabel {"Sync  ", widget}, 2, 0);
        this->_syncSpinBox = new QSpinBox{widget};
        this->_syncSpinBox->setMinimum(-1);
        this->_syncSpinBox->setMaximum(32);
        this->_syncSpinBox->setMinimumHeight(30);
        layout->addWidget(this->_syncSpinBox, 2, 1);

        // Color
        layout->addWidget(new QLabel {"Color ", widget}, 3, 0);
        this->_colorButton = new ColorPickerButton {widget};
        this->_colorButton->setMinimumHeight(30);
        layout->addWidget(this->_colorButton, 3, 1);

        // Width
        layout->setColumnMinimumWidth(0, 50);
        layout->setColumnStretch(0, 0);

        connect(this->_colorButton, &ColorPickerButton::colorChanged, this, &NetInfoWidget::colorChanged);
        connect(this->_syncSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), [this] (int sync) {
            assert(this->_ent != nullptr);
            emit this->syncChanged(this->_net, sync);
        });
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

        this->_syncSpinBox->setValue(this->_net->connection()->sync());

        this->_colorButton->setColor(this->_net->color());
    }
        
    void NetInfoWidget::colorChanged(const QColor& color) {
        if (this->_net == nullptr) {
            debug::exception("Change color for nullptr net");
        }

        this->_net->setColor(color);
        this->_net->update();
    }

}