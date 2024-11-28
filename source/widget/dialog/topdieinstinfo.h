#pragma once

#include <QDialog>

class QComboBox;
class QSpinBox;
class QTreeView;
class QStandardItemModel;
class QLabel;

namespace kiwi::circuit {
    class TopDieInstance;
}

namespace kiwi::widget {

    class TopDieInsDialog : public QDialog {
    public:
        TopDieInsDialog(circuit::TopDieInstance* topdieinst);
        ~TopDieInsDialog();
        
    private:
        auto updateTreeView(int bumpIndex) -> void;

    private:
        circuit::TopDieInstance* _topdieinst;
    };

}