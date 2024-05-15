#include <vector>
#include <string>
#include <optional>
#include <algorithm>

#include <cmath>

#include "radar.h"
#include "draw.h"
#include "config.h"


/*
 *  private
 */

//get a color that represents the elevation of a given player compared to lain, on the radar
draw_color radar::get_color(float pos_z) {
    
    /*
     *  start with a red color, change color to be more purple if pos_z is positive, more yellow if pos_z is negative
     */

    draw_color color;
    float pos_z_clamped;
    float color_per_coord;
    float diff_z_lower_bound, diff_z_higher_bound;

    //get bounds, if player is above the bound, the max color value is assigned
    diff_z_lower_bound = -1.0f * this->sets->radar_height_diff_limit;
    diff_z_higher_bound = this->sets->radar_height_diff_limit;

    //calculate change in color per coordinate difference
    color_per_coord = this->sets->radar_height_diff_limit / COLOR_SATURATION;

    //setup default color
    color.r = COLOR_SATURATION;
    color.g = 0x00;
    color.b = 0x00;
    color.a = 0xff;
    
    //shift color to be more purple / yellow depending on clamped pos_z difference
    pos_z_clamped = std::clamp(pos_z, diff_z_lower_bound, diff_z_higher_bound);
    
    if (pos_z_clamped >= 0.0f) {
        color.b = (int) (pos_z_clamped * color_per_coord);
    } else {
        color.g = (int) (pos_z_clamped * color_per_coord * -1.0f);
    }

    return color;
}


//convert view-relative coordinates to screen position | return false if shouldn't draw
bool radar::pos_to_screen(float pos_x, float pos_y, int * scr_x, int * scr_y) {

    float total_distance;

    //check if the position is too far & shouldn't be drawn
    total_distance = sqrt((pos_x * pos_x) + (pos_y * pos_y));
    if (total_distance > this->sets->radar_view_distance_limit) return false;

    //get screen coordiantes
    *scr_x = (int) ((pos_x / this->sets->radar_coords_in_pixel) + this->sets->radar_pos_x);
    *scr_y = (int) ((pos_y / this->sets->radar_coords_in_pixel) + this->sets->radar_pos_y);

    return true;
}


/*
 *  public
 */

void radar::init(settings * sets, draw * draw_mngr, std::vector<player_ent> * player_ents) {

    //setup references to settings & player entities
    this->sets = sets;
    this->draw_mngr = draw_mngr;
    this->player_ents = player_ents;
    
    //calc radar center
    this->radar_center.x = (this->sets->radar_diameter / 2.0f) + this->sets->radar_pos_x;
    this->radar_center.y = (this->sets->radar_diameter / 2.0f) + this->sets->radar_pos_y;
}


//draw radar contents to screen
void radar::draw_radar() {

    player_ent * p_ent_ptr;
    draw_color color;

    int scr_x, scr_y;

    //for every single player entity
    for (unsigned long i = 0; i < this->player_ents->size(); ++i) {

        //get next player entity
        p_ent_ptr = &(*this->player_ents)[i];

        //get screen coordinates for this player entity, and check if it should be drawn
        if(!pos_to_screen(p_ent_ptr->pos_view.x, p_ent_ptr->pos_view.y, &scr_x, &scr_y)) continue;

        //calculate color for this entity's blip
        color = get_color(p_ent_ptr->pos_view.z);

        //ask draw manager to draw radar blip
        this->draw_mngr->draw_blip(scr_x, scr_y, this->sets->radar_blip_size, &color);
        
    } //end for
}
