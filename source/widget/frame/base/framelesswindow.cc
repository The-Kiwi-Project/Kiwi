#include "./framelesswindow.h"
#include <QApplication>
#include <QPainterPath>
#include <QScreen>
#include <QLabel>
#include <QMouseEvent>
#include <QDebug>

namespace kiwi::widget {

    const QColor FramelessWindow::DEFAULT_BACKGROUD_COLOR = QColor{251, 251, 251};
    const QColor FramelessWindow::DEFAULT_BORDER_COLOR = QColor{104, 104, 104};

    FramelessWindow::FramelessWindow(int cornerRadius, const QColor &backgroundColor, const QColor &borderColor, QWidget *parent):
        QWidget(parent),
        _cornerRadius{cornerRadius},
        _backgroundColor{backgroundColor},
        _borderColor{borderColor}
    {
        this->setAttribute(Qt::WA_TranslucentBackground);
        this->setWindowFlags(Qt::FramelessWindowHint);
        this->setFocusPolicy(Qt::StrongFocus);
        this->setMouseTracking(true);
        this->setFocus();

        // create and properly set real displayed window widget
        this->_stretchLayout = new QVBoxLayout(this);
        this->_stretchLayout->setContentsMargins(30, 30, 30, 30);
        this->_windowWidget = new QWidget(this);
        this->_windowWidget->setObjectName("windowWidget");
        this->_windowWidget->setMouseTracking(true);
        this->_stretchLayout->addWidget(this->_windowWidget);
        this->_windowWidget->show();
        setLayout(this->_stretchLayout);

        // set style sheet for window widget
        QString windowWidgetStyleSheet = "QWidget#windowWidget{background-color:"
                                        + this->_backgroundColor.name(QColor::HexArgb)
                                        + ";border-radius:"
                                        + QString::number(this->_cornerRadius) + "px;}";
        this->_windowWidget->setStyleSheet(windowWidgetStyleSheet);

        // set shadow for window widget
        this->_windowShadow = new QGraphicsDropShadowEffect(this->_windowWidget);
        this->_windowShadow->setBlurRadius(30);
        this->_windowShadow->setColor(QColor(0, 0, 0));
        this->_windowShadow->setOffset(0, 0);
        this->_windowWidget->setGraphicsEffect(this->_windowShadow);

        // create window control buttons container widget & its layout
        this->_windowBtnWidget = new QWidget(this->_windowWidget);
        this->_windowBtnWidget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        this->_windowBtnWidget->setMouseTracking(true);
        this->_windowBtnLayout = new QHBoxLayout(this->_windowBtnWidget);
        this->_windowBtnLayout->setContentsMargins(0, 0, 0, 0);
        this->_windowBtnLayout->setSpacing(10);
        this->_windowBtnLayout->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        this->_windowBtnWidget->setLayout(this->_windowBtnLayout);

        // create window control buttons
        this->_minimizeBtn = new QPushButton(this->_windowBtnWidget);
        this->_maximizeBtn = new QPushButton(this->_windowBtnWidget);
        this->_closeBtn = new QPushButton(this->_windowBtnWidget);

        this->_minimizeBtn->setFixedSize(2 * BUTTON_RADIUS, 2 * BUTTON_RADIUS);
        this->_maximizeBtn->setFixedSize(2 * BUTTON_RADIUS, 2 * BUTTON_RADIUS);
        this->_closeBtn->setFixedSize(2 * BUTTON_RADIUS, 2 * BUTTON_RADIUS);

        this->_minimizeBtn->setStyleSheet("QPushButton{border-radius: "
                                    + QString::number(BUTTON_RADIUS)
                                    + "px; background-color: #c2c2c2;}QPushButton:hover{background-color: #e98b2a;}");
        this->_maximizeBtn->setStyleSheet("QPushButton{border-radius: "
                                    + QString::number(BUTTON_RADIUS)
                                    + "px; background-color: #c2c2c2;}QPushButton:hover{background-color: #2d6d4b;}");
        this->_closeBtn->setStyleSheet("QPushButton{border-radius: "
                                    + QString::number(BUTTON_RADIUS)
                                    + "px; background-color: #c2c2c2;}QPushButton:hover{background-color: #ab3b3a;}");

        this->_windowBtnLayout->addWidget(this->_minimizeBtn);
        this->_windowBtnLayout->addWidget(this->_maximizeBtn);
        this->_windowBtnLayout->addWidget(this->_closeBtn);

        if ((this->_attributes & WindowAttribute::WINDOW_DISABLE_MINIMIZE) == 0)
            this->_minimizeBtn->show();
        if ((this->_attributes & WindowAttribute::WINDOW_DISABLE_MAXIMIZE) == 0)
            this->_maximizeBtn->show();
        if ((this->_attributes & WindowAttribute::WINDOW_DISABLE_CLOSE) == 0)
            this->_closeBtn->show();

        // connect window control buttons
        connect(this->_minimizeBtn, &QPushButton::clicked, this, &QWidget::showMinimized);
        connect(this->_maximizeBtn, &QPushButton::clicked, this, &FramelessWindow::controlWindowScale);
        connect(this->_closeBtn, &QPushButton::clicked, this, &QWidget::close);
    }

