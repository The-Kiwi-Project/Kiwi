#include "./cobinfo.h"
#include "hardware/cob/cobdirection.hh"
#include "hardware/cob/cobregister.hh"
#include "qboxlayout.h"
#include "qchar.h"
#include "qcombobox.h"
#include "qdialog.h"
#include "qglobal.h"
#include "qgroupbox.h"
#include "qlabel.h"
#include "qlist.h"
#include "qnamespace.h"
#include "qobject.h"
#include "qpushbutton.h"
#include "qspinbox.h"
#include "qtextformat.h"
#include <hardware/cob/cob.hh>

#include <QComboBox>
#include <QSpinBox>
#include <QGroupBox>
#include <QDebug>
#include <QMessageBox>

// MARK: A Litter MESS(String)

namespace kiwi::widget {

    static auto directions = QStringList {
        "Left", "Right", "Up", "Down"
    };

    COBInfoDialog::COBInfoDialog(hardware::COB* cob) :
        QDialog{},
        _cob{cob}
    {
        this->setWindowTitle("COB Info");
        
        auto layout = new QVBoxLayout {this};
        layout->setContentsMargins(10, 10, 10, 10);
        layout->setAlignment(Qt::AlignTop);
        this->setLayout(layout);
    
        ////////////////////////////////////////////////////////////////////////////////
        auto baseGroup = new QGroupBox {this};
        baseGroup->setTitle("Base Information");
        layout->addWidget(baseGroup);
        auto baseLayout = new QVBoxLayout {baseGroup};
        baseLayout->setContentsMargins(10, 10, 10, 10);
        
        auto sublayout1 = new QHBoxLayout {};
        sublayout1->setContentsMargins(0, 0, 0, 0);

        auto label = new QLabel {baseGroup};
        label->setText("Coord");
        sublayout1->addWidget(label);
        
        label = new QLabel {baseGroup};
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

        baseLayout->addLayout(sublayout1);

        ////////////////////////////////////////////////////////////////////////////////
        auto registerGroup = new QGroupBox {this};
        registerGroup->setTitle("Register Inquiry");
        layout->addWidget(registerGroup);
        auto registerLayout = new QVBoxLayout {registerGroup};
        registerLayout->setContentsMargins(10, 10, 10, 10);
        
        // From dir
        auto sublayout2 = new QHBoxLayout {};
        sublayout2->setContentsMargins(0, 0, 0, 0);

        auto fromdirLabel = new QLabel {registerGroup};
        fromdirLabel->setText("From Direction");
        sublayout2->addWidget(fromdirLabel);

        this->_fromDirection = new QComboBox {registerGroup};
        for (const auto& d : directions) {
            this->_fromDirection->addItem(d);
        }
        sublayout2->addWidget(this->_fromDirection);

        registerLayout->addLayout(sublayout2);

        // From index
        auto sublayout4 = new QHBoxLayout {};
        sublayout4->setContentsMargins(0, 0, 0, 0);

        auto fromTrackIndex = new QLabel {registerGroup};
        fromTrackIndex->setText("From Track Index");
        sublayout4->addWidget(fromTrackIndex);

        this->_fromTrackIndex = new QSpinBox {registerGroup};
        this->_fromTrackIndex->setMaximum(hardware::COB::INDEX_SIZE - 1);
        this->_fromTrackIndex->setMinimum(0);
        sublayout4->addWidget(this->_fromTrackIndex);

        registerLayout->addLayout(sublayout4);

        // To dir
        auto sublayout3 = new QHBoxLayout {};
        sublayout3->setContentsMargins(0, 0, 0, 0);

        auto todirLabel = new QLabel {registerGroup};
        todirLabel->setText("To Direction");
        sublayout3->addWidget(todirLabel);

        this->_toDirection = new QComboBox {registerGroup};
        auto currentText = this->_fromDirection->currentText();
        for (const auto& s : directions) {
            if (currentText != s) {
                this->_toDirection->addItem(s);
            }
        }
        sublayout3->addWidget(this->_toDirection);

        registerLayout->addLayout(sublayout3);

        // To Index
        auto sublayout5 = new QHBoxLayout {};
        sublayout5->setContentsMargins(0, 0, 0, 0);

        auto toindexLabel = new QLabel {registerGroup};
        toindexLabel->setText("To Index");
        sublayout5->addWidget(toindexLabel);

        this->_toIndex = new QSpinBox {registerGroup};
        this->_toIndex->setMaximum(hardware::COB::INDEX_SIZE - 1);
        this->_toIndex->setMinimum(0);
        this->_toIndex->setEnabled(false);
        sublayout5->addWidget(this->_toIndex);
    
        registerLayout->addLayout(sublayout5);
        
        ////////////////////////////////////////////////////////////////////////////////
        auto resultGroup = new QGroupBox {this};
        resultGroup->setTitle("Register Value");
        layout->addWidget(resultGroup);
        auto resultLayout = new QVBoxLayout {resultGroup};
        resultLayout->setContentsMargins(10, 10, 10, 10); 

        // SW Register
        auto sublayout6 = new QHBoxLayout {};
        sublayout6->setContentsMargins(0, 0, 0, 0);

        auto swregLabel = new QLabel {registerGroup};
        swregLabel->setText("Swith Register");
        sublayout6->addWidget(swregLabel);

        this->_swRegister = new QComboBox {registerGroup};
        this->_swRegister->addItem("Connected");
        this->_swRegister->addItem("Disconnected");
        this->_swRegister->setEditable(false);
        this->_swRegister->setEnabled(false);
        sublayout6->addWidget(this->_swRegister);

        resultLayout->addLayout(sublayout6);

        // Sel Register
        auto sublayout7 = new QHBoxLayout {};
        sublayout7->setContentsMargins(0, 0, 0, 0);

        auto selregLabel = new QLabel {registerGroup};
        selregLabel->setText("Sel Register");
        sublayout7->addWidget(selregLabel);

        this->_selRegister = new QComboBox {registerGroup};
        this->_selRegister->addItem("COB To Track");
        this->_selRegister->addItem("Track to COB");
        this->_selRegister->setEditable(false);
        this->_selRegister->setEnabled(false);
        sublayout7->addWidget(this->_selRegister);

        resultLayout->addLayout(sublayout7);

        ///////////////////////////////////////////////////////////////////////

        auto buttonLayout = new QHBoxLayout {};
        layout->addLayout(buttonLayout);

        this->_editorButton = new QPushButton {this};
        this->_editorButton->setText("Enable Edit");
        buttonLayout->addWidget(this->_editorButton);
    
        this->_setButton = new QPushButton {this};
        this->_setButton->setText("Set Value");
        this->_setButton->setEnabled(false);
        buttonLayout->addWidget(this->_setButton);

        this->setMinimumSize(400, 400);

        /////////////////////////////////////////////////////////////////////////

        QObject::connect(this->_fromDirection, QOverload<int>::of(&QComboBox::currentIndexChanged), [this] (int _) {
            auto currentText = this->_fromDirection->currentText();
            this->_toDirection->clear();
            for (const auto& s : directions) {
                if (currentText != s) {
                    this->_toDirection->addItem(s);
                }
            }
            
            this->updateRegister();
        });

        this->_fromDirection->setCurrentIndex(0);

        QObject::connect(this->_fromTrackIndex, QOverload<int>::of(&QSpinBox::valueChanged), [this] (int _) {
            this->updateRegister();
        });

        QObject::connect(this->_toDirection, QOverload<int>::of(&QComboBox::currentIndexChanged), [this] (int _) {
            this->updateRegister();
        });

        QObject::connect(this->_editorButton, &QPushButton::clicked, [this] () {
            if (this->_setButton->isEnabled()) {
                this->_setButton->setEnabled(false);
                this->_selRegister->setEnabled(false);
                this->_swRegister->setEnabled(false);
            } else {
                this->_setButton->setEnabled(true);
                this->_selRegister->setEnabled(true);
                this->_swRegister->setEnabled(true);
            }
        });

        QObject::connect(this->_setButton, &QPushButton::clicked, [this] () {
            auto reply = QMessageBox::question(
                this, 
                "Set the register value", 
                "Do you want to continue?", 
                QMessageBox::Yes | QMessageBox::No
            );

            if (reply == QMessageBox::Yes) {
                this->setRegister();
            } 
        });

        this->updateRegister();
    }

