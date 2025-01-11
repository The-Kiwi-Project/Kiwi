#pragma once

#include "qobject.h"
#include <circuit/basedie.hh>
#include <circuit/export/export.hh>
#include <QGraphicsScene>
#include <QHash>

namespace kiwi::hardware {
    class TOB;
    class Interposer;
}

namespace kiwi::circuit {
    class TopDieInstance;
    class BaseDie;
}

namespace kiwi::widget {

    namespace layout {
        class NetItem;
        class PinItem;
        class TOBItem;
        class TopDieInstanceItem;
        class ExternalPortItem;
        class SourcePortItem;
    }

    class LayoutScene : public QGraphicsScene {   
        static constexpr qreal TOB_INTERVAL = 30.0;
        static constexpr qreal INTERPOSER_SIDE_GAP = 30.0;
        static constexpr qreal EXPORT_SIDE_GAP = 50.0;

        static_assert(EXPORT_SIDE_GAP > INTERPOSER_SIDE_GAP);

        static const   QPointF INTERPOSER_LEFT_DOWN_POSITION;
        static const   QPointF INTERPOSER_RIGHT_UP_POSITION;

        static const   QPointF EXPORT_LEFT_DOWN_POSITION;
        static const   QPointF EXPORT_RIGHT_UP_POSITION;

    public:
        LayoutScene(hardware::Interposer* interposer, circuit::BaseDie* basedie, QObject* parent = nullptr);

    public:
        auto addNet(layout::PinItem* beginPoint, layout::PinItem* endPoint) -> layout::NetItem*;
        auto addTOB(hardware::TOB* tob) -> layout::TOBItem*;
        auto addTopDieInstance(circuit::TopDieInstance* topdieInst) -> layout::TopDieInstanceItem*;
        auto addExternalPort(circuit::ExternalPort* eport) -> layout::ExternalPortItem*;
        auto addVDDSourcePort() -> layout::SourcePortItem*;
        auto addGNDSourcePort() -> layout::SourcePortItem*;

    private:
        void addSceneItems();

        void addTOBItems();
        void addTopDieInstanceItems();
        void addExternalPortItems();
        void addNetItems();

    private:
        auto circuitPinToPinItem(const circuit::Pin& pin) -> layout::PinItem*;
        
    private:
        static auto tobPosition(const hardware::TOBCoord& coord) -> QPointF;

    protected:
        circuit::BaseDie* _basedie;
        hardware::Interposer* _interposer;

        QHash<circuit::TopDieInstance*, layout::TopDieInstanceItem*> _topdieinstMap {};
        QHash<circuit::ExternalPort*, layout::ExternalPortItem*> _externalPortsMap {};
        QHash<hardware::TOB*, layout::TOBItem*> _tobsMaps {};
    };


}