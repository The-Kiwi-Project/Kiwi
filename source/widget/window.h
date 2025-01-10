#pragma once

#include <QWidget>

namespace kiwi::hardware {
    class Interposer;
}

namespace kiwi::circuit {
    class BaseDie;
}

namespace kiwi::widget {

    class Window : public QWidget 
    {
        Q_OBJECT

    public:
        Window(hardware::Interposer* i, circuit::BaseDie* b, QWidget *parent = nullptr);
        ~Window();

    private:
        QWidget* _widget {nullptr};
    };

}

