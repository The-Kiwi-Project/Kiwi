#ifndef __GUI_SIDEBAR_HH__
#define __GUI_SIDEBAR_HH__

#include "./constant.h"
#include "./pushbutton.h"
#include "./pagewidget.h"
#include <QWidget>
#include <QLabel>
#include <QBoxLayout>
#include <QColor>
#include <QMap>
#include <QList>

namespace kiwi::widget {

    class SideBar : public QWidget {
        Q_OBJECT

    public:
        SideBar(QWidget* parent = 0);
        ~SideBar();

    private:
        // Initialized flag
        bool _initialized = false;

        // UI control variables
        QColor _backgroundColor = QColor(230, 230, 230);
        int _collapsedWidth = 86;
        int _expandedWidth = 200;
        int _animationDuration = Default::ANIMATION_DURATION;

        // UI widgets
        PushButton* _expandButton;
        QLabel* _expandButtonIcon;
        QVBoxLayout* _mainLayout = nullptr; // Layout expand icon and page selection buttons
        QWidget* _pageIconButtonWidget = nullptr;   // Container for page icon buttons
        QVBoxLayout* _pageIconButtonLayout = nullptr;   // Layout page icon buttons
        QWidget* _pageTextButtonWidget = nullptr;   // Container for page text buttons
        QVBoxLayout* _pageTextButtonLayout = nullptr;   // Layout page text buttons

        // Interaction controls
        bool _expanded = false;
        PageWidget* _currentPage = nullptr;

        // State storage
        QList<PageWidget*> _pageList;
        QList<QPair<PushButton*, PushButton*>> _pageButtonList;

    private:
        void expand();
        void collapse();

    public:
        void selectPage(PageWidget* page);

    public:
        void setBackgroundColor(QColor color);
        void setCollapsedWidth(int width);
        void setExpandedWidth(int width);
        void addPage(PageWidget* page);
        void insertPage(PageWidget* page, int index);
        void removePage(PageWidget* page);
        void setAnimationDuration(int animationDuration)
        { this->_animationDuration = animationDuration; }

    signals:
        void onPageChanged(PageWidget* previousPage, PageWidget* currentPage);
    };

}

#endif // __GUI_SIDEBAR_HH__
