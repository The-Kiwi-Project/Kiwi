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

    class ExternalPortItem;

    class ExternalPortInfoWidget : public QWidget {
        Q_OBJECT

    public:
        ExternalPortInfoWidget(QWidget* parent = nullptr);

    signals:
        void exportRename(ExternalPortItem* eport, const QString& name);
        void exportSetCoord(ExternalPortItem* eport, hardware::TrackCoord& coord);
        void removeExPort(ExternalPortItem* eport);

    public:
        void loadExPort(ExternalPortItem* eport);

    protected:
        ExternalPortItem* _eport {nullptr};

        LineEditWithButton* _nameEdit {nullptr};
        
        QSpinBox* _rowSpinBox;
        QSpinBox* _colSpinBox;
        QComboBox* _dirComboBox;
        QSpinBox* _indexSpinBox;
        QPushButton* _setCoordButton;
    };

}