    // MARK: with method a dir
    static auto directionFromString(const QString& dir) -> hardware::COBDirection {
        if (dir == "Left") {
            return hardware::COBDirection::Left;
        } else if (dir == "Right") {
            return hardware::COBDirection::Right;
        } else if (dir == "Up") {
            return hardware::COBDirection::Up;    
        } 
        return hardware::COBDirection::Down;
    }

    auto COBInfoDialog::updateRegister() -> void {
        auto fromDirection = directionFromString(this->_fromDirection->currentText());
        auto fromTrackIndex = this->_fromTrackIndex->value();
        auto toDirection = directionFromString(this->_toDirection->currentText());

        auto toTrackIndex = hardware::COB::track_index_map(fromDirection, fromTrackIndex, toDirection);
        auto fromCOBIndex = hardware::COB::track_index_to_cob_index(fromTrackIndex);

        auto selreg = this->_cob->sel_register(fromDirection, fromCOBIndex);
        auto swreg = this->_cob->sw_register(fromDirection, fromCOBIndex, toDirection);

        using hardware::COBSignalDirection;
        switch (selreg->get()) {
            case COBSignalDirection::COBToTrack: {
                this->_selRegister->setCurrentText("COB To Track");
                break;
            }
            case COBSignalDirection::TrackToCOB: {
                this->_selRegister->setCurrentText("Track to COB");
                break;
            }
        }

        using hardware::COBSwState;
        switch (swreg->get()) {
            case COBSwState::Connected: {
                this->_swRegister->setCurrentText("Connected");
                break;
            }
            case COBSwState::DisConnected: {
                this->_swRegister->setCurrentText("Disconnected");
                break;
            }
        }

        this->_toIndex->setValue(toTrackIndex);
    }

    auto COBInfoDialog::currentRegister() -> std::Tuple<hardware::COBSwRegister*, hardware::COBSelRegister*> {
        auto fromDirection = directionFromString(this->_fromDirection->currentText());
        auto fromTrackIndex = this->_fromTrackIndex->value();
        auto toDirection = directionFromString(this->_toDirection->currentText());
        auto fromCOBIndex = hardware::COB::track_index_to_cob_index(fromTrackIndex);

        auto selreg = this->_cob->sel_register(fromDirection, fromCOBIndex);
        auto swreg = this->_cob->sw_register(fromDirection, fromCOBIndex, toDirection);

        return {swreg, selreg};
    }

    auto COBInfoDialog::setRegister() -> void {
        auto selregStr = this->_selRegister->currentText();
        auto swregStr = this->_swRegister->currentText();

        using enum hardware::COBSignalDirection;
        auto dir = (selregStr == "Track To COB") ? TrackToCOB : COBToTrack;

        using enum hardware::COBSwState;
        auto state = (swregStr == "Connected") ? Connected : DisConnected;
    
        auto [swreg, selreg] = this->currentRegister();
        swreg->set(state);
        selreg->set(dir);
    }

    COBInfoDialog::~COBInfoDialog() {}

}