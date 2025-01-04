#pragma once

#include <QGroupBox>

class QLabel;
class QSpinBox;

namespace kiwi::widget {
    class ColorPickerButton;
    class SchematicView;
}

namespace kiwi::widget::schematic {

    class ViewInfoWidget : public QGroupBox {
    public:
        ViewInfoWidget(SchematicView* view, QWidget* parent = nullptr);

    private:
        SchematicView* _view {nullptr};
    };

}