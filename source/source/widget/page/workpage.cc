#include "./workpage.h"
#include "../render/renderwidget.h"
#include "qlabel.h"

#include <QLayoutItem>
#include <QIcon>
#include <QDate>
#include <QLocale>
#include <QWidget>
#include <QLabel>
#include <QBoxLayout>

namespace kiwi::widget
{

    WorkPage::WorkPage(QWidget* parent) :
        TitlePageWidget("WORK", parent)
    {
        this->setMouseTracking(true);
        this->buildTitle();

        this->_renderWidget = new RenderWidget(this->_mainWidget);
        this->_renderWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        // this->_renderWidget->setMinimumSize(10, 10);
        this->_mainLayout->addWidget(this->_renderWidget);
        this->_renderWidget->show();
    }

    WorkPage::~WorkPage() {

    }

}
