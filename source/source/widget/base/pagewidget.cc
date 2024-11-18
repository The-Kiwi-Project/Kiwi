#include "pagewidget.h"
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>

namespace kiwi::widget
{

    PageWidget::PageWidget(QWidget* parent) : QWidget(parent) {
        // Add opacity effect to real content
        this->_pageOpacityEffect = new QGraphicsOpacityEffect(this);
        this->_pageOpacityEffect->setOpacity(0);
        this->setGraphicsEffect(this->_pageOpacityEffect);

        // Move offstage
        this->move(this->_originPagePosition + QPoint(0, 150));
        this->hide();
    }

    PageWidget::~PageWidget() {
    }

    PushButton *PageWidget::getPageIconButton(QWidget *context) {
        // Check for existed button
        if (this->_iconButton != nullptr)
            return this->_iconButton;

        // Generate new icon button
        this->_iconButton = new PushButton(nullptr, context);
        this->_iconButton->setMargin(20, 18, 16, 18);
        this->_iconButtonLabel = new QLabel(this->_iconButton);
        this->_iconButtonLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
        QFont iconButtonFont("Font Awesome 6 Free Regular", 12);
        iconButtonFont.setStyleStrategy(QFont::PreferAntialias);
        this->_iconButtonLabel->setFont(iconButtonFont);
        this->_iconButtonLabel->setFixedWidth(20);
        QString style = "border-image:url('" + this->pageIcon() + "');";
        this->_iconButtonLabel->setStyleSheet(style);
        this->_iconButtonLabel->setAlignment(Qt::AlignLeft);
        this->_iconButton->setChildWidget(this->_iconButtonLabel);

        // Return newly generated icon
        return this->_iconButton;
    }

    PushButton *PageWidget::getPageTextButton(QWidget *context) {
        // Check for existed button
        if (this->_textButton != nullptr)
            return this->_textButton;

        // Generate new text button
        this->_textButton = new PushButton(nullptr, context);
        this->_textButton->setMargin(20, 18, 16, 18);
        this->_textButtonWidget = new QWidget(this->_textButton);
        this->_textButtonLayout = new QHBoxLayout(this->_textButtonWidget);
        this->_textButtonLayout->setContentsMargins(0, 0, 0, 0);
        this->_textButtonLayout->setSpacing(12);
        this->_textButtonWidget->setLayout(this->_textButtonLayout);

        // Generate text button contents
        this->_textButtonIcon = new QLabel(this->_textButtonWidget);
        QFont textButtonFont("Font Awesome 6 Free Regular", 12);
        textButtonFont.setStyleStrategy(QFont::PreferQuality);
        this->_textButtonIcon->setFont(textButtonFont);
        this->_textButtonIcon->setFixedWidth(20);
        QString style = "border-image:url('" + this->pageIcon() + "');";
        this->_textButtonIcon->setStyleSheet(style);
        this->_textButtonIcon->setAlignment(Qt::AlignLeft);

        this->_textButtonLabel = new QLabel(this->_textButtonWidget);
        this->_textButtonLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
        this->_textButtonLabel->setText(pageText());
        this->_textButtonLabel->setAlignment(Qt::AlignLeft);

        // Add text button contents to layout
        this->_textButtonLayout->addWidget(this->_textButtonIcon);
        this->_textButtonLayout->addWidget(this->_textButtonLabel);
        this->_textButtonIcon->show();
        this->_textButtonLabel->show();

        // Set text button child widget
        this->_textButton->setChildWidget(this->_textButtonWidget);
        this->_textButtonWidget->show();

        // Return newly generated text button
        return this->_textButton;
    }

    void PageWidget::onStage() {
        // Move up and fade in
        QParallelAnimationGroup* onStageAnimation = new QParallelAnimationGroup(this);
        QPropertyAnimation* moveAnimation = new QPropertyAnimation(this, "pos");
        QPropertyAnimation* fadeInAnimation = new QPropertyAnimation(this->_pageOpacityEffect, "opacity");
        moveAnimation->setDuration(this->_animationDuration);
        moveAnimation->setEasingCurve(QEasingCurve::OutExpo);
        moveAnimation->setStartValue(pos());
        moveAnimation->setEndValue(this->_originPagePosition);
        fadeInAnimation->setDuration(this->_animationDuration);
        fadeInAnimation->setEasingCurve(QEasingCurve::InQuad);
        fadeInAnimation->setStartValue(this->_pageOpacityEffect->opacity());
        fadeInAnimation->setEndValue(0.999);
        onStageAnimation->addAnimation(moveAnimation);
        onStageAnimation->addAnimation(fadeInAnimation);
        onStageAnimation->start(QAbstractAnimation::DeleteWhenStopped);

        // Show page
        this->show();
    }

    void PageWidget::offStage() {
        // Move down and fade out
        QParallelAnimationGroup* offStageAnimation = new QParallelAnimationGroup(this);
        //QPropertyAnimation* moveAnimation = new QPropertyAnimation(this->_contentWidget, "pos");
        QPropertyAnimation* fadeOutAnimation = new QPropertyAnimation(this->_pageOpacityEffect, "opacity");
        fadeOutAnimation->setDuration(200);
        fadeOutAnimation->setStartValue(this->_pageOpacityEffect->opacity());
        fadeOutAnimation->setEndValue(0);
        //offStageAnimation->addAnimation(moveAnimation);
        offStageAnimation->addAnimation(fadeOutAnimation);
        offStageAnimation->start(QAbstractAnimation::DeleteWhenStopped);

        // Connect animation finished signal to hide page
        connect(offStageAnimation, &QParallelAnimationGroup::finished, [this]() {
            this->move(this->_originPagePosition + QPoint(0, 150));
            this->hide();
        });
    }

}
