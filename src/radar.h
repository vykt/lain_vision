#ifndef RADAR_H
#define RADAR_H

#include <vector>
#include <string>
#include <optional>

#include "state.h"
#include "draw.h"
#include "config.h"


#define COLOR_SATURATION 0xb0


class radar {

    //attributes
    private:
    settings * sets;
    draw * draw_mngr;
    std::vector<player_ent> * player_ents;

    vertex_2d radar_center;

    //methods
    private:
        //convert view-relative z-pos into a color
        draw_color get_color(float pos_z);

    public:
        //init & fini
        void init(settings * sets, draw * draw_mngr, std::vector<player_ent> * player_ents);

        //draw radar
        void draw_radar();
};

#endif
