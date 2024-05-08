#include <vector>
#include <string>
#include <optional>

#include <cmath>

#include "radar_model.h"


//convert from world coordinates to model-relative coordinates
inline void radar_model::world_to_model(std::vector<vertex_2d> * new_w_players) {

    vertex_2d temp;

    this->m_players.clear();

    for (unsigned long i = 0; i < new_w_players->size(); ++i) {
        temp.x = (*new_w_players)[i].x - this->m_origin.x;
        temp.y = (*new_w_players)[i].y - this->m_origin.y;
        this->m_players.push_back(temp);
    }
}


inline void radar_model::init_r_matrix() {

    this->r_matrix.v[0].x = this->r_matrix.v[1].y = cos(this->theta);
    this->r_matrix.v[1].x = sin(this->theta);
    this->r_matrix.v[0].y = -1.0f * this->r_matrix.v[1].x;
}


//rotate each player's coordinates by theta (angle i am facing)
inline void radar_model::rotate_m_players() {

    vertex_2d temp;
    this->r_players.clear();

    for (unsigned long i = 0; i < this->m_players.size(); ++i) {

        temp.x = (this->m_players[i].x * this->r_matrix.v[0].x) + (this->m_players[i].y * this->r_matrix.v[0].y);
        temp.y = (this->m_players[i].y * this->r_matrix.v[1].x) + (this->m_players[i].x * this->r_matrix.v[1].y);

        this->r_players.push_back(temp);
    }
}


radar_model::radar_model(double max_x, double max_y) {

    this->max_x = max_x;
    this->max_y = max_y;

}


std::optional<std::string> radar_model::update(vertex_2d new_m_origin, std::vector<vertex_2d> new_w_players, double theta) {

    //setup model coords
    this->m_origin = new_m_origin;
    this->world_to_model(&new_w_players);

    //setup rotation
    this->theta = theta;
    this->init_r_matrix();

    //rotate players
    this->rotate_m_players();

    return std::nullopt;
}
