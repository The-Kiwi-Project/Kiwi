#include "./sidebar.h"
#include <QFontDatabase>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
#include <QLayoutItem>
#include <QDebug>

namespace kiwi::widget {

    SideBar::SideBar(QWidget* parent)
        : QWidget(parent)
    {
        this->setAttribute(Qt::WA_StyledBackground, true);
        this->setMaximumWidth(this->_collapsedWidth);

        // Create stylesheet
        this->setObjectName("sideBar");
        QString sideBarStyleSheet = "QWidget#sideBar{background-color:" + this->_backgroundColor.name(QColor::HexArgb) + "; }";
        this->setStyleSheet(sideBarStyleSheet);

        // Create main layout
        this->_mainLayout = new QVBoxLayout(this);
        this->_mainLayout->setSpacing(8);
        this->_mainLayout->setContentsMargins(16, 16, 16, 16);
        this->_mainLayout->setAlignment(Qt::AlignTop | Qt::AlignLeft);
        this->_mainLayout->addSpacing(8); // Add the top spacing between the expand button and the window edge

        // Create expand button
        this->_expandButton = new PushButton(nullptr, this);
        this->_expandButton->setMargin(18, 16, 18, 16);
        this->_expandButton->setIndicatorColor(QColor(255, 255, 255, 0)); // Set indicator to transparent
        this->_expandButton->setHoverColor(QColor(0, 0, 0, 10));
        this->_expandButton->setPressedColor(QColor(0, 0, 0, 20));
        int buttonSize = this->_collapsedWidth - this->_mainLayout->contentsMargins().left() - this->_mainLayout->contentsMargins().right();
        // int buttonSizeMax = this->_expandedWidth - this->_mainLayout->contentsMargins().left() - this->_mainLayout->contentsMargins().right();
        this->_expandButton->setMinimumSize(buttonSize, buttonSize);
        this->_expandButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum);
        this->_expandButtonIcon = new QLabel(this->_expandButton);
        this->_expandButtonIcon->setFont(QFont("Font Awesome 6 Free Solid", 14));
        this->_expandButtonIcon->setStyleSheet("border-image:url(':/images/images/labels/list.png');");
        this->_expandButtonIcon->setFixedSize(QSize(20, 20));
        this->_expandButton->setChildWidget(this->_expandButtonIcon);

        // Connect the expand button event to expand / collapse event
        connect(this->_expandButton, &PushButton::onClick, this, [this]() {
            if (this->_expanded)
                this->collapse();
            else
                this->expand();
        });

        // Add expand button to main layout
        this->_mainLayout->addWidget(this->_expandButton);
        this->_expandButton->show();

        // Create the page icon button container widget
        this->_pageIconButtonWidget = new QWidget(this);
        this->_pageIconButtonWidget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        this->_pageIconButtonLayout = new QVBoxLayout(this->_pageIconButtonWidget);
        this->_pageIconButtonLayout->setSpacing(8);
        this->_pageIconButtonLayout->setContentsMargins(0, 0, 0, 0);
        this->_pageIconButtonLayout->setAlignment(Qt::AlignTop | Qt::AlignLeft);
        this->_pageIconButtonWidget->setLayout(this->_pageIconButtonLayout);

        // Create the page text button container widget
        this->_pageTextButtonWidget = new QWidget(this);
        this->_pageTextButtonWidget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        this->_pageTextButtonLayout = new QVBoxLayout(this->_pageTextButtonWidget);
        this->_pageTextButtonLayout->setSpacing(8);
        this->_pageTextButtonLayout->setContentsMargins(0, 0, 0, 0);
        this->_pageTextButtonLayout->setAlignment(Qt::AlignTop | Qt::AlignLeft);
        this->_pageTextButtonWidget->setLayout(this->_pageTextButtonLayout);

        // Add page icon and text button container widgets to main layout
        this->_mainLayout->addWidget(this->_pageIconButtonWidget);
        this->_mainLayout->addWidget(this->_pageTextButtonWidget);

