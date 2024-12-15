#pragma once

#include "./pagewidget.h"
#include "qchar.h"

namespace kiwi::widget {

    class TitlePageWidget : public PageWidget {
    public:
        TitlePageWidget(QString title, QWidget* parent = 0);

    protected:
        void buildTitle();

    protected:
        const QFont _titleFont = QFont("DengXian", 26, QFont::ExtraLight);

        QVBoxLayout* _titleLayout = nullptr;
        QLabel* _titleText = nullptr;

        QWidget* _mainWidget = nullptr;
        QVBoxLayout* _mainLayout = nullptr;

        QString _title;
    };

}