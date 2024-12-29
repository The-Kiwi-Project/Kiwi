#pragma once 

#include "../net.hh"
#include <std/collection.hh>
#include <hardware/cob/cob.hh>
#include <algo/router/maze/mazeroutestrategy.hh>
#include <algo/router/route.hh>


namespace kiwi::hardware {
    class Track;
    class Bump;
    class COB;
    class COBUnit;
}

namespace kiwi::circuit {

    class TracksToBumpsNet : public Net {
    public:
        TracksToBumpsNet(std::Vector<hardware::Track*> begin_tracks, std::Vector<hardware::Bump*>  end_bumps);

    public:
        virtual auto update_tob_postion(hardware::TOB* prev_tob, hardware::TOB* next_tob) -> void override;
        virtual auto route(hardware::Interposer* interposer, const algo::RouteStrategy& strategy) -> std::usize override;
        virtual auto update_priority(float bias) -> void override;
        virtual auto coords() const -> std::Vector<hardware::Coord> override;
        virtual auto check_accessable_cobunit() -> void override;
         virtual auto to_string() -> std::String override;
        virtual auto port_number() const -> std::usize override;

        auto begin_tracks() const -> const std::Vector<hardware::Track*>& { return this->_begin_tracks; }
        auto end_bumps() const -> const std::Vector<hardware::Bump*>& { return this->_end_bumps; }

    private:
        std::Vector<hardware::Track*> _begin_tracks;
        std::Vector<hardware::Bump*>  _end_bumps;
    };

}