#include "./netinfowidget.h"
#include "../item/netitem.h"
#include "../item/netpointitem.h"
#include "../item/pinitem.h"

#include <QLabel>
#include <QComboBox>
#include <QSpinBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>

namespace kiwi::widget::schematic {

    NetInfoWidget::NetInfoWidget(NetItem* net, QWidget* parent) : 
        QWidget{parent},
        _net{net}
    {
        this->setStyleSheet("background-color: white;");
        auto layout = new QGridLayout{this};
        layout->setSpacing(10);
        
        auto titleLabel = new QLabel {"Net Infomation", this};
        layout->addWidget(titleLabel, 0, 0, 1, 2);

        // Begin
        auto beginPoint = this->_net->beginPoint();
        auto beginPin = beginPoint->connectedPin();
        layout->addWidget(new QLabel {"Begin ", this}, 1, 0);
        auto beginLabel = new QLabel {beginPin->toString(), this};
        beginLabel->setStyleSheet(
            "border-radius: 5px;"        // 圆角半径
            "padding: 10px;"             // 内边距
            "border: 1px solid #A9A9A9;" // 边框
        );

        layout->addWidget(beginLabel, 1, 1);

        // End
        auto endPoint = this->_net->endPoint();
        auto endPin = endPoint->connectedPin();
        layout->addWidget(new QLabel {"End   ", this}, 2, 0);
        auto endLabel = new QLabel {endPin->toString(), this};
        endLabel->setStyleSheet(
            "border-radius: 5px;"        // 圆角半径
            "padding: 10px;"             // 内边距
            "border: 1px solid #A9A9A9;" // 边框
        );
        layout->addWidget(endLabel, 2, 1);

        // Sync
        layout->addWidget(new QLabel {"Sync  ", this}, 3, 0);
        auto syncSpinBox = new QSpinBox{this};
        syncSpinBox->setMinimum(-1);
        syncSpinBox->setMaximum(32);
        syncSpinBox->setValue(this->_net->sync());
        layout->addWidget(syncSpinBox, 3, 1);

        // Color 

        // Width

        layout->setColumnStretch(0, 1);
        layout->setColumnStretch(1, 2);

        layout->setColumnMinimumWidth(0, 50);  // 第一列最小宽度为 100px
        layout->setColumnStretch(0, 0);         // 第一列不参与伸缩
        
        this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    }

}