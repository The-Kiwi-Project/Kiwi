#pragma once

#include "./pin.hh"

namespace kiwi::circuit {

    class BaseDie;

    class Connection {
        friend BaseDie;

    private:
        Connection(int sync, Pin input, Pin output);

        void set_input(Pin input) { this->_input = std::move(input); } 
        void set_output(Pin output) { this->_input = std::move(output); }
        void set_sync(int sync) { this->_sync = sync; }

    public:
        auto sync() const -> int { return this->_sync; }
        auto input() const -> const Pin& { return this->_input; }
        auto output() const -> const Pin& { return this->_output; }
    
    private:
        int _sync;
        Pin _input;
        Pin _output;
    };

}