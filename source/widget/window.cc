#include "window.h"
#include "./page/aboutpage.h"
#include <QDebug>
#include <QResizeEvent>

namespace kiwi::widget {

    Window::Window(QWidget *parent)
        : FramelessWindow(15, parent)
    {
        // Deafult all font as black(In Windows, some fonts become white...)
        this->setStyleSheet("QLabel { color : black; }");
        
        // Create main layout for page and sidebar
        this->_mainLayout = new QHBoxLayout(this->_windowWidget);
        this->_mainLayout->setContentsMargins(0, 0, 0, 0);
        this->_mainLayout->setSpacing(0);
        this->_windowWidget->setLayout(this->_mainLayout);

        this->_placeHolderWidget = new QWidget(this->_windowWidget);
        this->_placeHolderWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        this->_placeHolderWidget->setMouseTracking(true);

        // Connect the resize event of the placeholder widget to the resizePages function using event filter
        this->_placeHolderWidget->installEventFilter(this);

        this->_sideBar = new SideBar(this->_windowWidget);
        connect(this->_sideBar, &SideBar::onPageChanged, this, [=](PageWidget* previousPage, PageWidget* currentPage) {
            // Check for input validity
            if (previousPage == currentPage)
                return;
            if (previousPage != nullptr)
                previousPage->offStage();
            if (currentPage != nullptr)
                currentPage->onStage();
        });
        this->_sideBar->setMouseTracking(true);

        this->_mainLayout->addWidget(this->_sideBar);
        this->_mainLayout->addWidget(this->_placeHolderWidget);

        // Add page into sidebar
        this->_welcomePage = new WelcomePage(this->_placeHolderWidget);
        this->_welcomePage->setMouseTracking(true);
        this->_sideBar->addPage(this->_welcomePage);

        this->_aboutPage = new AboutPage(this->_placeHolderWidget);
        this->_aboutPage->setMouseTracking(true);
        this->_sideBar->addPage(this->_aboutPage);

        this->setMinimumSize(QSize(800, 600));
        this->resize(900, 650);
    }

    Window::~Window() {
    }

    void Window::resizePages(QResizeEvent *event) {
        // Check for input validity
        if (event == nullptr)
            return;

        // Get the size of the placeholder widget
        QSize size = event->size();

        // Resize the editor page
        this->_welcomePage->resize(size);
        this->_aboutPage->resize(size);
    }

    void Window::showEvent(QShowEvent *event) {
        // Call parent show event
        FramelessWindow::showEvent(event);

        // Resize all the pages based on the placeholder widget
        this->_welcomePage->resize(this->_placeHolderWidget->size());
        this->_aboutPage->resize(this->_placeHolderWidget->size());
    }

    bool Window::eventFilter(QObject *object, QEvent *event) {
        // Check for input validity
        if (object == nullptr || event == nullptr)
            return false;

        // Check if the object is the placeholder widget and the event is a resize event
        if (object == _placeHolderWidget && event->type() == QEvent::Resize)
            resizePages(static_cast<QResizeEvent*>(event));

        // Call parent event filter
        return FramelessWindow::eventFilter(object, event);
    }

}
