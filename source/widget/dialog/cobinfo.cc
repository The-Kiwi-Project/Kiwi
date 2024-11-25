#include "./cobinfo.h"
#include "qboxlayout.h"
#include "qchar.h"
#include "qdialog.h"
#include "qglobal.h"
#include "qgroupbox.h"
#include "qlabel.h"
#include "qlineedit.h"
#include "qnamespace.h"
#include "qtextformat.h"
#include <hardware/cob/cob.hh>

#include <QComboBox>
#include <QSpinBox>
#include <QGroupBox>
#include <QDebug>

namespace kiwi::widget {

    COBInfoDialog::COBInfoDialog(hardware::COB* cob) :
        QDialog{},
        _cob{cob}
    {
        this->setWindowTitle("COB Info");
        
        auto layout = new QVBoxLayout {this};
        layout->setContentsMargins(10, 10, 10, 10);
        layout->setAlignment(Qt::AlignTop);
        this->setLayout(layout);
    
        // Show Coord 
        auto sublayout1 = new QHBoxLayout {this};
        sublayout1->setContentsMargins(0, 0, 0, 0);

        auto label = new QLabel {this};
        label->setText("Coord");
        sublayout1->addWidget(label);
        
        label = new QLabel {this};
        label->setText(QString("(%1, %2)")
            .arg(cob->coord().row)
            .arg(cob->coord().col)
        );
        label->setAlignment(Qt::AlignRight);
        label->setStyleSheet(
            "QLabel {""border: 1px solid black;"
                "border-radius: 3px;"
            "}");
        sublayout1->addWidget(label);

        layout->addLayout(sublayout1);

        ////////////////////////////////////////////////////////////////////////////////
        auto registerGroup = new QGroupBox {this};
        registerGroup->setTitle("Register Inquiry");
        layout->addWidget(registerGroup);
        auto registerLayout = new QVBoxLayout {registerGroup};
        registerLayout->setContentsMargins(10, 10, 10, 10);
        
        // From dir
        auto sublayout2 = new QHBoxLayout {registerGroup};
        sublayout2->setContentsMargins(0, 0, 0, 0);

        auto fromdirLabel = new QLabel {registerGroup};
        fromdirLabel->setText("From Direction");
        sublayout2->addWidget(fromdirLabel);

        auto fromdirComboBox = new QComboBox {registerGroup};
        fromdirComboBox->addItem("Left");
        fromdirComboBox->addItem("Right");
        fromdirComboBox->addItem("Up");
        fromdirComboBox->addItem("Down");
        sublayout2->addWidget(fromdirComboBox);

        registerLayout->addLayout(sublayout2);

        // From index
        auto sublayout4 = new QHBoxLayout {registerGroup};
        sublayout4->setContentsMargins(0, 0, 0, 0);

        auto fromIndex = new QLabel {registerGroup};
        fromIndex->setText("From index");
        sublayout4->addWidget(fromIndex);

        auto fromidxSpinbox = new QSpinBox {registerGroup};
        fromidxSpinbox->setMaximum(hardware::COB::INDEX_SIZE - 1);
        fromidxSpinbox->setMinimum(0);
        sublayout4->addWidget(fromidxSpinbox);

        registerLayout->addLayout(sublayout4);

        // To dir
        auto sublayout3 = new QHBoxLayout {registerGroup};
        sublayout3->setContentsMargins(0, 0, 0, 0);

        auto todirLabel = new QLabel {registerGroup};
        todirLabel->setText("To Direction");
        sublayout3->addWidget(todirLabel);

        auto todirComboBox = new QComboBox {registerGroup};
        todirComboBox->addItem("Left");
        todirComboBox->addItem("Right");
        todirComboBox->addItem("Up");
        todirComboBox->addItem("Down");
        sublayout3->addWidget(todirComboBox);

        registerLayout->addLayout(sublayout3);

        this->setMinimumSize(600, 400);
    }

    COBInfoDialog::~COBInfoDialog() {}

}