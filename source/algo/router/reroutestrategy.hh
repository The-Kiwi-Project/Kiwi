#pragma once

#include <hardware/interposer.hh>

#include <std/collection.hh>
#include <std/integer.hh>
#include <std/utility.hh>


namespace kiwi::hardware {
    class Interposer;
    class Track;
    class COBConnector;
    class TOBConnector;
    class Bump;
}

namespace kiwi::algo{

    struct RerouteStrategy {
        using routed_path = std::Vector<std::Tuple<kiwi::hardware::Track *, std::Option<kiwi::hardware::COBConnector>>>;

        virtual ~RerouteStrategy() = default;

        virtual auto reroute(              
            hardware::Interposer* interposer, std::Vector<routed_path*>& path_ptrs,
            std::usize max_length, const std::Vector<std::Option<hardware::Bump*>>& end_bumps,
            std::Vector<std::HashMap<hardware::Track*, hardware::TOBConnector>*>& end_track_to_tob_maps,
            std::usize bump_length
        ) const -> std::tuple<bool, std::usize> = 0;

        virtual auto path_length(const routed_path& path, bool switch_length = false) const -> std::usize = 0;
    };
}
