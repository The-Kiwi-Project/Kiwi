#pragma once

#include <QWidget>

namespace kiwi::widget::schematic {

    class NetItem;

    class NetInfoWidget : public QWidget {
    public:
        NetInfoWidget(NetItem* net, QWidget* parent = nullptr);

    protected:
        NetItem* _net;
    };

}