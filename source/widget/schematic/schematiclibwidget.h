#pragma once

#include <std/memory.hh>
#include <QWidget>
#include <circuit/topdie/topdie.hh>

class QLineEdit;
class QVBoxLayout;

namespace kiwi::circuit {
    class BaseDie;
};

namespace kiwi::widget {

    class SchematicLibWidget : public QWidget {
        Q_OBJECT

    public:
        SchematicLibWidget(circuit::BaseDie* basedie, QWidget* parent = nullptr);

    signals:
        void initialTopDieInst(circuit::TopDie* topdie);
        void addExport();

    public:
        void onLoadTopDieClicked();
        void onLoadTopDiesClicked();

        void loadTopDie(const QString& path);
        void loadTopDies(const QString& path);
            
        void addTopDie(std::String name, std::HashMap<std::String, std::usize> pinmap);
        void addTopDie(circuit::TopDie* topdie);

    private:
        QVBoxLayout* _libraryLayout;

        circuit::BaseDie* _basedie {nullptr};
    };

}