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
        void topDieInstRename(TopDieInstanceItem* inst, const QString& name);
        void removeTopDieInst(TopDieInstanceItem* inst);

    public:
        void loadInst(TopDieInstanceItem* inst);

    protected:
        TopDieInstanceItem* _inst {nullptr};

        LineEditWithButton*_nameEdit {nullptr};
        QTableView* _pinMapView {nullptr};
        
    };

}