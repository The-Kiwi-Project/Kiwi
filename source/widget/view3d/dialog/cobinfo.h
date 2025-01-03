#pragma once

#include "hardware/cob/cobregister.hh"
#include "std/utility.hh"
#include <QDialog>

namespace kiwi::hardware {
    class COB;
}

class QButton;
class QComboBox;
class QSpinBox;

namespace kiwi::widget {

    class COBInfoDialog : public QDialog {
    public:
        COBInfoDialog(hardware::COB* cob);
        ~COBInfoDialog();

    private:
        auto currentRegister() -> std::Tuple<hardware::COBSwRegister*, hardware::COBSelRegister*>;
        auto setRegister() -> void;
        auto updateRegister() -> void;
        
    private:
        hardware::COB* _cob;

        QComboBox* _fromDirection {nullptr};
        QSpinBox*  _fromTrackIndex {nullptr};
        QComboBox* _toDirection {nullptr};

        QSpinBox*  _toIndex {nullptr};
        QComboBox* _swRegister {nullptr};
        QComboBox* _selRegister {nullptr};

        QPushButton* _editorButton {nullptr};
        QPushButton* _setButton {nullptr};

    };

}