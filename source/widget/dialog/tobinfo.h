#pragma once

#include "qdialog.h"
#include <QDialog>

class QComboBox;
class QSpinBox;

namespace kiwi::hardware {
    class TOB;
}

namespace kiwi::widget {

    class TOBInfoDialog : public QDialog {
    public:
        TOBInfoDialog(hardware::TOB* tob);
        ~TOBInfoDialog();
        
    private:
        hardware::TOB* _tob;

        QSpinBox* _bumpIndexSpinBox;
    };

}