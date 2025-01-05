#include "./exportwidget.h"

#include "../item/exportitem.h"

#include <debug/debug.hh>

#include <QLabel>
#include <QComboBox>
#include <QSpinBox>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QGridLayout>

namespace kiwi::widget::schematic {

    constexpr int MIN_HEIGHT = 30;

    ExPortItemInfoWidget::ExPortItemInfoWidget(QWidget* parent) :
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

        // Name
        layout->addWidget(new QLabel {"Name ", widget}, 0, 0);
        this->_nameLabel = new QLabel {"", widget};
        this->_nameLabel->setStyleSheet(
            "border-radius: 5px;"
            "padding: 5px;"
            "border: 1px solid #A9A9A9;"
        );
        layout->addWidget(this->_nameLabel, 0, 1);

        // Connected Pin
        layout->addWidget(new QLabel {"Name ", widget}, 1, 0);
        this->_connectedPinLabel = new QLabel {"", widget};
        this->_connectedPinLabel->setStyleSheet(
            "border-radius: 5px;"
            "padding: 5px;"
            "border: 1px solid #A9A9A9;"
        );
        layout->addWidget(this->_connectedPinLabel, 1, 1);
    }

    void ExPortItemInfoWidget::loadExPort(ExPortItem* eport) {
        this->_eport = eport;
        if (this->_eport == nullptr) {
            debug::exception("Load a empty externl port into ExPortInfoWidget");
        }

        this->_nameLabel->setText(eport->pin()->name());
    }
}