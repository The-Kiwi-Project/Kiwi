#pragma once

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QFileDialog>
#include <optional>

namespace kiwi::widget {

    class EntryDialog : public QDialog {
        Q_OBJECT

    public:
        explicit EntryDialog(QWidget *parent = nullptr);
        auto getResult() const -> std::optional<QString>;

    private slots:
        void onCreateEmptyProject();
        void onOpenExistingProject();

    private:
        std::optional<QString> result {};
    };

}