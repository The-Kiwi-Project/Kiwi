#pragma once 

#include "../net.hh"
#include <std/collection.hh>
#include <algo/router/maze/mazeroutestrategy.hh>
#include <algo/router/route.hh>
#include <hardware/cob/cob.hh>


namespace kiwi::hardware {
    class Track;
    class Bump;
    class COB;
    class COBUnit;
}

namespace kiwi::circuit {

    class TrackToBumpsNet : public Net {
    public:
        TrackToBumpsNet(hardware::Track* begin_track, std::Vector<hardware::Bump*> end_bumps);
        virtual ~TrackToBumpsNet() noexcept override;

    public:
        virtual auto update_tob_postion(hardware::TOB* prev_tob, hardware::TOB* next_tob) -> void override;
        virtual auto route(hardware::Interposer* interposer, const algo::RouteStrategy& strategy) -> std::usize override;
        virtual auto priority() const -> Priority override;
        virtual auto check_accessable_cobunit() -> void override;

        auto begin_track() const -> hardware::Track* { return this->_begin_track; }
        auto end_bumps() const -> const std::Vector<hardware::Bump*>& { return this->_end_bumps; }
        virtual auto coords() const -> std::Vector<hardware::Coord> override;

    private:
        hardware::Track* _begin_track;
        std::Vector<hardware::Bump*> _end_bumps;
    };

}