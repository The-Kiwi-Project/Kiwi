#include "./topdieinstinfo.h"
#include "qboxlayout.h"
#include "qchar.h"
#include "qlabel.h"
#include "qlineedit.h"
#include "qnamespace.h"
#include "qsizepolicy.h"
#include "qstandarditemmodel.h"
#include "qtableview.h"
#include <circuit/topdieinst/topdieinst.hh>
#include <hardware/tob/tob.hh>
#include <hardware/bump/bump.hh>

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QSpinBox>
#include <QComboBox>
#include <QTreeView>
#include <QTableView>
#include <QListView>
#include <QStandardItemModel>
#include <QHeaderView>

namespace kiwi::widget {

    TopDieInsDialog::TopDieInsDialog(circuit::TopDieInstance* topdieinst) :
        QDialog{},
        _topdieinst{topdieinst}
    {
        this->setWindowTitle("TopDie Instance Info");
        
        auto layout = new QVBoxLayout {this};
        layout->setContentsMargins(10, 10, 10, 10);
        layout->setAlignment(Qt::AlignTop);
        this->setLayout(layout);

        //// Base info
        auto baseGroup = new QGroupBox {this};
        baseGroup->setTitle("Base Infomation");
        auto baseLayout = new QVBoxLayout {baseGroup};
        baseLayout->setContentsMargins(10, 10, 10, 10);
        layout->addWidget(baseGroup);
        
        // coord
        auto baseLayout0 = new QHBoxLayout {};
        auto nameLabel = new QLabel {baseGroup};
        nameLabel->setText("Inst Name");
        baseLayout0->addWidget(nameLabel);
        auto nameValueLabel = new QLabel {baseGroup};
        nameValueLabel->setText(this->_topdieinst->name().data());
        baseLayout0->addWidget(nameValueLabel);

        baseLayout->addLayout(baseLayout0);

        auto baseLayout2 = new QHBoxLayout {};
        auto topdieNameLabel = new QLabel {baseGroup};
        topdieNameLabel->setText("TopDie Name");
        baseLayout2->addWidget(topdieNameLabel);
        auto topdieNameValueLabel = new QLabel {baseGroup};
        topdieNameValueLabel->setText(this->_topdieinst->topdie()->name().data());
        baseLayout2->addWidget(topdieNameValueLabel);

        baseLayout->addLayout(baseLayout2);

        auto baseLayout1 = new QHBoxLayout {};
        auto coordLabel = new QLabel {baseGroup};
        coordLabel->setText("TOB Coord");
        baseLayout1->addWidget(coordLabel);
        auto coordValueLabel = new QLabel {baseGroup};
        coordValueLabel->setText(QString{"(%1, %2)"}.
            arg(this->_topdieinst->tob()->coord().row).
            arg(this->_topdieinst->tob()->coord().col));
        baseLayout1->addWidget(coordValueLabel);

        baseLayout->addLayout(baseLayout1);

        //// Pin map
        auto pinmapGroup = new QGroupBox {this};
        pinmapGroup->setTitle("Pin Map");
        auto pinmapLayout = new QVBoxLayout {pinmapGroup};
        pinmapLayout->setContentsMargins(10, 10, 10, 10);
        layout->addWidget(pinmapGroup);

        // Seach pin
        auto pinmapLayout0 = new QHBoxLayout {};

        auto searchLabel = new QLabel {pinmapGroup};
        searchLabel->setText("Search pin name");
        pinmapLayout0->addWidget(searchLabel);

        auto pinNameLineEdit = new QLineEdit {pinmapGroup};
        pinmapLayout0->addWidget(pinNameLineEdit);
        
        auto bumpIndexLabel = new QLabel {pinmapGroup};
        bumpIndexLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        bumpIndexLabel->setAlignment(Qt::AlignRight);
        pinmapLayout0->addWidget(bumpIndexLabel);

        pinmapLayout->addLayout(pinmapLayout0);

        this->connect(pinNameLineEdit, &QLineEdit::textChanged, [this, bumpIndexLabel](const QString& pinName) {
            const auto& pinMap = this->_topdieinst->topdie()->pins_map();
            auto res = pinMap.find(pinName.toStdString());
            if (res == pinMap.end()) {
                bumpIndexLabel->setText(QString{"Pin '%1' no found"}.arg(pinName));
            } else {
                bumpIndexLabel->setText(QString{"Bump Index: %1"}.arg(res->second));
            }
        });

        // Tree view
        auto pinMapTableView = new QTableView {pinmapGroup};
        pinMapTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        pinmapLayout->addWidget(pinMapTableView);

        // Create model
        const auto& pinmap = this->_topdieinst->topdie()->pins_map();
        auto pinMapModel = new QStandardItemModel {
            static_cast<int>(pinmap.size()), 2,
            pinMapTableView
        };
        
        // Init header
        pinMapModel->setHorizontalHeaderLabels(QStringList{"Pin Name", "Bump Index"}); 
        // Add items
        auto itemRoot = pinMapModel->invisibleRootItem();
        int row = 0;
        for (auto& [pinName, bumpIndex] : pinmap) {
            auto nameItem = new QStandardItem {pinName.c_str()};
            auto indexItem = new QStandardItem {QString{"%1"}.arg(bumpIndex)};
            pinMapModel->setItem(row, 0, nameItem);
            pinMapModel->setItem(row, 1, indexItem);
            row += 1;
        }
        
        pinMapTableView->setModel(pinMapModel);
        
        //// Net info
        auto netGroup = new QGroupBox {this};
        netGroup->setTitle("Net Infomation");
        auto netLayout = new QVBoxLayout {netGroup};
        netLayout->setContentsMargins(10, 10, 10, 10);
        layout->addWidget(netGroup);        

        auto netTableView = new QTableView {netGroup};
        netTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        netLayout->addWidget(netTableView);

        const auto& nets = this->_topdieinst->nets();
        auto netModel = new QStandardItemModel {
            static_cast<int>(nets.size()), 3,
            pinMapTableView
        };

        // Init header
        // MARK: How to show nets?
        netModel->setHorizontalHeaderLabels(QStringList{"Begin", "End", "Priority"}); 
        // Add items
        itemRoot = netModel->invisibleRootItem();
        for (auto row = 0; row < nets.size(); ++row) {
        }
        netTableView->setModel(netModel);

        this->setMinimumSize(600, 600);
    }

    TopDieInsDialog::~TopDieInsDialog() {}

}