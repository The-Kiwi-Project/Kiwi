#pragma once

#include <QWidget>
#include <QEvent>
#include <QBoxLayout>
#include <QGraphicsEffect>
#include <QPushButton>

namespace kiwi::widget {

class FramelessWindow : public QWidget {
        Q_OBJECT

    //! \group constant
    protected:
        static constexpr int MAX_MOUSE_MOVEMENT = 300;
        static constexpr int BUTTON_RADIUS = 7;
        static constexpr int DEFAULT_CORNER_RADIUS = 0;
        static const QColor DEFAULT_BACKGROUD_COLOR;
        static const QColor DEFAULT_BORDER_COLOR;

    //! \group enum type
    public:
        enum WindowAttribute {
            WINDOW_NO_ATTRIBUTES        = 0,
            WINDOW_DISABLE_CLOSE        = 1 << 0,
            WINDOW_DISABLE_MAXIMIZE     = 1 << 1,
            WINDOW_DISABLE_MINIMIZE     = 1 << 2,
            WINDOW_DISABLE_RESIZE       = 1 << 3,
        };

        enum MouseState {
            MOUSE_STATE_NONE            = 0,
            MOUSE_STATE_RESIZE_LEFT     = 1 << 0,
            MOUSE_STATE_RESIZE_TOP      = 1 << 1,
            MOUSE_STATE_RESIZE_RIGHT    = 1 << 2,
            MOUSE_STATE_RESIZE_BOTTOM   = 1 << 3,
        };

    //! \group constructor and desctructor
    public:
        FramelessWindow(int cornerRadius, const QColor& backgroundColor, const QColor& borderColor, QWidget* parent = nullptr);
        FramelessWindow(int cornerRadius, QWidget* parent = nullptr);
        FramelessWindow(const QColor& backgroundColor, const QColor& borderColor, QWidget* parent = nullptr);
        FramelessWindow(QWidget* parent = nullptr);
        ~FramelessWindow();

    //! \group GUI constrl variables
    protected:
        int _cornerRadius;
        QColor _backgroundColor;
        QColor _borderColor;

    //! \group window initialize
    protected:
        bool _initialized = false;
        void initializeWindowUI();
        virtual void showEvent(QShowEvent* event) override;

    //! \group widget list
    protected:
        QVBoxLayout* _stretchLayout = nullptr;

        QWidget* _windowWidget = nullptr;
        QWidget* _windowBorder = nullptr;
        QGraphicsDropShadowEffect* _windowShadow = nullptr;

        QWidget* _windowBtnWidget = nullptr;
        QHBoxLayout* _windowBtnLayout = nullptr;
        QPushButton* _minimizeBtn = nullptr;
        QPushButton* _maximizeBtn = nullptr;
        QPushButton* _closeBtn = nullptr;

    //! \group window size control
    protected:
        WindowAttribute _attributes = WINDOW_NO_ATTRIBUTES;
        bool _maximized = false;
        QRect _lastWindowGeometry = QRect{};

        virtual void resizeEvent(QResizeEvent* event) override;
        void controlWindowScale();

    //! \group user interaction control
    protected:
        bool _mousePressed = false;
        int _mouseState = MOUSE_STATE_NONE;
        QPoint _lastMousePosition = QPoint{};

        void updateMouseState(QMouseEvent* event);
        virtual void mousePressEvent(QMouseEvent* event) override;
        virtual void mouseReleaseEvent(QMouseEvent* event) override;
        virtual void mouseMoveEvent(QMouseEvent* event) override;

    //! \group public API
    public:
        QWidget* windowWidget() const { return this->_windowWidget; }
        WindowAttribute getWindowAttributes() const { return this->_attributes; }
        void setWindowAttributes(WindowAttribute attributes);
    };

}