        // Display the icon buttons
        this->_pageIconButtonWidget->show();
        this->_pageTextButtonWidget->hide();
    }

    SideBar::~SideBar() {
    }

    void SideBar::expand() {
        // Add animation to expand the side bar
        QPropertyAnimation* expandAnimation = new QPropertyAnimation(this, "maximumWidth");
        expandAnimation->setDuration(this->_animationDuration);
        expandAnimation->setEasingCurve(QEasingCurve::OutExpo);
        expandAnimation->setStartValue(width());
        expandAnimation->setEndValue(this->_expandedWidth);
        expandAnimation->start(QAbstractAnimation::DeleteWhenStopped);

        // Show the page text buttons
        this->_pageIconButtonWidget->hide();
        this->_pageTextButtonWidget->show();

        // Set the expand state
        this->_expanded = true;
    }

    void SideBar::collapse() {
        // Add animation to collapse the side bar
        QPropertyAnimation* collapseAnimation = new QPropertyAnimation(this, "maximumWidth");
        collapseAnimation->setDuration(this->_animationDuration);
        collapseAnimation->setEasingCurve(QEasingCurve::OutExpo);
        collapseAnimation->setStartValue(width());
        collapseAnimation->setEndValue(this->_collapsedWidth);
        collapseAnimation->start(QAbstractAnimation::DeleteWhenStopped);

        // Show the page icon buttons
        this->_pageTextButtonWidget->hide();
        this->_pageIconButtonWidget->show();

        // Set the expand state
        this->_expanded = false;
    }

    void SideBar::setBackgroundColor(QColor color) {
        this->_backgroundColor = color;
        QString sideBarStyleSheet = "QWidget#sideBar{background-color:" + this->_backgroundColor.name(QColor::HexArgb) + "; }";
        this->setStyleSheet(sideBarStyleSheet);
    }

    void SideBar::setExpandedWidth(int width) {
        if (width <= 0)
            return;
        this->_expandedWidth = width;
    }

    void SideBar::setCollapsedWidth(int width) {
        if (width <= 0)
            return;
        this->_collapsedWidth = width;
    }

    void SideBar::selectPage(PageWidget* page) {
        // Check for input validity
        if (page == nullptr)
            return;

        // Deselect current page if there is one
        if (this->_currentPage != nullptr)
        {
            // Find the buttons of the current page
            int index = this->_pageList.indexOf(this->_currentPage);
            PushButton* currentPageIconButton = this->_pageButtonList.at(index).first;
            PushButton* currentPageTextButton = this->_pageButtonList.at(index).second;

            // Deselect both the buttons
            currentPageIconButton->deselect();
            currentPageTextButton->deselect();
        }

        // Temporarily store the current page
        PageWidget* previousPage = this->_currentPage;

        // Select the new page
        int index = this->_pageList.indexOf(page);
        PushButton* newPageIconButton = this->_pageButtonList.at(index).first;
        PushButton* newPageTextButton = this->_pageButtonList.at(index).second;
        newPageIconButton->select();
        newPageTextButton->select();
        this->_currentPage = page;

        // Emit the page selected signal
        emit this->onPageChanged(previousPage, page);
    }

    void SideBar::addPage(PageWidget* page) {
        // Call on the page's get button method to get icon button & text button
        PushButton* iconButton = page->getPageIconButton(this->_pageIconButtonWidget);
        PushButton* textButton = page->getPageTextButton(this->_pageTextButtonWidget);

        // Store the page in the page list
        this->_pageList.append(page);

        // Store the corresponding buttons
        this->_pageButtonList.append(QPair<PushButton*, PushButton*>(iconButton, textButton));

        // Add the icon button to the icon button layout
        this->_pageIconButtonLayout->addWidget(iconButton);
        iconButton->show();

        // Add the text button to the text button layout
        this->_pageTextButtonLayout->addWidget(textButton);
        textButton->show();

        // Resize the buttons and set size policy to fixed
        int buttonSize = this->_collapsedWidth - this->_mainLayout->contentsMargins().left() - this->_mainLayout->contentsMargins().right();
        //int buttonSizeMax = this->_expandedWidth - this->_mainLayout->contentsMargins().left() - this->_mainLayout->contentsMargins().right();
        iconButton->setMinimumSize(buttonSize, buttonSize);
        textButton->setMinimumSize(buttonSize, buttonSize);
        iconButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        textButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

        // Connect the button click event with page selection
        connect(iconButton, &PushButton::onClick, this, [this,  page]() {
            this->selectPage(page);
        });
        connect(textButton, &PushButton::onClick, this, [this,  page]() {
            this->selectPage(page);
        });

        // Select the page if there are no pages
        if (this->_currentPage == nullptr)
            this->selectPage(page);
    }

    void SideBar::insertPage(PageWidget* page, int index) {
        // Call on the page's get button method to get icon button & text button
        PushButton* iconButton = page->getPageIconButton(this->_pageIconButtonWidget);
        PushButton* textButton = page->getPageTextButton(this->_pageTextButtonWidget);

        // Store the page
        this->_pageList.insert(index, page);

        // Store the corresponding buttons
        this->_pageButtonList.insert(index, QPair<PushButton*, PushButton*>(iconButton, textButton));

        // Add the icon button to the icon button layout
        this->_pageIconButtonLayout->insertWidget(index, iconButton);
        iconButton->show();

        // Add the text button to the text button layout
        this->_pageTextButtonLayout->insertWidget(index, textButton);
        textButton->show();

        // Resize the buttons and set size policy to fixed
        int buttonSize = this->_collapsedWidth - this->_mainLayout->contentsMargins().left() - this->_mainLayout->contentsMargins().right();
        //int buttonSizeMax = this->_expandedWidth - this->_mainLayout->contentsMargins().left() - this->_mainLayout->contentsMargins().right();
        iconButton->setMinimumSize(buttonSize, buttonSize);
        textButton->setMinimumSize(buttonSize, buttonSize);
        iconButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        textButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

        // Connect the button click event with page selection
        connect(iconButton, &PushButton::onClick, this, [this, page]() {
            this->selectPage(page);
        });
        connect(textButton, &PushButton::onClick, this, [this,  page]() {
            this->selectPage(page);
        });

        // Select the page if there are no pages
        if (this->_currentPage == nullptr)
            this->selectPage(page);
    }

    void SideBar::removePage(PageWidget* page) {
        // Get the index of the page from the page list
        int index = this->_pageList.indexOf(page);

        // Get the corresponding buttons
        PushButton* iconButton = this->_pageButtonList.at(index).first;
        PushButton* textButton = this->_pageButtonList.at(index).second;

        // Remove the page from the list
        this->_pageList.removeAt(index);

        // Remove the icon and text buttons from the list
        this->_pageButtonList.removeAt(index);

        // Remove the icon button from the icon button layout
        this->_pageIconButtonLayout->removeWidget(iconButton);
        iconButton->hide();

        // Remove the text button from the text button layout
        this->_pageTextButtonLayout->removeWidget(textButton);
        textButton->hide();

        // Select the first page if the current page is the page being removed
        if (this->_currentPage == page)
        {
            if (this->_pageList.size() > 0)
                this->selectPage(this->_pageList.first());
            else
                this->_currentPage = nullptr;
        }
    }

}
