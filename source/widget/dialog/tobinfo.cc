#include "./tobinfo.h"
#include "qboxlayout.h"
#include "qgroupbox.h"
#include <hardware/tob/tob.hh>

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QSpinBox>
#include <QComboBox>

namespace kiwi::widget {

    TOBInfoDialog::TOBInfoDialog(hardware::TOB* tob) :
        QDialog{},
        _tob{tob}
    {
        this->setWindowTitle("COB Info");
        
        auto layout = new QVBoxLayout {this};
        layout->setContentsMargins(10, 10, 10, 10);
        layout->setAlignment(Qt::AlignTop);
        this->setLayout(layout);

        //// Base info
        auto baseGroup = new QGroupBox {this};
        baseGroup->setTitle("Base Infomation");
        auto baseLayout = new QVBoxLayout {baseGroup};
        baseLayout->setContentsMargins(10, 10, 10, 10);
        layout->addWidget(baseGroup);
        
        // coord
        auto baseLayout1 = new QHBoxLayout {baseGroup};
        auto coordLabel = new QLabel {baseGroup};
        coordLabel->setText("Coord");
        baseLayout1->addWidget(coordLabel);
        auto coordValueLabel = new QLabel {baseGroup};
        coordValueLabel->setText(QString{"(%1, %2)"}.
            arg(this->_tob->coord().row).
            arg(this->_tob->coord().col));
        baseLayout1->addWidget(coordValueLabel);

        baseLayout->addLayout(baseLayout1);

        //// Register Inquiry
        auto inquiryGroup = new QGroupBox {this};
        inquiryGroup->setTitle("Register Inquiry");
        auto inquiryLayout = new QVBoxLayout {inquiryGroup};
        inquiryLayout->setContentsMargins(10, 10, 10, 10);
        layout->addWidget(inquiryGroup);

        // Bump index spinbox
        auto inquiryLayout1 = new QHBoxLayout {inquiryGroup};
        inquiryLayout1->setContentsMargins(0, 0, 0, 0);

        auto bumpIndexLabel = new QLabel {inquiryGroup};
        bumpIndexLabel->setText("Bump Index");
        inquiryLayout1->addWidget(bumpIndexLabel);

        this->_bumpIndexSpinBox = new QSpinBox {inquiryGroup};
        this->_bumpIndexSpinBox->setMaximum(hardware::TOB::INDEX_SIZE - 1);
        this->_bumpIndexSpinBox->setMinimum(0);
        inquiryLayout1->addWidget(this->_bumpIndexSpinBox);

        inquiryLayout->addLayout(inquiryLayout1);

        //// Optional 

        this->setMinimumSize(400, 400);
    }

    TOBInfoDialog::~TOBInfoDialog() {}

}