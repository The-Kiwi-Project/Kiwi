#include "export.hh"

namespace kiwi::circuit {

    ExternalPort::ExternalPort(std::String name, const hardware::TrackCoord& coord) :
        _name{std::move(name)},
        _coord{coord}
    {

    }

}