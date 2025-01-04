#pragma once

#include <QGroupBox>

class QLabel;
class QSpinBox;
class QTableView;

namespace kiwi::widget {
    class ColorPickerButton;
}

namespace kiwi::widget::schematic {

    class TopDieInstanceItem;

    class TopdieInstInfoWidget : public QGroupBox {
    public:
        TopdieInstInfoWidget(QWidget* parent = nullptr);

    public:
        void loadInst(TopDieInstanceItem* inst);

    protected:
        TopDieInstanceItem* _inst {nullptr};

        QLabel* _nameLabel {nullptr};
        QTableView* _pinMapView {nullptr};
        
    };

}