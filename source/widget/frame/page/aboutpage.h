#ifndef __EDDO_ABOUTPAGE_HH__
#define __EDDO_ABOUTPAGE_HH__

#include "../base/titlepagewidget.h"

class QWidget;
class QLabel;
class QBoxLayout;

namespace kiwi::widget {

    class LineEdit;

    class AboutPage : public TitlePageWidget {

        Q_OBJECT

    //! \group constrcutor and desctructor
    public:
        AboutPage(QWidget* parent = 0);
        ~AboutPage();

    private:
        // create a line label
        LineEdit* createLine(const QString& title, const QString& context);
        // inser a line break
        void insertLineBreak();

    //! \group override the pure virtual function for PageWidget
    private:
        virtual QString pageIcon() const override
        { return ":/image/image/icon/about.png"; }
        virtual QString pageText() const override
        { return "About"; }
    };

}

#endif // __EDDO_ABOUTPAGE_HH__
