#pragma once

#include "../base/titlepagewidget.h"

class QWidget;
class QLabel;
class QHBoxLayout;
class QVBoxLayout;
class QGridLayout;

namespace kiwi::widget {

    class PushButton;
    class LineEdit;

    class WelcomePage : public TitlePageWidget {

        Q_OBJECT

    //! \group constrcutor and desctructor
    public:
        WelcomePage(QWidget* parent = 0);
        ~WelcomePage();

    //! \group UI elements
    private:

    //! \group override the pure virtual function for PageWidget
    private:
        virtual QString pageIcon() const override
        { return ":/image/image/icon/welcome.png"; }
        virtual QString pageText() const override
        { return "Welcome"; }
    };

}

