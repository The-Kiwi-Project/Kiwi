#pragma once 

#include <std/utility.hh>
#include <std/integer.hh>

namespace kiwi::hardware {

    class TOBMuxRegister {
    public:
        TOBMuxRegister(std::usize index) :
            _index{index} {}

        TOBMuxRegister() = default;

        auto get() const -> std::Option<std::usize> {
            return this->_index;
        }

        // Warning!!! All 'TOBMuxRegister' in one TOBMux must `None` or have different value. 
        // Becarefull when you call `set` directly 
        auto set(std::usize index) -> void {
            this->_index.emplace(index);
        }

        auto reset() -> void {
            this->_index.reset();
        }

    private:
        std::Option<std::usize> _index;
    };

    //////////////////////////////////////////////////

    enum class TOBBumpDirection {
        DisConnected,
        BumpToTOB,
        TOBToBump,
    };

    class TOBBumpDirRegister {
    public:
        TOBBumpDirRegister(TOBBumpDirection direction) :
            _direction{direction} {}

        TOBBumpDirRegister() :
            TOBBumpDirRegister{TOBBumpDirection::DisConnected} {}

        auto get() const -> TOBBumpDirection { 
            return this->_direction; 
        }

        auto set(TOBBumpDirection direction) -> void {
            this->_direction = direction;
        } 

        auto reset() -> void {
            this->set(TOBBumpDirection::DisConnected);
        }

    private:
        TOBBumpDirection _direction;
    };

    //////////////////////////////////////////////////

    enum class TOBTrackDirection {
        DisConnected,
        TrackToTOB,
        TOBToTrack,
    }; 

    class TOBTrackDirRegister {
    public:
        TOBTrackDirRegister(TOBTrackDirection direction) :
            _direction{direction} {}

        TOBTrackDirRegister() :
            TOBTrackDirRegister{TOBTrackDirection::DisConnected} {}

        auto get() const -> TOBTrackDirection { 
            return this->_direction; 
        }

        auto set(TOBTrackDirection direction) -> void {
            this->_direction = direction;
        } 

        auto reset() -> void {
            this->set(TOBTrackDirection::DisConnected);
        }

    private:
        TOBTrackDirection _direction;
    };

}