    FramelessWindow::FramelessWindow(int cornerRadius, QWidget *parent):
        FramelessWindow(cornerRadius, DEFAULT_BACKGROUD_COLOR, DEFAULT_BORDER_COLOR, parent)
    {
    }

    FramelessWindow::FramelessWindow(const QColor &backgroundColor, const QColor &borderColor, QWidget *parent):
        FramelessWindow(DEFAULT_CORNER_RADIUS, backgroundColor, borderColor, parent)
    {
    }

    FramelessWindow::FramelessWindow(QWidget *parent):
        FramelessWindow(DEFAULT_CORNER_RADIUS, DEFAULT_BACKGROUD_COLOR, DEFAULT_BORDER_COLOR, parent)
    {
    }

    FramelessWindow::~FramelessWindow()
    {
    }

    void FramelessWindow::initializeWindowUI() {
        if (this->_initialized == true)
            return;

        // create a round cornered mask for window widget
        QPainterPath path;
        path.addRoundedRect(this->_windowWidget->rect(), this->_cornerRadius - 1, this->_cornerRadius - 1);
        QRegion region(path.toFillPolygon().toPolygon());
        this->_windowWidget->setMask(region);

        // create a border for window widget (in order to hide zigzagged edges)
        this->_windowBorder = new QWidget(this);
        this->_windowBorder->setObjectName("windowBorder");
        QString windowBorderStyleSheet =
            "QWidget#windowBorder{background-color:#00FFFFFF;border:1.5px solid "
            + this->_borderColor.name(QColor::HexArgb)
            + ";border-radius:"
            + QString::number(this->_cornerRadius) + "px;}";
        this->_windowBorder->setStyleSheet(windowBorderStyleSheet);
        this->_windowBorder->setAttribute(Qt::WA_TransparentForMouseEvents);
        this->_windowBorder->move(_windowWidget->pos() - QPoint(1, 1));
        this->_windowBorder->resize(_windowWidget->size() + QSize(2, 2));
        this->_windowBorder->show();

        // move button widget to the top right of the window widget
        this->_windowBtnWidget->move(this->_windowWidget->width() - this->_windowBtnWidget->width() - 18, 18);
        this->_windowBtnWidget->show();
        this->_windowBtnWidget->raise();

        // set initialized state
        this->_initialized = true;
    }

    void FramelessWindow::showEvent(QShowEvent *event) {
        Q_UNUSED(event);
        this->initializeWindowUI();
    }

    void FramelessWindow::resizeEvent(QResizeEvent *event) {
        Q_UNUSED(event);
        // Resize window border
        if (this->_windowBorder != nullptr)
        {
            this->_windowBorder->move(this->_windowWidget->pos() - QPoint(1, 1));
            this->_windowBorder->resize(this->_windowWidget->size() + QSize(2, 2));
        }

        // resize window mask
        QPainterPath path;
        path.addRoundedRect(this->_windowWidget->rect(), this->_cornerRadius - 1, this->_cornerRadius - 1);
        QRegion region(path.toFillPolygon().toPolygon());
        this->_windowWidget->setMask(region);

        // move button widget to the top right of the window widget
        this->_windowBtnWidget->move(this->_windowWidget->width() - this->_windowBtnWidget->width() - 18, 18);
    }

