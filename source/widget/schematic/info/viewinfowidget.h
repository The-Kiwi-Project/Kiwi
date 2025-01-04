#pragma once

#include <QWidget>

class QLabel;
class QSpinBox;

namespace kiwi::widget {
    class ColorPickerButton;
}

namespace kiwi::widget::schematic {

    class ViewInfoWidget : public QWidget {
    public:
        ViewInfoWidget(QWidget* parent = nullptr);
        
    };

}