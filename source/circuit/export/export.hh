#pragma once

#include "hardware/track/trackcoord.hh"
#include <std/string.hh>

namespace kiwi::circuit {

    class ExternalPort {
    public:
        ExternalPort(std::String name, const hardware::TrackCoord& coord);

    public:
        auto name() const -> const std::String& { return this->_name; }
        auto name_view() const -> std::StringView { return this->_name; }
        auto coord() const -> const hardware::TrackCoord& { return this->_coord; }

        void set_name(std::String name) { this->_name = std::move(name); }
        void set_coord(const hardware::TrackCoord& coord) { this->_coord = coord; }

    private:
        std::String _name;
        hardware::TrackCoord _coord;
    };

}