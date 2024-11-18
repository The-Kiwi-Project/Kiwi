#pragma once

#include <hardware/cob/cob.hh>

#include <std/collection.hh>
#include <std/integer.hh>
#include <std/utility.hh>


namespace kiwi::hardware {
    class Interposer;
    class Track;
    class COBConnector;
}

namespace kiwi::algo{

    struct RerouteStrategy {
        using routed_path = std::Vector<std::Tuple<kiwi::hardware::Track *, std::Option<kiwi::hardware::COBConnector>>>;

        virtual ~RerouteStrategy() = default;

        virtual auto reroute(              
            hardware::Interposer* interposer, std::Vector<routed_path*>& path_ptrs,     
            std::usize max_length, const std::Vector<std::HashSet<hardware::Track*>>& end_tracks
        ) const -> std::tuple<bool, std::usize> = 0;
    };
}
