#pragma once

#include "qdialog.h"
#include <QDialog>

namespace kiwi::hardware {
    class COB;
}

namespace kiwi::widget {

    /*
    
        COB Info:

        - 
    
    */

    class COBInfoDialog : public QDialog {
    public:
        COBInfoDialog(hardware::COB* cob);
        ~COBInfoDialog();
        
    private:
        hardware::COB* _cob;
    };

}