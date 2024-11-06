#include "./lineedit.h"
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>

namespace kiwi::widget {

    LineEdit::LineEdit(QWidget* parent) :
        QWidget(parent)
    {
        // Generate colors
        this->generateColor(this->_defaultColorScheme);

        // Initialize ui
        this->initializeUI();

        // Connect line edit events
        connect(this->_editor, &QLineEdit::returnPressed, this, &LineEdit::endEdit);
        connect(this->_editor, &QLineEdit::editingFinished, this, &LineEdit::endEdit);
        connect(this->_editor, &QLineEdit::textChanged, this, &LineEdit::onTextChanged);
    }

    LineEdit::~LineEdit()
    {
    }

    void LineEdit::initializeUI()
    {
        // Construct and set main layout
        this->_mainLayout = new QHBoxLayout(this);
        this->_mainLayout->setContentsMargins(12, 0, 12, 0);
        this->_mainLayout->setSpacing(8);
        this->setLayout(this->_mainLayout);

        // Construct editor container widget
        this->_editorWidget = new QWidget(this);
        this->_mainLayout->addWidget(this->_editorWidget);
        this->_editorWidget->show();

        // Construct editor layout to stretch editor widget
        this->_editorWidgetLayout = new QHBoxLayout(this->_editorWidget);
        this->_editorWidgetLayout->setContentsMargins(0, 10, 0, 10);
        this->_editorWidgetLayout->setSpacing(0);
        this->_editorWidget->setLayout(this->_editorWidgetLayout);

        // Install event filter to editor widget to resize indicator
        this->_editorWidget->installEventFilter(this);

        // Construct real text editor
        this->_editor = new QLineEdit(this->_editorWidget);
        this->_editor->setText("");
        this->_editor->setFont(this->_defaultFont);
        this->_editor->setAlignment(Qt::AlignRight);
        this->_editor->setReadOnly(true);
        this->_editor->setAttribute(Qt::WA_TransparentForMouseEvents, true);
        this->_editor->setStyleSheet("QLineEdit{color:#2c2c2c;background-color:#00ffffff;border-style:none;}");
        //this->_editor->setFixedHeight(this->_editor->fontMetrics().lineSpacing());  // Restrict one line
        this->_editor->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);
        this->_editorWidgetLayout->addWidget(this->_editor);
        this->_editor->show();

        // Construct indicator widget
        this->_indicator = new QWidget(this->_editorWidget);
        this->_indicator->setObjectName("indicator");
        this->_indicator->setStyleSheet("QWidget#indicator{background-color:" + this->_indicatorColor.name(QColor::HexArgb) + ";"
            "border-radius:" + QString::number((float)this->_indicatorWidth / 2) + "px;}");
        this->_indicator->resize(this->_indicatorWidth, this->_indicatorWidth);
        this->_indicator->move(this->_editorWidget->width() - this->_indicatorWidth, this->_editorWidget->height() - this->_indicatorWidth - this->_indicatorSpacing);
        this->_indicatorEffect = new QGraphicsOpacityEffect(this->_indicator);
        this->_indicatorEffect->setOpacity(0);
        this->_indicator->setGraphicsEffect(this->_indicatorEffect);
        this->_indicator->show();

        // Construct background widget
        this->_backgroundWidget = new QWidget(this);
        this->_backgroundWidget->resize(size());
        this->_backgroundWidget->setObjectName("backgroundWidget");
        this->_backgroundWidget->setStyleSheet("QWidget#backgroundWidget{background-color:" + this->_backgroundColor.name(QColor::HexArgb) + ";"
            "border-radius:" + QString::number(this->_cornerRadius) + "px;}");
        this->_backgroundWidget->lower();
        this->_backgroundWidget->show();

