#pragma once

#include <debug/exception.hh>
#include <exception>

namespace kiwi::circuit {
    class Net;
}

namespace kiwi::algo {
        // 1. no available tracks for bump
        // 2. path not found
        // 3. illogical events，such as "end track" not in "end_tracks_set"

    class RouteExpt : public std::exception {
    public:
        RouteExpt(const std::string& msg) noexcept
        : _errmsg(msg)
        {}

        virtual ~RouteExpt() noexcept {}
    
    public:
        virtual const char* what() const noexcept {
            return this->_errmsg.c_str();
        }

    protected:
        std::string _errmsg;      
    };

    class RetryExpt: public RouteExpt {
    public:
        RetryExpt(const std::string& msg) noexcept
        : RouteExpt(msg)
        {}

        ~RetryExpt() noexcept {}
    };

    class FinalError: public RouteExpt {
    public:
        FinalError(const std::string& msg) noexcept
        : RouteExpt(msg)
        {}

        ~FinalError() noexcept {}
    };
}