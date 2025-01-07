#pragma once

#include <QWidget>
#include <hardware/track/trackcoord.hh>

class QLabel;
class QSpinBox;
class QTableView;
class QLineEdit;
class QComboBox;
class QPushButton;

namespace kiwi::widget {
    class LineEditWithButton;
}

namespace kiwi::widget::schematic {

    class ExPortItem;

    class ExPortItemInfoWidget : public QWidget {
        Q_OBJECT

    public:
        ExPortItemInfoWidget(QWidget* parent = nullptr);

    signals:
        void exportRename(ExPortItem* eport, const QString& name);
        void exportSetCoord(ExPortItem* eport, hardware::TrackCoord& coord);

    public:
        void loadExPort(ExPortItem* eport);

    protected:
        ExPortItem* _eport {nullptr};

        LineEditWithButton* _nameEdit {nullptr};
        
        QSpinBox* _rowSpinBox;
        QSpinBox* _colSpinBox;
        QComboBox* _dirComboBox;
        QSpinBox* _indexSpinBox;
        QPushButton* _setCoordButton;
    };

}