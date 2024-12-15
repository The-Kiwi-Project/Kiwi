#include "./aboutpage.h"
#include "../base/lineedit.h"
#include "qlabel.h"
#include <QLayoutItem>
#include <QIcon>
#include <QDate>
#include <QLocale>

namespace kiwi::widget {

    AboutPage::AboutPage(QWidget* parent) :
        TitlePageWidget("ABOUT", parent)
    {
        this->setMouseTracking(true);
        this->buildTitle();

        // Message
        this->createLine("Authored by","Jiaheng Tang & Chen Ye");
        this->createLine("Version", "v0.1.0");
        const QDate buildDate = QLocale( QLocale::English ).toDate( QString( __DATE__ ).replace( "  ", " 0" ), "MMM dd yyyy");
        this->createLine("Last Update", buildDate.toString("yyyy-MM-dd"));

        this->insertLineBreak();

        this->createLine("Supervisor", "Jun Tao");
        this->createLine("Project Instructor", "Lei Xu & Haitao Yang");

        this->insertLineBreak();

        this->createLine("Opensource Repo", "https://github.com/The-Kiwi-Project/Kiwi.git");
    }

    AboutPage::~AboutPage() {}

    LineEdit* AboutPage::createLine(const QString &title, const QString &context) {
        LineEdit* line = new LineEdit(this->_mainWidget);
        line->mainLayout()->insertWidget(0, new QLabel(title));
        line->mainLayout()->insertSpacing(1, 8);
        line->setBackgroundColor(QColor(0, 0, 0, 0));
        line->setHoverColor(QColor(0, 0, 0, 10));
        line->setPressedColor(QColor(0, 0, 0, 20));
        line->setText(context);
        line->setEnabled(false);
        this->_mainLayout->addWidget(line);
        line->show();
        return line;
    }

    void AboutPage::insertLineBreak() {
        QWidget* line = new QWidget(this->_mainWidget);
        line->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        line->setFixedHeight(1);
        line->setStyleSheet("background-color: #c2c2c2;");
        _mainLayout->addWidget(line);
        line->show();
    }

}
