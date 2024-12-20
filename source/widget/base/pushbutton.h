#pragma once

#include <QWidget>
#include <QEvent>
#include <QBoxLayout>
#include <QGraphicsEffect>
#include <QPropertyAnimation>

namespace kiwi::widget {

    enum LUI_BTN_INDICATOR_POS {
        LUI_BTN_POS_LEFT,
        LUI_BTN_POS_RIGHT,
        LUI_BTN_POS_TOP,
        LUI_BTN_POS_BOTTOM
    };

    class PushButton : public QWidget {

        Q_OBJECT

    public:
        PushButton(QWidget* child, QWidget* parent, LUI_BTN_INDICATOR_POS indicatorPos = LUI_BTN_POS_LEFT, const QColor& colorScheme = QColor(58, 143, 183));
        ~PushButton();

    private:
        // Button Layout
        QWidget* _childWidget;
        QRect _childWidgetOriginalGeometry;
        const float _childWidgetShrinkRatio = 0.9;
        QHBoxLayout* _stretchLayout;
        QMargins _contentMargin = QMargins(12, 12, 12, 12);

        // Button ui
        int _radius = 8;

        QWidget* _backgroundWidget;
        QColor _backgroundColor;
        QColor _hoverColor;
        QColor _pressedColor;
        QColor _selectedColor;
        QColor _restoredColor[5];

        QWidget* _indicator;
        LUI_BTN_INDICATOR_POS _indicatorPosition;
        const int _indicatorWidth = 6;
        const int _indicatorSpacing = 4;
        const float _activatedLengthRatio = 0.4;
        const float _hoveredLengthRatio = 0.5;
        const float _pressedLengthRatio = 0.2;
        QColor _indicatorColor;
        QGraphicsOpacityEffect* _indicatorEffect;

        // Button state
        bool _enabled = true;
        bool _hovered = false;
        bool _pressed = false;
        bool _selected = false;

    private:
        // UI util functions
        void initializeUI();
        void generateColor(const QColor& colorScheme);

    private:
        // Interactions
    #if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        virtual void enterEvent(QEnterEvent* event) override;
    #else
        virtual void enterEvent(QEvent* event) override;
    #endif
        virtual void leaveEvent(QEvent* event) override;
        virtual void mousePressEvent(QMouseEvent* event) override;
        virtual void mouseReleaseEvent(QMouseEvent* event) override;
        virtual void resizeEvent(QResizeEvent* event) override;

    public:
        // Operation APIs
        void select();
        void deselect();
        void setEnabled(bool enabled = true);

        // Attribute setter APIs
        void setRadius(int radius);

        void setBackgroundColor(QColor color);
        void setHoverColor(QColor color);
        void setPressedColor(QColor color);
        void setSelectedColor(QColor color);
        void setIndicatorColor(QColor color);
        void setColorScheme(QColor primaryColor);

        QHBoxLayout* mainLayout() const;
        void setAlignment(Qt::Alignment alignment);
        void setMargin(QMargins margin);
        void setMargin(int left, int top, int right, int bottom);

        void setIndicatorPosition(LUI_BTN_INDICATOR_POS position);

        QWidget* childWidget();
        void setChildWidget(QWidget* widget);

        bool isSelected() const;

    signals:
        void onClick();
        void onHover();
        void onPressed();
    };

}

