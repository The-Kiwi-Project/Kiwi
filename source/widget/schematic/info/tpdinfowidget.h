#pragma once

#include "qchar.h"
#include "qobjectdefs.h"
#include <QWidget>

class QLabel;
class QSpinBox;
class QTableView;

namespace kiwi::widget {
    class ColorPickerButton;
    class LineEditWithButton;
}

namespace kiwi::widget::schematic {

    class TopDieInstanceItem;

    class TopDieInstanceInfoWidget : public QWidget {
        Q_OBJECT

    public:
        TopDieInstanceInfoWidget(QWidget* parent = nullptr);

    signals:
        void topdieInstanceRename(TopDieInstanceItem* inst, const QString& name);
        void removeTopDieInstance(TopDieInstanceItem* inst);

    public:
        void loadTopDieInstance(TopDieInstanceItem* inst);
        auto currentTopDieInstance() -> TopDieInstanceItem*;

    protected:
        TopDieInstanceItem* _topdieInstance {nullptr};

        LineEditWithButton*_nameEdit {nullptr};
        QTableView* _pinMapView {nullptr};
        
    };

}