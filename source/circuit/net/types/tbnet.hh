#pragma once 

#include "../net.hh"
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

    class TrackToBumpNet : public Net {
    public:
        TrackToBumpNet(hardware::Track* begin_track, hardware::Bump* end_bump);
        virtual ~TrackToBumpNet() noexcept override;
    
    public:
        virtual auto update_tob_postion(hardware::TOB* prev_tob, hardware::TOB* next_tob) -> void override;
        virtual auto route(hardware::Interposer* interposer, const algo::RouteStrategy& strategy) -> std::usize override;
        virtual auto update_priority(float bias) -> void override;
        virtual auto check_accessable_cobunit() -> void override; 
        virtual auto to_string() const -> std::String override;
        virtual auto port_number() const -> std::usize override;

    public:
        auto begin_track() const -> hardware::Track* { return this->_begin_track; }
        auto end_bump() const -> hardware::Bump* { return this->_end_bump; }
        virtual auto coords() const -> std::Vector<hardware::Coord> override;

    private:
        hardware::Track* _begin_track;
        hardware::Bump* _end_bump;
    };

}