#include "./exportwidget.h"

#include "../item/exportitem.h"
#include "hardware/cob/cob.hh"
#include "hardware/interposer.hh"
#include <circuit/export/export.hh>
#include "hardware/track/trackcoord.hh"
#include "qchar.h"
#include "qcombobox.h"
#include "qlabel.h"
#include "qobjectdefs.h"
#include "qpushbutton.h"
#include "qspinbox.h"
#include "widget/frame/lineeditwithbutton.h"

#include <cassert>
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
        this->_nameEdit = new LineEditWithButton {widget};
        this->_nameEdit->setMinimumHeight(MIN_HEIGHT);
        layout->addWidget(this->_nameEdit, 0, 1);

        connect(this->_nameEdit, &LineEditWithButton::textConfirmed, [this] (const QString& name) {
            assert(this->_eport == nullptr);
            emit this->exportRename(this->_eport, name);
        });

        // Coord
        layout->addWidget(new QLabel {"Row  ", widget}, 1, 0);
        this->_rowSpinBox = new QSpinBox {widget};
        this->_rowSpinBox->setMinimumHeight(MIN_HEIGHT);
        this->_rowSpinBox->setMinimum(0);
        this->_rowSpinBox->setMaximum(hardware::Interposer::COB_ARRAY_HEIGHT - 1);
        layout->addWidget(this->_rowSpinBox, 1, 1);

        layout->addWidget(new QLabel {"Column", widget}, 2, 0);
        this->_colSpinBox = new QSpinBox {widget};
        this->_colSpinBox->setMinimumHeight(MIN_HEIGHT);
        this->_colSpinBox->setMinimum(0);
        this->_colSpinBox->setMaximum(hardware::Interposer::COB_ARRAY_WIDTH - 1);
        layout->addWidget(this->_colSpinBox, 2, 1);

        layout->addWidget(new QLabel {"Dir  ", widget}, 3, 0);
        this->_dirComboBox = new QComboBox {widget};
        this->_dirComboBox->setMinimumHeight(MIN_HEIGHT);
        this->_dirComboBox->addItem("Hori");
        this->_dirComboBox->addItem("Vert");
        layout->addWidget(this->_dirComboBox, 3, 1);

        layout->addWidget(new QLabel {"Index "}, 4, 0);
        this->_indexSpinBox = new QSpinBox {widget};
        this->_indexSpinBox->setMinimumHeight(MIN_HEIGHT);
        this->_indexSpinBox->setMinimum(0);
        this->_indexSpinBox->setMaximum(hardware::COB::INDEX_SIZE);
        layout->addWidget(this->_indexSpinBox, 4, 1);

        this->_setCoordButton = new QPushButton {"Set Coord", widget};
        this->_setCoordButton->setMinimumHeight(MIN_HEIGHT);
        layout->addWidget(this->_setCoordButton, 5, 0, 1, 2);

        connect(this->_setCoordButton, &QPushButton::clicked, [this] () {
            auto coord = hardware::TrackCoord {
                this->_rowSpinBox->value(),
                this->_colSpinBox->value(),
                this->_dirComboBox->currentIndex() == 0 ? hardware::TrackDirection::Horizontal : hardware::TrackDirection::Vertical,
                static_cast<std::usize>(this->_indexSpinBox->value())
            };
            emit this->exportSetCoord(this->_eport, coord);
        });
    }

    void ExPortItemInfoWidget::loadExPort(ExPortItem* eport) {
        this->_eport = eport;
        if (this->_eport == nullptr) {
            debug::exception("Load a empty externl port into ExPortInfoWidget");
        }

        this->_nameEdit->setText(eport->pin()->name());

        this->_rowSpinBox->setValue(eport->exPort()->coord().row);
        this->_colSpinBox->setValue(eport->exPort()->coord().col);
        this->_dirComboBox->setCurrentIndex(
            eport->exPort()->coord().dir == hardware::TrackDirection::Horizontal ? 0 : 1
        );
        this->_indexSpinBox->setValue(eport->exPort()->coord().index);
    }
}