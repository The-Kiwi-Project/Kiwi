#pragma once

#include <QWidget>

class QLabel;
class QSpinBox;

namespace kiwi::widget {
    class ColorPickerButton;
}

namespace kiwi::widget::schematic {

    class NetItem;

    class NetInfoWidget : public QWidget {
    public:
        NetInfoWidget(QWidget* parent = nullptr);

    public:
        void loadNet(NetItem* net);

    private:
        void syncChanged(int sync);
        void colorChanged(const QColor& color);

    protected:
        NetItem* _net {nullptr};

        QLabel* _beginPinLabel;
        QLabel* _endPinLabel;
        QSpinBox* _syncSpinBox;
        ColorPickerButton* _colorButton;
        
    };

}