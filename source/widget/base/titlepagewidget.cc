#include "./titlepagewidget.h"
#include "qglobal.h"

namespace kiwi::widget {

    TitlePageWidget::TitlePageWidget(QString title, QWidget* parent) :
        _title{qMove(title)},
        PageWidget(parent)
    {
    }

    void TitlePageWidget::buildTitle() {
        // Construct title layout
        this->_titleLayout = new QVBoxLayout(this);
        this->_titleLayout->setContentsMargins(28, 28, 28, 18);
        this->_titleLayout->setSpacing(18);
        this->_titleLayout->setAlignment(Qt::AlignTop | Qt::AlignLeft);
        this->setLayout(this->_titleLayout);

        // Construct title
        this->_titleText = new QLabel(this->_title, this);
        this->_titleText->setFont(this->_titleFont);
        this->_titleLayout->addWidget(this->_titleText);
        this->_titleText->show();

        // Construct main layout
        this->_mainWidget = new QWidget(this);
        this->_mainWidget->setObjectName("mainWidget");
        this->_mainWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        this->_mainWidget->setStyleSheet("QWidget#mainWidget { background-color: #efefef; border-radius: 8px; }");
        this->_mainLayout = new QVBoxLayout(this->_mainWidget);
        this->_mainLayout->setAlignment(Qt::AlignTop);
        this->_mainLayout->setContentsMargins(0, 8, 0, 8);
        this->_mainLayout->setSpacing(8);
        this->_mainWidget->setLayout(this->_mainLayout);
        this->_titleLayout->addWidget(this->_mainWidget);
        this->_mainWidget->show();
    }

}