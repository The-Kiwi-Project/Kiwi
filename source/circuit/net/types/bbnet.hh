#pragma once 

#include "../net.hh"
#include "../../topdie/topdieinst.hh"
#include <hardware/tob/tob.hh>
#include <std/integer.hh>
#include <algo/router/maze/mazeroutestrategy.hh>
#include <algo/router/route.hh>


namespace kiwi::hardware {
    class Track;
    class Bump;
}

namespace kiwi::circuit {

    class BumpToBumpNet : public Net {
    public:
        BumpToBumpNet(hardware::Bump* begin_bump, hardware::Bump* end_bump);
        virtual ~BumpToBumpNet() noexcept; 

    public:
        virtual auto update_tob_postion(hardware::TOB* prev_tob, hardware::TOB* next_tob) -> void override;
        virtual auto route(hardware::Interposer* interposer, const algo::RouteStrategy& strategy) -> void override;
        virtual auto priority() const -> Priority override;
        virtual auto coords() const -> std::Vector<hardware::Coord> override;

        auto begin_bump() const -> hardware::Bump* { return this->_begin_bump; }
        auto end_bump() const -> hardware::Bump* { return this->_end_bump; }

    private:
        hardware::Bump* _begin_bump;
        hardware::Bump* _end_bump;
    };

}