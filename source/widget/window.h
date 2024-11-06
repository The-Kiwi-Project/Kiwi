#pragma once

#include <QMainWindow>
#include "./base/framelesswindow.h"
#include "./base/sidebar.h"
#include "./page/welcomepage.h"
#include "./page/aboutpage.h"

namespace kiwi::widget {

    class Window : public FramelessWindow {
        Q_OBJECT

    public:
        Window(QWidget *parent = nullptr);
        ~Window();

    private:
        void resizePages(QResizeEvent* event);

    private:
        virtual void showEvent(QShowEvent* event) override;
        virtual bool eventFilter(QObject* object, QEvent* event) override;

    private:
        QHBoxLayout* _mainLayout = nullptr;
        QWidget* _placeHolderWidget = nullptr;
        SideBar* _sideBar = nullptr;

        WelcomePage* _welcomePage = nullptr;
        AboutPage* _aboutPage = nullptr;
    };

}

