#include "./tobinfo.h"
#include "qchar.h"
#include "qlabel.h"
#include "qspinbox.h"
#include "qstringliteral.h"
#include "std/collection.hh"
#include "std/integer.hh"
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

namespace kiwi::widget {

    TOBInfoDialog::TOBInfoDialog(hardware::TOB* tob) :
        QDialog{},
        _tob{tob}
    {
        this->setWindowTitle("COB Info");
        
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
        auto baseLayout1 = new QHBoxLayout {};
        auto coordLabel = new QLabel {baseGroup};
        coordLabel->setText("Coord");
        baseLayout1->addWidget(coordLabel);
        auto coordValueLabel = new QLabel {baseGroup};
        coordValueLabel->setText(QString{"(%1, %2)"}.
            arg(this->_tob->coord().row).
            arg(this->_tob->coord().col));
        baseLayout1->addWidget(coordValueLabel);

        baseLayout->addLayout(baseLayout1);

        //// Register Inquiry
        auto inquiryGroup = new QGroupBox {this};
        inquiryGroup->setTitle("Register Inquiry");
        auto inquiryLayout = new QVBoxLayout {inquiryGroup};
        inquiryLayout->setContentsMargins(10, 10, 10, 10);
        layout->addWidget(inquiryGroup);

        // Bump index spinbox
        auto inquiryLayout1 = new QHBoxLayout {};
        inquiryLayout1->setContentsMargins(0, 0, 0, 0);

        auto bumpIndexLabel = new QLabel {inquiryGroup};
        bumpIndexLabel->setText("Bump Index");
        inquiryLayout1->addWidget(bumpIndexLabel);

        this->_bumpIndexSpinBox = new QSpinBox {inquiryGroup};
        this->_bumpIndexSpinBox->setMaximum(hardware::TOB::INDEX_SIZE - 1);
        this->_bumpIndexSpinBox->setMinimum(0);
        inquiryLayout1->addWidget(this->_bumpIndexSpinBox);

        inquiryLayout->addLayout(inquiryLayout1);

        //// Optional 
        auto optionalGroup = new QGroupBox {this};
        optionalGroup->setTitle("Register Inquiry");
        auto optionalLayout = new QVBoxLayout {optionalGroup};
        optionalLayout->setContentsMargins(10, 10, 10, 10);
        layout->addWidget(optionalGroup);

        // Connection info label
        this->_connectionLabel = new QLabel {optionalGroup};
        optionalLayout->addWidget(this->_connectionLabel);

        // Tree view
        this->_treeView = new QTreeView {optionalGroup};
        optionalLayout->addWidget(this->_treeView);

        this->_model = new QStandardItemModel {this->_treeView};
        this->_treeView->setModel(this->_model);

        /////////////////////////////////////////////////////////////////////////
        this->connect(
            this->_bumpIndexSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), 
            this, &TOBInfoDialog::updateTreeView);

        this->setMinimumSize(400, 400);
        this->updateTreeView(this->_bumpIndexSpinBox->value());
    }

    auto TOBInfoDialog::updateTreeView(int bumpIndex) -> void {
        auto connectors = this->_tob->available_connectors_bump_to_track(bumpIndex);
    
        using TrackIndexes = std::Vector<std::usize>;
        using VertIndexes = std::HashMap<std::usize, TrackIndexes>;
        using HoriIndexes = std::HashMap<std::usize, VertIndexes>;

        auto horiIndexes = HoriIndexes{};

        for (const auto& connector : connectors) {
            auto horiIndex = connector.hori_index();
            auto vertIndex = connector.vert_index();
            auto trackIndex = connector.track_index();

            auto& vertIndexes = horiIndexes.emplace(horiIndex, VertIndexes{}).first->second;
            auto& trackIndexes = vertIndexes.emplace(vertIndex, TrackIndexes{}).first->second;
            trackIndexes.emplace_back(trackIndex);
        }

        this->_model->clear();
        this->_model->setHorizontalHeaderLabels(QStringList{QString{"Optional settings: %1"}.arg(connectors.size())});  
        
        if (connectors.size() == 0) {
            // Already connect!
            auto hortIndex = this->_tob->bump_index_map_hori_index(bumpIndex).value();
            auto veriIndex = this->_tob->hori_index_map_vert_index(hortIndex).value();
            auto trackIndex = this->_tob->vert_index_map_track_index(veriIndex).value();

            this->_connectionLabel->setText(QString{
                "Bump connected in path: %1 > %2 > %3 > %4"}
                .arg(bumpIndex).arg(hortIndex).arg(veriIndex).arg(trackIndex)
            );
        }
        else {
            this->_connectionLabel->setText(QStringLiteral("Bump unconnected"));

            auto itemRoot = this->_model->invisibleRootItem(); 
            for (const auto& [horiIndex, vertIndexes] : horiIndexes) {
                auto horiItem = new QStandardItem(QString{"hori index %1"}.arg(horiIndex));
                itemRoot->appendRow(horiItem);

                for (const auto& [vertIndex, trackIndexes] : vertIndexes) {
                    auto vertItem = new QStandardItem(QString{"vert index %1"}.arg(vertIndex));
                    horiItem->appendRow(vertItem);

                    for (auto trackIndex : trackIndexes) {
                        auto trackItem = new QStandardItem(QString{"track index %1"}.arg(trackIndex));
                        vertItem->appendRow(trackItem);
                    }
                }
            }
        }
    }

    TOBInfoDialog::~TOBInfoDialog() {}

}