    void FramelessWindow::controlWindowScale() {
        if ((this->_attributes & WindowAttribute::WINDOW_DISABLE_MAXIMIZE) != 0)
            return;

        if (this->_maximized == false)
        {
            this->_lastWindowGeometry = frameGeometry();

            this->_windowShadow->setEnabled(false);
            this->_windowBorder->hide();
            QString windowWidgetStyleSheet = "QWidget#windowWidget{background-color:" + _backgroundColor.name(QColor::HexArgb) + ";}";
            this->_windowWidget->setStyleSheet(windowWidgetStyleSheet);

            this->_stretchLayout->setContentsMargins(0, 0, 0, 0);

            showMaximized();

            QPainterPath path;
            path.addRect(this->_windowWidget->rect());
            QRegion mask(path.toFillPolygon().toPolygon());
            this->_windowWidget->setMask(mask);

            this->_maximized = true;
        }
        else {
            this->_stretchLayout->setContentsMargins(30, 30, 30, 30);

            showNormal();

            resize(this->_lastWindowGeometry.size());
            move(this->_lastWindowGeometry.topLeft());

            this->_windowShadow->setEnabled(true);
            this->_windowBorder->show();
            QString windowWidgetStyleSheet =
                    "QWidget#windowWidget{background-color:"
                    + this->_backgroundColor.name(QColor::HexArgb)
                    + ";border-radius:"
                    + QString::number(_cornerRadius) + "px;}";
            this->_windowWidget->setStyleSheet(windowWidgetStyleSheet);

            QPainterPath path;
            path.addRoundedRect(this->_windowWidget->rect(), this->_cornerRadius - 1, this->_cornerRadius - 1);
            QRegion mask(path.toFillPolygon().toPolygon());
            this->_windowWidget->setMask(mask);

            this->_maximized = false;
        }
    }

    void FramelessWindow::updateMouseState(QMouseEvent *event) {
        this->_mouseState = MOUSE_STATE_NONE;
        if ((this->_attributes & WindowAttribute::WINDOW_DISABLE_RESIZE) != 0)
            return;

        if (this->_maximized == true)
        {
            this->setCursor(Qt::ArrowCursor);
            return;
        }

        // set mouse state according to abs distance from window border
        if (abs(event->globalPos().x() - (this->_windowWidget->frameGeometry().left() + frameGeometry().left())) < 5) {
            this->_mouseState |= MOUSE_STATE_RESIZE_LEFT;
        }
        if (abs(event->globalPos().x() - (this->_windowWidget->frameGeometry().right() + frameGeometry().left())) < 5) {
            this->_mouseState |= MOUSE_STATE_RESIZE_RIGHT;
        }
        if (abs(event->globalPos().y() - (this->_windowWidget->frameGeometry().top() + frameGeometry().top())) < 5) {
            this->_mouseState |= MOUSE_STATE_RESIZE_TOP;
        }
        if (abs(event->globalPos().y() - (this->_windowWidget->frameGeometry().bottom() + frameGeometry().top())) < 5) {
            this->_mouseState |= MOUSE_STATE_RESIZE_BOTTOM;
        }

        // set cursor shape according to mouse state
        switch (this->_mouseState) {
            case MOUSE_STATE_RESIZE_LEFT:
            case MOUSE_STATE_RESIZE_RIGHT:
                setCursor(Qt::SizeHorCursor);
                break;
            case MOUSE_STATE_RESIZE_TOP:
            case MOUSE_STATE_RESIZE_BOTTOM:
                setCursor(Qt::SizeVerCursor);
                break;
            case MOUSE_STATE_RESIZE_LEFT | MOUSE_STATE_RESIZE_TOP:
            case MOUSE_STATE_RESIZE_RIGHT | MOUSE_STATE_RESIZE_BOTTOM:
                setCursor(Qt::SizeFDiagCursor);
                break;
            case MOUSE_STATE_RESIZE_LEFT | MOUSE_STATE_RESIZE_BOTTOM:
            case MOUSE_STATE_RESIZE_RIGHT | MOUSE_STATE_RESIZE_TOP:
                setCursor(Qt::SizeBDiagCursor);
                break;
            default:
                setCursor(Qt::ArrowCursor);
                break;
        }
    }

    void FramelessWindow::mousePressEvent(QMouseEvent *event) {
        if (event->button() == Qt::LeftButton) {
            this->_mousePressed = true;
            this->_lastMousePosition = event->globalPos();
        }
    }

    void FramelessWindow::mouseReleaseEvent(QMouseEvent *event) {
        this->_mousePressed = false;
        QScreen* screen = QGuiApplication::screenAt(event->globalPos());

        if (abs(event->globalPos().y() - screen->geometry().top()) < 5)
            this->controlWindowScale();

        this->updateMouseState(event);
    }

