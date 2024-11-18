#pragma once 

namespace kiwi::hardware {

    enum class COBSwState {
        DisConnected,
        Connected,  
    };

    class COBSwRegister {
    public:
        COBSwRegister(COBSwState state):
            _state{state} {}
        
        COBSwRegister(): COBSwRegister{COBSwState::DisConnected} {}
        

        auto get() const -> COBSwState {
            return this->_state;
        }

        auto set(COBSwState state) -> void {
            this->_state = state;
        }

        operator bool() const {
            return this->get() == COBSwState::Connected;
        }

    private:
        COBSwState _state;
    };

    //////////////////////////////////////////////

    enum class COBSignalDirection {
        TrackToCOB,
        COBToTrack,
    };

    class COBSelRegister {
    public:
        COBSelRegister(COBSignalDirection direction):
            _direction{direction} {}

        COBSelRegister(): COBSelRegister{COBSignalDirection::TrackToCOB} {}

        auto get() const -> COBSignalDirection {
            return this->_direction;
        } 

        auto set(COBSignalDirection direction) -> void {
            this->_direction = direction;
        }

        auto set_cob_to_track() -> void {
            this->set(COBSignalDirection::COBToTrack);
        }

        auto set_track_to_cob() -> void {
            this->set(COBSignalDirection::TrackToCOB);
        }

        auto set_floating() -> void {
            this->set_track_to_cob();
        }

        operator bool() const {
            return this->get() == COBSignalDirection::COBToTrack;
        }

    private:
        COBSignalDirection _direction;
    };



}