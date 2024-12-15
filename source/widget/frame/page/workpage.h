#pragma once

#include "../base/titlepagewidget.h"

class QWidget;
class QLabel;
class QHBoxLayout;
class QVBoxLayout;
class QGridLayout;

namespace kiwi::hardware {
    class Interposer;
}

namespace kiwi::circuit {
    class BaseDie;
}

namespace kiwi::widget {

    class PushButton;
    class LineEdit;
    class View3DWidget;
    class View2DWidget;
    
    class WorkPage : public TitlePageWidget {

        Q_OBJECT

    //! \group constrcutor and desctructor
    public:
        WorkPage(hardware::Interposer* i, circuit::BaseDie* b, QWidget* parent = 0);
        ~WorkPage();

    //! \group UI elements
    private:
        View3DWidget* _renderWidget;

    //! \group override the pure virtual function for PageWidget
    private:
        virtual QString pageIcon() const override
        { return ":/image/image/icon/work.png"; }
        virtual QString pageText() const override
        { return "Work"; }
    };

}

