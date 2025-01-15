#include "./topdieinst.hh"
#include <hardware/tob/tob.hh>
#include <hardware/bump/bump.hh>
#include <cassert>

namespace kiwi::circuit {

    TopDieInstance::TopDieInstance(std::String name, TopDie* topdie, hardware::TOB* tob) :
        _name{std::move(name)},
        _topdie{topdie},
        _tob{tob},
        _nets{}
    {
    }

    void TopDieInstance::place_to_idle_tob(hardware::TOB* tob) {
        assert(this->_tob.has_value());
        assert(tob != nullptr);
        if (!tob->is_idle()) {
            return;
        }

        auto origin_tob = this->tob();
        origin_tob->remove_placed_instance();
        this->_tob.emplace(tob);
        tob->set_placed_instance(this);
    }
    
    void TopDieInstance::swap_tob_with_(TopDieInstance* other){
        assert(other != nullptr);

        assert(this->_tob.has_value());
        assert(other->_tob.has_value());

        auto this_tob = this->tob();
        auto other_tob = other->tob();

        // NO IDEL!
        assert(!this_tob->is_idle());
        assert(!other_tob->is_idle());

        assert(this_tob->placed_instance() == this);
        assert(other_tob->placed_instance() == other);

        this->_tob.emplace(other_tob);
        other->_tob.emplace(this_tob);

        this_tob->set_placed_instance(other);
        other_tob->set_placed_instance(this);
    }

    auto TopDieInstance::swap_tob_with(TopDieInstance* other) -> void {
        auto this_tob = this->_tob;
        auto other_tob = other->_tob;

        assert(this_tob.has_value());
        assert(other_tob.has_value());

        this->move_to_tob(*other_tob);
        other->move_to_tob(*this_tob);
    }

    auto TopDieInstance::move_to_tob(hardware::TOB* tob) -> void {
        assert(tob != nullptr);

        auto prev_tob = this->_tob;
        auto next_tob = tob;
        this->_tob = next_tob;
        for (auto net : this->_nets) {
            assert(prev_tob.has_value());
            net->update_tob_postion(*prev_tob, next_tob);
        }
    }

    auto TopDieInstance::add_net(Net* net) -> void {
        this->_nets.emplace_back(net);
    }
}