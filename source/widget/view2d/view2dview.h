#pragma once

#include <widget/frame/graphicsview.h>

namespace kiwi::widget {
   
    class View2DView : public GraphicsView {

    public:
        explicit View2DView(QWidget *parent = nullptr);

        ~View2DView() noexcept;
    };

}