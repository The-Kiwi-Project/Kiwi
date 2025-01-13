#pragma once

#include <QMessageBox>

namespace kiwi::widget {

    #define QMESSAGEBOX_REPORT_EXCEPTION(title)\
    catch (const std::Exception& err) {\
        QMessageBox::critical(\
            this,\
            title,\
            QString::fromLatin1(err.what())\
        );\
    }
    
}