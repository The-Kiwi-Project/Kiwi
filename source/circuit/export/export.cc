#include "export.hh"
#include "debug/debug.hh"
#include "hardware/interposer.hh"

namespace kiwi::circuit {

    ExternalPort::ExternalPort(std::String name, const hardware::TrackCoord& coord) :
        _name{std::move(name)},
        _coord{coord}
    {
        debug::check_fmt(
            hardware::Interposer::is_external_port_coord(coord),
            "{} is not a valid external port coord!",
            coord
        );
    }

}