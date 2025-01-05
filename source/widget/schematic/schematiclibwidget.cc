#include "./schematiclibwidget.h"
#include "circuit/topdie/topdie.hh"
#include <circuit/basedie.hh>
#include "parse/reader/config/topdie.hh"
#include "qboxlayout.h"
#include "qchar.h"
#include "qglobal.h"
#include "qlineedit.h"
#include "qpushbutton.h"
#include "serde/de.hh"
#include "serde/json/json.hh"
#include "std/collection.hh"
#include "std/exception.hh"
#include "std/file.hh"
#include "std/string.hh"

#include <QVBoxLayout>
#include <QScrollArea>
#include <QLineEdit>
#include <QPushButton>
#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>

namespace kiwi::widget {

    SchematicLibWidget::SchematicLibWidget(circuit::BaseDie* basedie, QWidget* parent) :
        QWidget{parent},
        _basedie{basedie}
    {
        auto thisLayout = new QVBoxLayout{this};
        thisLayout->setSpacing(10);

        auto searchEdit = new QLineEdit {this};

        auto buttonWidget = new QWidget {this};
        auto buttonLayout = new QHBoxLayout {buttonWidget};
        auto loadTopDieButton = new QPushButton {"+", buttonWidget};
        auto loadTopDiesButton = new QPushButton {"++", buttonWidget};
        buttonLayout->addWidget(loadTopDieButton);
        buttonLayout->addWidget(loadTopDiesButton);

        auto libraryScrollArea = new QScrollArea {this};
        libraryScrollArea->setWidgetResizable(true);

        thisLayout->addWidget(searchEdit);
        thisLayout->addWidget(buttonWidget);
        thisLayout->addWidget(libraryScrollArea);
        
        auto libraryWidget = new QWidget;
        libraryWidget->setStyleSheet("background-color: white;");
        this->_libraryLayout = new QVBoxLayout(libraryWidget);
        this->_libraryLayout->addStretch();

        libraryScrollArea->setWidget(libraryWidget);

        for (auto& [_, topdie] : this->_basedie->topdies()) {
            this->addTopDie(topdie.get());
        }

        connect(loadTopDieButton, &QPushButton::clicked, this, &SchematicLibWidget::onLoadTopDieClicked);
        connect(loadTopDiesButton, &QPushButton::clicked, this, &SchematicLibWidget::onLoadTopDiesClicked);
    }

    void SchematicLibWidget::onLoadTopDieClicked() try {
        auto filePath = QFileDialog::getOpenFileName(
            this,
            tr("Select TopDie Config"),
            QDir::currentPath(),
            tr("JSON File (*.json);;All File (*)")
        );

        if (!filePath.isEmpty()) {
            this->loadTopDie(filePath);
        }
    }
    catch (const std::Exception& err) {
        QMessageBox::critical(
            this,
            "Load TopDie Error",
            QString::fromStdString(err.what())
        );
    }

    void SchematicLibWidget::onLoadTopDiesClicked() try {
        auto filePath = QFileDialog::getOpenFileName(
            this,
            tr("Select TopDies Config"),
            QDir::currentPath(),
            tr("JSON File (*.json);;All File (*)")
        );

        if (!filePath.isEmpty()) {
            this->loadTopDies(filePath);
        }
    }
    catch (const std::Exception& err) {
        QMessageBox::critical(
            this,
            "Load TopDies Error",
            QString::fromStdString(err.what())
        );
    }

    void SchematicLibWidget::loadTopDie(const QString& path) {
        auto filepath = std::FilePath{path.toStdString()};
        auto topdieConfig = serde::deserialize_from<serde::Json, parse::TopDieConfig>(filepath);

        this->addTopDie(filepath.stem().string(), std::move(topdieConfig.pin_map));
    }

    void SchematicLibWidget::loadTopDies(const QString& path) {
        auto filepath = std::FilePath{path.toStdString()};
        auto topdieConfigs = 
            serde::deserialize_from<serde::Json, std::HashMap<std::String, parse::TopDieConfig>>(filepath);

        for (auto& [name, config] : topdieConfigs) {
            this->addTopDie(name, std::move(config.pin_map));
        }
    }

    void SchematicLibWidget::addTopDie(std::String name, std::HashMap<std::String, std::usize> pinmap) {
        auto topdie = this->_basedie->add_topdie(std::move(name), std::move(pinmap));
        this->addTopDie(topdie);
    }

    void SchematicLibWidget::addTopDie(circuit::TopDie* topdie) {
        auto button = new QPushButton(QString::fromStdString(topdie->name().data()));
        button->setMinimumHeight(50);
        button->setStyleSheet(
                "QPushButton {"
                "    background-color: white;"
                "    color: black;"
                "    border: 1px solid lightgray;"
                "    border-radius: 5px;"
                "    padding: 5px;"
                "}"
                "QPushButton:hover {"
                "    background-color: lightgray;"
                "}"
                "QPushButton:pressed {"
                "    background-color: gray;"
                "    color: white;"
                "}");
        this->_libraryLayout->insertWidget(0, button); 

        connect(button, &QPushButton::clicked, [this, topdie] () {
            
            emit this->initialTopDieInst(topdie);
        });
    }
}