#pragma once

#include <QWidget>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QGraphicsOpacityEffect>
#include <QEvent>

namespace kiwi::widget {

    class LineEdit : public QWidget {

        Q_OBJECT

    //! \group constructor & desctructor
    public:
        LineEdit(QWidget* parent = 0);
        ~LineEdit();

    //! \group // UI util functions
    private:
        void initializeUI();
        void generateColor(QColor colorScheme);

    //! \group interaction Util functions
    private:
        void startEdit();
        void endEdit();

    //! \group widget events
    private:
        virtual void showEvent(QShowEvent* event) override;
    #if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        virtual void enterEvent(QEnterEvent* event) override;
    #else
        virtual void enterEvent(QEvent* event) override;
    #endif
        virtual void leaveEvent(QEvent* event) override;
        virtual void mousePressEvent(QMouseEvent* event) override;
        virtual void mouseReleaseEvent(QMouseEvent* event) override;
        virtual void focusInEvent(QFocusEvent* event) override;
        virtual void focusOutEvent(QFocusEvent* event) override;
        virtual void resizeEvent(QResizeEvent* event) override;
        virtual bool eventFilter(QObject* obj, QEvent* event) override;

    //! \group public setter APIs
    public:
        void setText(const QString& text);
        void setPlaceholderText(const QString& text);
        void setValidator(const QValidator* validator);

        void setEnabled(bool enabled = true);
        void setMargins(QMargins margins);
        void setMargins(int left, int top, int right, int bottom);
        void setBackgroundColor(QColor color);
        void setHoverColor(QColor color);
        void setPressedColor(QColor color);
        void setIndicatorColor(QColor color);
        void setColorScheme(QColor primaryColor);

    //! \group public getter APIs
    public:
        QString text() const { return _editor->text(); }
        QHBoxLayout* mainLayout() const { return _mainLayout; }

    //! \group public signal functions
    signals:
        void onStartEditing(const QString& text);
        void onTextChanged(const QString& text);
        void onEndEditing(const QString& text);

    //! \group data members
    private:
        // UI control variables
        int _cornerRadius = 8;

        // UI elements
        QHBoxLayout* _mainLayout = nullptr;
        QWidget* _backgroundWidget = nullptr;
        const QColor _defaultColorScheme = QColor(58, 143, 183);
        QColor _backgroundColor;
        QColor _hoverColor;
        QColor _pressedColor;

        QWidget* _editorWidget = nullptr;
        QHBoxLayout* _editorWidgetLayout = nullptr;

        QLineEdit* _editor = nullptr;
        const QFont _defaultFont = QFont("DengXian", 10, QFont::Normal);

        QWidget* _indicator = nullptr;
        const int _indicatorWidth = 4;
        const int _indicatorSpacing = 4;
        QColor _indicatorColor;
        QGraphicsOpacityEffect* _indicatorEffect;

        // Editor state
        bool _hovered = false;
        bool _pressed = false;
        bool _enabled = true;
        bool _editing = false;
        bool _initialized = false;
    };

}
