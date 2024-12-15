#include "./welcomepage.h"

#include <QLayoutItem>
#include <QIcon>
#include <QDate>
#include <QLocale>
#include <QWidget>
#include <QLabel>
#include <QBoxLayout>

namespace kiwi::widget
{

    WelcomePage::WelcomePage(QWidget* parent) :
        TitlePageWidget("WELCOME", parent)
    {
        this->setMouseTracking(true);
        this->buildTitle();
    }

    WelcomePage::~WelcomePage() {

    }

}
