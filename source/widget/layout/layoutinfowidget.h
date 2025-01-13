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

    class LayoutScene;

    class LayoutInfoWidget : public QWidget {
    public:
        LayoutInfoWidget(
            hardware::Interposer* interposer, circuit::BaseDie* basedie, 
            LayoutScene* scene,
            QWidget* parent
        );

        void updateInfo();

    private:
        hardware::Interposer* _interposer {nullptr};
        circuit::BaseDie*     _basedie {nullptr};
        LayoutScene* _scene {nullptr};
    
        QSpinBox* _topdieInstSizeSpinBox {nullptr};
        QTableView* _instPlaceView {nullptr};
        QLineEdit* _pathLengthEdit {nullptr};
    };

}