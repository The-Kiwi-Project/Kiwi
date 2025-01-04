#pragma once

#include <QWidget>

class QLabel;
class QSpinBox;

namespace kiwi::widget {
    class ColorPickerButton;
}

namespace kiwi::widget::schematic {

    class TopDieInstanceItem;

    class TopdieInstInfoWidget : public QWidget {
    public:
        TopdieInstInfoWidget(QWidget* parent = nullptr);

    public:
        void loadInst(TopDieInstanceItem* inst);

    protected:
        TopDieInstanceItem* _inst {nullptr};
        
    };

}