#pragma once

#include <QHash>
#include <QWidget>

class QSpinBox;
class QTableView;
class QLineEdit;

namespace kiwi::hardware {
    class Interposer;
};

namespace kiwi::circuit {
    class BaseDie;
};

namespace kiwi::widget {

    class LayoutInfoWidget : public QWidget {
    public:
        LayoutInfoWidget(hardware::Interposer* interposer, circuit::BaseDie* basedie, QWidget* parent);

        void updateInfo();

    private:
        QSpinBox* _topdieInstSizeSpinBox {nullptr};
        QTableView* _instPlaceView {nullptr};

        hardware::Interposer* _interposer {nullptr};
        circuit::BaseDie*     _basedie {nullptr};
        QLineEdit* _predictPathLength {nullptr};
    };

}