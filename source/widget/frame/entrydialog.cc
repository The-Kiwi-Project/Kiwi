#include "./entrydialog.h"
#include "qlabel.h"

namespace kiwi::widget {

    EntryDialog::EntryDialog(QWidget *parent)
        : QDialog(parent)
    {
        this->setStyleSheet(
            "QPushButton {"
            "    background-color: white;"
            "    color: black;"
            "    border: 1px solid lightgray;"
            "    border-radius: 7px;"
            "    padding: 5px;"
            "}"
            "QPushButton:hover {"
            "    background-color: lightgray;"
            "}"
            "QPushButton:pressed {"
            "    background-color: gray;"
            "    color: white;"
            "}");

        auto layout = new QVBoxLayout(this);

        auto label = new QLabel{this};
        label->setText("Welcome to Kiwi");
        auto font = label->font();
        font.setPointSize(20);
        font.setBold(true);
        label->setFont(font);
        label->setAlignment(Qt::AlignCenter);
        layout->addWidget(label);

        auto createButton = new QPushButton("Empty Project", this);
        createButton->setFixedHeight(40);
        auto openButton = new QPushButton("Load Config", this);
        openButton->setFixedHeight(40);
        layout->addWidget(createButton);
        layout->addWidget(openButton);

        connect(createButton, &QPushButton::clicked, this, &EntryDialog::onCreateEmptyProject);
        connect(openButton, &QPushButton::clicked, this, &EntryDialog::onOpenExistingProject);

        this->setFixedSize(400, 300);
        this->setLayout(layout);
    }

    auto EntryDialog::getResult() const -> std::optional<QString> {
        return result;
    }

    void EntryDialog::onCreateEmptyProject() {
        this->accept();
    }

    void EntryDialog::onOpenExistingProject() {
        auto filePath = QFileDialog::getExistingDirectory(this, "Select Config path");
        if (!filePath.isEmpty()) {
            result.emplace(filePath);
            this->accept();
        }
    }

}
