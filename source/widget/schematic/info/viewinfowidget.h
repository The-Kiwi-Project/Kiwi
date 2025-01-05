#pragma once

#include <QWidget>

class QLabel;
class QSpinBox;

namespace kiwi::widget {
    class ColorPickerButton;
    class SchematicView;
}

namespace kiwi::widget::schematic {

    class ViewInfoWidget : public QWidget {
    public:
        ViewInfoWidget(SchematicView* view, QWidget* parent = nullptr);

    private:
        SchematicView* _view {nullptr};
    };

}