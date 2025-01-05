#pragma once

#include <QWidget>

class QLabel;
class QSpinBox;
class QTableView;

namespace kiwi::widget::schematic {

    class ExPortItem;

    class ExPortItemInfoWidget : public QWidget {
    public:
        ExPortItemInfoWidget(QWidget* parent = nullptr);

    public:
        void loadExPort(ExPortItem* eport);

    protected:
        ExPortItem* _eport {nullptr};

        QLabel* _nameLabel {nullptr};
        QLabel* _connectedPinLabel {nullptr};
    };

}