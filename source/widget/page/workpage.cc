#include "./workpage.h"
#include "../render/renderwidget.h"

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

        this->_renderWidget = new RenderWidget(i, b, this->_mainWidget);
        this->_renderWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        this->_mainLayout->addWidget(this->_renderWidget);
    }

    WorkPage::~WorkPage() {

    }

}
