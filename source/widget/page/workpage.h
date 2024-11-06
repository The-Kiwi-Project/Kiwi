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
    class RenderWidget;

    class WorkPage : public TitlePageWidget {

        Q_OBJECT

    //! \group constrcutor and desctructor
    public:
        WorkPage(QWidget* parent = 0);
        ~WorkPage();

    //! \group UI elements
    private:
        RenderWidget* _renderWidget;

    //! \group override the pure virtual function for PageWidget
    private:
        virtual QString pageIcon() const override
        { return ":/images/images/labels/work.png"; }
        virtual QString pageText() const override
        { return "Work"; }
    };

}