    void FramelessWindow::mouseMoveEvent(QMouseEvent *event) {
        if (this->_mousePressed == false)
            return this->updateMouseState(event);

        // resize window according to mouse state
        switch (this->_mouseState) {
            case MOUSE_STATE_RESIZE_LEFT:
                resize(frameGeometry().width() - event->globalPos().x() + frameGeometry().left() + _windowWidget->frameGeometry().left(), frameGeometry().height());
                move(event->globalPos().x() - _windowWidget->frameGeometry().left(), frameGeometry().top());
                break;
            case MOUSE_STATE_RESIZE_RIGHT:
                resize(event->globalPos().x() - frameGeometry().left() + _windowWidget->frameGeometry().left(), frameGeometry().height());
                break;
            case MOUSE_STATE_RESIZE_TOP:
                resize(frameGeometry().width(), frameGeometry().height() - event->globalPos().y() + frameGeometry().top() + _windowWidget->frameGeometry().top());
                move(frameGeometry().left(), event->globalPos().y() - _windowWidget->frameGeometry().top());
                break;
            case MOUSE_STATE_RESIZE_BOTTOM:
                resize(frameGeometry().width(), event->globalPos().y() - frameGeometry().top() + _windowWidget->frameGeometry().top());
                break;
            case MOUSE_STATE_RESIZE_LEFT | MOUSE_STATE_RESIZE_TOP:
                resize(frameGeometry().width() - event->globalPos().x() + frameGeometry().left() + _windowWidget->frameGeometry().left(), frameGeometry().height() - event->globalPos().y() + frameGeometry().top() + _windowWidget->frameGeometry().top());
                move(event->globalPos().x() - _windowWidget->frameGeometry().left(), event->globalPos().y() - _windowWidget->frameGeometry().top());
                break;
            case MOUSE_STATE_RESIZE_LEFT | MOUSE_STATE_RESIZE_BOTTOM:
                resize(frameGeometry().width() - event->globalPos().x() + frameGeometry().left() + _windowWidget->frameGeometry().left(), event->globalPos().y() - frameGeometry().top() + _windowWidget->frameGeometry().top());
                move(event->globalPos().x() - _windowWidget->frameGeometry().left(), frameGeometry().top());
                break;
            case MOUSE_STATE_RESIZE_RIGHT | MOUSE_STATE_RESIZE_TOP:
                resize(event->globalPos().x() - frameGeometry().left() + _windowWidget->frameGeometry().left(), frameGeometry().height() - event->globalPos().y() + frameGeometry().top() + _windowWidget->frameGeometry().top());
                move(frameGeometry().left(), event->globalPos().y() - _windowWidget->frameGeometry().top());
                break;
            case MOUSE_STATE_RESIZE_RIGHT | MOUSE_STATE_RESIZE_BOTTOM:
                resize(event->globalPos().x() - frameGeometry().left() + _windowWidget->frameGeometry().left(), event->globalPos().y() - frameGeometry().top() + _windowWidget->frameGeometry().top());
                break;
            default:
                if (this->_maximized == true) {
                    qreal wRatio = (qreal)event->pos().x() / (qreal)_windowWidget->width();
                    controlWindowScale();
                    move(event->globalPos().x() - _windowWidget->width() * wRatio, event->globalPos().y() - 52);
                }
                else {
                    move(frameGeometry().left() + event->globalPos().x() - _lastMousePosition.x(), frameGeometry().top() + event->globalPos().y() - _lastMousePosition.y());
                }
                break;
        }
        this->_lastMousePosition = event->globalPos();
    }

    void FramelessWindow::setWindowAttributes(WindowAttribute attributes) {
        this->_attributes = attributes;
        if ((_attributes & WindowAttribute::WINDOW_DISABLE_MINIMIZE) == 0) {
            this->_minimizeBtn->show();
        } else {
            this->_minimizeBtn->hide();
        }
        
        if ((_attributes & WindowAttribute::WINDOW_DISABLE_MAXIMIZE) == 0) {
            this->_maximizeBtn->show();
        } else {
            this->_maximizeBtn->hide();
        }

        if ((_attributes & WindowAttribute::WINDOW_DISABLE_CLOSE) == 0) {
            this->_closeBtn->show();
        }
        else {
            this->_closeBtn->hide();
        }
    }

}
