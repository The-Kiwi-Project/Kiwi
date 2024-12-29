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
}

namespace kiwi::circuit {

    class BumpToBumpsNet : public Net {
    public:
        BumpToBumpsNet(hardware::Bump* begin_bump, std::Vector<hardware::Bump*> end_bumps);
        virtual ~BumpToBumpsNet() noexcept override;
    
    public:
        virtual auto update_tob_postion(hardware::TOB* prev_tob, hardware::TOB* next_tob) -> void override;
        virtual auto route(hardware::Interposer* interposer, const algo::RouteStrategy& strategy) -> std::usize override;
        virtual auto update_priority(float bias) -> void override;
        virtual auto coords() const -> std::Vector<hardware::Coord> override;
        virtual auto check_accessable_cobunit() -> void override;
        virtual auto port_number() const -> std::usize override;

        auto begin_bump() const -> hardware::Bump* { return this->_begin_bump; }
        auto end_bumps() const -> const std::Vector<hardware::Bump*>& { return this->_end_bumps; }

    private:
        hardware::Bump* _begin_bump;
        std::Vector<hardware::Bump*> _end_bumps;
    };

}