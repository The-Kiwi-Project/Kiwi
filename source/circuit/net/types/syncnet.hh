#pragma once


#include "../net.hh"
#include "circuit/net/types/bbnet.hh"
#include "circuit/net/types/btnet.hh"
#include "circuit/net/types/tbnet.hh"
#include <algo/router/maze/mazeroutestrategy.hh>
#include <algo/router/route.hh>

#include <std/collection.hh>
#include <std/memory.hh>


namespace kiwi::hardware
{
    class TOB;
    class Interposer;
}

namespace kiwi::algo
{
    class RouteStrategy;
    struct RerouteStrategy;
}

namespace kiwi::circuit {

    class BumpToBumpNet;
    class BumpToTrackNet;
    class TrackToBumpNet;

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
        virtual auto route(hardware::Interposer* interposer, const algo::RouteStrategy& strategy) -> std::usize override;
        virtual auto update_priority(float bias) -> void override;
        virtual auto coords() const -> std::Vector<hardware::Coord> override;
        virtual auto check_accessable_cobunit() -> void override;
        virtual auto to_string() -> std::String override;
        virtual auto port_number() const -> std::usize override;
    
    public:
        auto btbnets() -> std::Vector<std::Box<BumpToBumpNet>>& {return this->_btbnets;}
        auto bttnets() -> std::Vector<std::Box<BumpToTrackNet>>& {return this->_bttnets;}
        auto ttbnets() -> std::Vector<std::Box<TrackToBumpNet>>& {return this->_ttbnets;}

    private:
        std::Vector<std::Box<BumpToBumpNet>>  _btbnets;
        std::Vector<std::Box<BumpToTrackNet>> _bttnets;
        std::Vector<std::Box<TrackToBumpNet>> _ttbnets;
    };

}



