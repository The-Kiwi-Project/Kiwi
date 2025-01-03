#pragma once

#include <QDialog>

class QComboBox;
class QSpinBox;
class QTreeView;
class QStandardItemModel;
class QLabel;

namespace kiwi::hardware {
    class TOB;
}

namespace kiwi::widget {

    class TOBInfoDialog : public QDialog {
    public:
        TOBInfoDialog(hardware::TOB* tob);
        ~TOBInfoDialog();
        
    private:
        auto updateTreeView(int bumpIndex) -> void;

    private:
        hardware::TOB* _tob;

        QSpinBox* _bumpIndexSpinBox;
        QLabel* _connectionLabel;
        QTreeView* _treeView;
        QStandardItemModel *_model;
    };

}