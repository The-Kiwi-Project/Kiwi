#pragma once


#include "../net.hh"
#include "circuit/net/types/bbnet.hh"
#include "circuit/net/types/btnet.hh"
#include "circuit/net/types/tbnet.hh"
#include <std/collection.hh>
#include <algo/router/maze/mazeroutestrategy.hh>
#include <algo/router/route.hh>


namespace kiwi::hardware
{
    class TOB;
    class Interposer;
}

namespace kiwi::algo
{
    class RouteStrategy;
}

namespace kiwi::circuit {

    class SyncNet : public Net {   
    public:
        // temperarily use shallow copy
        SyncNet(
            std::Vector<std::Box<BumpToBumpNet>> btbnets,
            std::Vector<std::Box<BumpToTrackNet>> bttnets,
            std::Vector<std::Box<TrackToBumpNet>> ttbnets
        );
        
    public:
        virtual auto update_tob_postion(hardware::TOB* prev_tob, hardware::TOB* next_tob) -> void override;
        virtual auto route(hardware::Interposer* interposer, const algo::RouteStrategy& strategy) -> void override;
        virtual auto priority() const -> Priority override;
        virtual auto coords() const -> std::Vector<hardware::Coord> override;

    private:
        std::Vector<std::Box<BumpToBumpNet>>  _btbnets;
        std::Vector<std::Box<BumpToTrackNet>> _bttnets;
        std::Vector<std::Box<TrackToBumpNet>> _ttbnets;
    };

}



