#include "./workpage.h"

#include <QLayoutItem>
#include <QIcon>
#include <QDate>
#include <QLocale>
#include <QWidget>
#include <QLabel>
#include <QBoxLayout>

namespace kiwi::widget
{

    WorkPage::WorkPage(hardware::Interposer* i, circuit::BaseDie* b, QWidget* parent) :
        TitlePageWidget("WORK", parent)
    {
        this->setMouseTracking(true);
        this->buildTitle();

    }

    WorkPage::~WorkPage() {

    }

}
