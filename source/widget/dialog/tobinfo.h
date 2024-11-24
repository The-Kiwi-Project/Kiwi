#pragma once

#include "qdialog.h"
#include <QDialog>

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
    };

}