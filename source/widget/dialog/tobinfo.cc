#include "./tobinfo.h"
#include "qdialog.h"

namespace kiwi::widget {

    TOBInfoDialog::TOBInfoDialog(hardware::TOB* tob) :
        QDialog{},
        _tob{tob}
    {
    }

    TOBInfoDialog::~TOBInfoDialog() {}

}