#pragma once

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

    public:
        void loadNet(NetItem* net);

    signals:
        void syncChanged(NetItem* net, int sync);

    private:
        void colorChanged(const QColor& color);
        void widthChanged(qreal width);

    protected:
        NetItem* _net {nullptr};

        QLabel* _beginPinLabel;
        QLabel* _endPinLabel;
        QSpinBox* _syncSpinBox;
        ColorPickerButton* _colorButton;
        
    };

}