        // Set size policy
        this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    }

    void LineEdit::generateColor(QColor colorScheme)
    {
        this->_backgroundColor = colorScheme.lighter(120);
        this->_backgroundColor.setAlpha(5);
        this->_hoverColor = colorScheme.lighter(120);
        this->_hoverColor.setAlpha(40);
        this->_pressedColor = colorScheme.lighter(120);
        this->_pressedColor.setAlpha(50);
        this->_indicatorColor = colorScheme;
    }

    void LineEdit::startEdit()
    {
        if (this->_editing)
            return;

        // Set editing flag
        this->_editing = true;

        // Enable qLineEdit widget
        this->_editor->setReadOnly(false);
        this->_editor->setAttribute(Qt::WA_TransparentForMouseEvents, false);
        this->_editor->setFocus();
        this->_editor->setCursorPosition(this->_editor->text().length());

        // Minorly move cursor to update cursor icon
        QCursor::setPos(QCursor::pos() + QPoint(1, 0));
        QCursor::setPos(QCursor::pos() + QPoint(-1, 0));

        // Add grow and fade in animation for indicator
        QParallelAnimationGroup* startEditAnimation = new QParallelAnimationGroup(this);
        QPropertyAnimation* growAnimation = new QPropertyAnimation(this->_indicator, "geometry");
        QPropertyAnimation* fadeInAnimation = new QPropertyAnimation(this->_indicatorEffect, "opacity");
        growAnimation->setDuration(200);
        growAnimation->setEasingCurve(QEasingCurve::OutQuad);
        fadeInAnimation->setDuration(200);
        fadeInAnimation->setEasingCurve(QEasingCurve::OutQuad);
        growAnimation->setStartValue(this->_indicator->geometry());
        growAnimation->setEndValue(QRect(
            0,
            this->_editorWidget->height() - this->_indicatorWidth - this->_indicatorSpacing,
            this->_editorWidget->width(),
            this->_indicatorWidth
        ));
        fadeInAnimation->setStartValue(this->_indicatorEffect->opacity());
        fadeInAnimation->setEndValue(0.999);
        startEditAnimation->addAnimation(growAnimation);
        startEditAnimation->addAnimation(fadeInAnimation);
        startEditAnimation->start(QAbstractAnimation::DeleteWhenStopped);

        // Emit signal
        emit this->onStartEditing(this->_editor->text());
    }

    void LineEdit::endEdit()
    {
        if (!this->_editing)
            return;

        // Set editing flag
        this->_editing = false;

        // Disable qLineEdit widget
        this->_editor->setReadOnly(true);
        this->_editor->setAttribute(Qt::WA_TransparentForMouseEvents, true);
        this->_editor->setSelection(0, 0);

        // Add shrink and fade out animation for indicator
        QParallelAnimationGroup* endEditAnimation = new QParallelAnimationGroup(this);
        QPropertyAnimation* shrinkAnimation = new QPropertyAnimation(this->_indicator, "geometry");
        QPropertyAnimation* fadeOutAnimation = new QPropertyAnimation(this->_indicatorEffect, "opacity");
        shrinkAnimation->setDuration(200);
        shrinkAnimation->setEasingCurve(QEasingCurve::OutQuad);
        fadeOutAnimation->setDuration(200);
        fadeOutAnimation->setEasingCurve(QEasingCurve::OutQuad);
        shrinkAnimation->setStartValue(this->_indicator->geometry());
        shrinkAnimation->setEndValue(QRect(
            this->_editorWidget->width() - this->_indicatorWidth,
            this->_editorWidget->height() - this->_indicatorWidth - this->_indicatorSpacing,
            this->_indicatorWidth,
            this->_indicatorWidth
        ));
        fadeOutAnimation->setStartValue(this->_indicatorEffect->opacity());
        fadeOutAnimation->setEndValue(0);
        endEditAnimation->addAnimation(shrinkAnimation);
        endEditAnimation->addAnimation(fadeOutAnimation);
        endEditAnimation->start(QAbstractAnimation::DeleteWhenStopped);

        // Emit signal
        emit this->onEndEditing(this->_editor->text());
    }

    void LineEdit::showEvent(QShowEvent* event)
    {
        // Call on parent
        QWidget::showEvent(event);

        // Check initialize state
        if (this->_initialized)
            return;

        // Initialize size dependent widgets
        this->_backgroundWidget->resize(size());
        if (this->_editing) {
            this->_indicator->move(0, this->_editorWidget->height() - this->_indicatorWidth - this->_indicatorSpacing);
            this->_indicator->resize(this->_editorWidget->width(), this->_indicatorWidth);
        }
        else {
            this->_indicator->move(this->_editorWidget->width() - this->_indicatorWidth, this->_editorWidget->height() - this->_indicatorWidth - this->_indicatorSpacing);
            this->_indicator->resize(this->_indicatorWidth, this->_indicatorWidth);
        }

        // Set initialized flag
        this->_initialized = true;
    }

    void LineEdit::enterEvent(QEvent* event)
    {
        Q_UNUSED(event);

        // Check on enabled
        if (this->_enabled)
            setCursor(Qt::PointingHandCursor);

        // Change background color
        this->_backgroundWidget->setStyleSheet("QWidget#backgroundWidget{background-color:" + this->_hoverColor.name(QColor::HexArgb) + ";"
            "border-radius:" + QString::number(this->_cornerRadius) + "px;}");

        // Set hover flag
        this->_hovered = true;
    }

    void LineEdit::leaveEvent(QEvent* event)
    {
        Q_UNUSED(event);

        this->setCursor(Qt::ArrowCursor);

        // Change background color
        this->_backgroundWidget->setStyleSheet("QWidget#backgroundWidget{background-color:" + this->_backgroundColor.name(QColor::HexArgb) + ";"
            "border-radius:" + QString::number(this->_cornerRadius) + "px;}");

        // Set hover flag
        this->_hovered = false;
        this->_pressed = false;
    }

    void LineEdit::mousePressEvent(QMouseEvent* event)
    {
        Q_UNUSED(event);

        // Change background color
        this->_backgroundWidget->setStyleSheet("QWidget#backgroundWidget{background-color:" + this->_pressedColor.name(QColor::HexArgb) + ";"
            "border-radius:" + QString::number(this->_cornerRadius) + "px;}");

        // Set pressed flag
        this->_pressed = true;
    }

    void LineEdit::mouseReleaseEvent(QMouseEvent* event)
    {
        Q_UNUSED(event);

        // Change background color
        this->_backgroundWidget->setStyleSheet("QWidget#backgroundWidget{background-color:" + this->_hoverColor.name(QColor::HexArgb) + ";"
            "border-radius:" + QString::number(this->_cornerRadius) + "px;}");

        // Trigger on click
        if (this->_pressed)
        {
            if (this->_enabled)
                if (this->_editing)
                    endEdit();
                else
                    startEdit();
            else if (this->_editing)
                endEdit();
        }

        // Set pressed flag
        this->_pressed = false;
    }

    void LineEdit::focusInEvent(QFocusEvent* event)
    {
        // Call on parent
        QWidget::focusInEvent(event);

        // Check on enabled
        if (!this->_enabled)
            return;

        // Change background color
        this->_backgroundWidget->setStyleSheet("QWidget#backgroundWidget{background-color:" + this->_hoverColor.name(QColor::HexArgb) + ";"
            "border-radius:" + QString::number(this->_cornerRadius) + "px;}");

        // Start edit
        if (!this->_editing)
            this->startEdit();
    }

    void LineEdit::focusOutEvent(QFocusEvent* event)
    {
        // Call on parent
        QWidget::focusOutEvent(event);

        // Change background color
        this->_backgroundWidget->setStyleSheet("QWidget#backgroundWidget{background-color:" + this->_backgroundColor.name(QColor::HexArgb) + ";"
            "border-radius:" + QString::number(this->_cornerRadius) + "px;}");

        // End edit
        if (this->_editing)
            this->endEdit();
    }

    void LineEdit::resizeEvent(QResizeEvent* event)
    {
        Q_UNUSED(event);

        // Check initialize state
        if (!this->_initialized)
            return;

        // Resize background widget
        this->_backgroundWidget->resize(size());
    }

    bool LineEdit::eventFilter(QObject* object, QEvent* event) {
        // Resize indicator when editor widget size changed
        if (object == this->_editorWidget && event->type() == QEvent::Resize) {
            if (this->_editing) {
                this->_indicator->move(0, this->_editorWidget->height() - this->_indicatorWidth - this->_indicatorSpacing);
                this->_indicator->resize(this->_editorWidget->width(), this->_indicatorWidth);
            }
            else {
                this->_indicator->move(this->_editorWidget->width() - this->_indicatorWidth, this->_editorWidget->height() - this->_indicatorWidth - this->_indicatorSpacing);
                this->_indicator->resize(this->_indicatorWidth, this->_indicatorWidth);
            }
        }

        // Call on parent
        return QWidget::eventFilter(object, event);
    }

    void LineEdit::setText(const QString& text) {
        // Set text
        this->_editor->setText(text);
    }

    void LineEdit::setPlaceholderText(const QString& text) {
        // Set placeholder text
        this->_editor->setPlaceholderText(text);
    }

    void LineEdit::setValidator(const QValidator* validator) {
        // Set validator
        this->_editor->setValidator(validator);
    }

    void LineEdit::setEnabled(bool enabled) {
        // Set enabled
        this->_enabled = enabled;

        // Check for current state
        if (this->_editing && !this->_enabled)
            this->endEdit();
    }

    void LineEdit::setMargins(QMargins margins) {
        // Set margins
        this->_mainLayout->setContentsMargins(margins);
    }

    void LineEdit::setMargins(int left, int top, int right, int bottom) {
        // Set margins
        this->_mainLayout->setContentsMargins(left, top, right, bottom);
    }

    void LineEdit::setBackgroundColor(QColor color) {
        // Set background color
        this->_backgroundColor = color;

        // Check for current state
        if (!this->_hovered && !this->_pressed)
        {
            this->_backgroundWidget->setStyleSheet("QWidget#backgroundWidget{background-color:" + this->_backgroundColor.name(QColor::HexArgb) + ";"
                "border-radius:" + QString::number(this->_cornerRadius) + "px;}");
        }
    }

    void LineEdit::setHoverColor(QColor color) {
        // Set hover color
        this->_hoverColor = color;

        // Check for current state
        if (this->_hovered && !this->_pressed)
        {
            this->_backgroundWidget->setStyleSheet("QWidget#backgroundWidget{background-color:" + this->_hoverColor.name(QColor::HexArgb) + ";"
                "border-radius:" + QString::number(this->_cornerRadius) + "px;}");
        }
    }

    void LineEdit::setPressedColor(QColor color) {
        // Set pressed color
        this->_pressedColor = color;

        // Check for current state
        if (this->_pressed)
        {
            this->_backgroundWidget->setStyleSheet("QWidget#backgroundWidget{background-color:" + this->_pressedColor.name(QColor::HexArgb) + ";"
                "border-radius:" + QString::number(this->_cornerRadius) + "px;}");
        }
    }

    void LineEdit::setIndicatorColor(QColor color) {
        // Set indicator color
        this->_indicatorColor = color;

        this->_indicator->setStyleSheet("QWidget#indicator{background-color:" + this->_indicatorColor.name(QColor::HexArgb) + ";"
            "border-radius:" + QString::number(this->_indicatorWidth) + "px;}");
    }

    void LineEdit::setColorScheme(QColor primaryColor) {
        // Generate colors
        generateColor(primaryColor);

        // Check for current state
        if (!this->_hovered && !this->_pressed)
        {
            this->_backgroundWidget->setStyleSheet("QWidget#backgroundWidget{background-color:" + this->_backgroundColor.name(QColor::HexArgb) + ";"
                "border-radius:" + QString::number(this->_cornerRadius) + "px;}");
        }
        else if (this->_hovered && !this->_pressed)
        {
            this->_backgroundWidget->setStyleSheet("QWidget#backgroundWidget{background-color:" + this->_hoverColor.name(QColor::HexArgb) + ";"
                "border-radius:" + QString::number(this->_cornerRadius) + "px;}");
        }
        else if (this->_pressed)
        {
            this->_backgroundWidget->setStyleSheet("QWidget#backgroundWidget{background-color:" + this->_pressedColor.name(QColor::HexArgb) + ";"
                "border-radius:" + QString::number(this->_cornerRadius) + "px;}");
        }

        this->_indicator->setStyleSheet("QWidget#indicator{background-color:" + this->_indicatorColor.name(QColor::HexArgb) + ";"
            "border-radius:" + QString::number(this->_indicatorWidth) + "px;}");
    }

}
