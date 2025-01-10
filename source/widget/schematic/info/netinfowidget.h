#pragma once

#include "qcolor.h"
#include "qobjectdefs.h"
#include <QWidget>

class QLabel;
class QSpinBox;

namespace kiwi::widget {
    class ColorPickerButton;
}

namespace kiwi::widget::schematic {

    class NetItem;

    class NetInfoWidget : public QWidget {
        Q_OBJECT
        
    public:
        NetInfoWidget(QWidget* parent = nullptr);

    signals:
        void netSyncChanged(NetItem* net, int sync);
        void netColorChanged(NetItem* net, const QColor& color);
        void netWidthChanged(NetItem* net, qreal width);
        void removeNet(NetItem* net);

    public:
        void loadNet(NetItem* net);
        auto currentNet() -> NetItem*;

    protected:
        NetItem* _net {nullptr};

        QLabel* _beginPinLabel;
        QLabel* _endPinLabel;
        QSpinBox* _syncSpinBox;
        ColorPickerButton* _colorButton;
        
    };

}