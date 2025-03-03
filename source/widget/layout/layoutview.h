#pragma once

#include <widget/frame/graphicsview.h>
#include <QWidget>
#include <QGraphicsView>

namespace kiwi::hardware {
    class Interposer;
    class TOB;
    class COB;
    class Track;
};

namespace kiwi::circuit {
    class BaseDie;
    class TopDieInstance;
};

namespace kiwi::widget {
   
    class LayoutScene;

    class LayoutView : public GraphicsView {
    public:
        explicit LayoutView(QWidget *parent = nullptr);

        ~LayoutView() noexcept;
    };

}