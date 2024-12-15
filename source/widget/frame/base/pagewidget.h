#pragma once 

#include "./constant.h"
#include "./pushbutton.h"
#include <QWidget>
#include <QGraphicsEffect>
#include <QLabel>

namespace kiwi::widget {

    class PageWidget : public QWidget
    {
        Q_OBJECT

    //! \group constructor and destructor
    public:
        PageWidget(QWidget* parent = 0);
        ~PageWidget();

    //! \group public API to get the widget's page icon or text
    public:
        PushButton* getPageIconButton(QWidget* context);
        PushButton* getPageTextButton(QWidget* context);

    //! \group pure virtual functions, give the icon path and text
    protected:
        virtual QString pageIcon() const = 0;
        virtual QString pageText() const = 0;

    //! \group stage animation functions
    public:
        void onStage();
        void offStage();

    //! \group data members
    protected:
        int _animationDuration = Default::ANIMATION_DURATION;

        // Button widgets for side bar
        PushButton* _iconButton = nullptr;
        PushButton* _textButton = nullptr;

        // Push button icons
        QLabel* _iconButtonLabel = nullptr;
        QWidget* _textButtonWidget = nullptr;
        QHBoxLayout* _textButtonLayout = nullptr;
        QLabel* _textButtonIcon = nullptr;
        QLabel* _textButtonLabel = nullptr;

        // Opacity effects
        QGraphicsOpacityEffect* _pageOpacityEffect = nullptr;

        // Page position memory
        QPoint _originPagePosition = QPoint(0, 0);

    public:
        int& rAnimationDuration()
        { return this->_animationDuration; }
    };

}
