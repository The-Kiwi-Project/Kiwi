#include "./path_length.hh"
#include "../routeerror.hh"


namespace kiwi::algo {

    auto path_length(const routed_path& path, bool switch_length) -> std::usize {
        if (path.size() == 0){
            throw FinalError("path_length: path is empty");
        }

        // path length = number of tracks
        if (switch_length){
            return path.size();
        }
        // for a group with number of consecutive tracks connected with the same COB >= 3, the length of the group is 2
        // because those tracks in the middle are not truely used as signal pathways
        else{
            std::usize head{0}, tail{0};
            std::usize path_length{0};

            if (path.size() <= 2){
                return path.size();
            }
            while(tail != path.size() - 1){
                auto current_pos {track_pos_to_cobs(std::get<0>(path[head]))};
                while (true){
                    auto tail_cobs {track_pos_to_cobs(std::get<0>(path[tail]))};
                    current_pos = shared_cobs(current_pos, tail_cobs);
                    if (current_pos.empty()){
                        path_length += 2;
                        head = tail;
                        break;
                    }
                    else{
                        if (tail == path.size() - 1){
                            break;
                        }
                        tail += 1;
                    }
                }
            }
            if (tail == head){
                path_length += 1;
            }
            else{
                path_length += 2;
            }

            return path_length;
        }
    }

    auto path_length(const std::Vector<hardware::Track*>& path, bool switch_length) -> std::usize {
        if (path.size() == 0) {
            throw FinalError("path_length: path is empty");
        }

        // path length = number of tracks
        if (switch_length){
            return path.size();
        }
        // for a group with number of consecutive tracks connected with the same COB >= 3, the length of the group is 2
        // because those tracks in the middle are not truely used as signal pathways
        else{
            std::usize head{0}, tail{0};
            std::usize path_length{0};

            if (path.size() <= 2){
                return path.size();
            }
            while(tail != path.size() - 1){
                auto current_pos {track_pos_to_cobs(path[head])};
                while (true){
                    auto tail_cobs {track_pos_to_cobs(path[tail])};
                    current_pos = shared_cobs(current_pos, tail_cobs);
                    if (current_pos.empty()){
                        path_length += 2;
                        head = tail;
                        break;
                    }
                    else{
                        if(tail == path.size() - 1){
                            break;
                        }
                        tail += 1;
                    }
                }
            }
            if (tail == head){
                path_length += 1;
            }
            else{
                path_length += 2;
            }

            return path_length;
        }
    }

    // return all cobs connected with track
    auto track_pos_to_cobs(const hardware::Track* track) -> std::Vector<hardware::COBCoord>{
        std::Vector<hardware::COBCoord> cobs {};
        auto coord {track->coord()};
        cobs.emplace_back(coord.row, coord.col);
        if (coord.dir == hardware::TrackDirection::Horizontal){
            if (coord.col >= 1){
                cobs.emplace_back(coord.row, coord.col - 1);
            }
        }
        else if (coord.dir == hardware::TrackDirection::Vertical){
            if (coord.row >= 1){
                cobs.emplace_back(coord.row - 1, coord.col);
            }
        }
        return cobs;
    }
    
    auto shared_cobs(
        const std::Vector<hardware::COBCoord>& cobs1, const std::Vector<hardware::COBCoord>& cobs2
    ) -> std::Vector<hardware::COBCoord>{
        std::Vector<hardware::COBCoord> shared_cobs {};
        for (auto& c1: cobs1){
            for (auto& c2: cobs2){
                if (c1 == c2){
                    shared_cobs.emplace_back(c1);
                }
            }
        }
        return shared_cobs;
    }


}

