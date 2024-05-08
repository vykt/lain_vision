#include <vector>
#include <string>
#include <optional>

#include <unistd.h>

#include <libpwu.h>

#include "state.h"
#include "mem.h"
#include "config.h"


/*
 *  private
 */

//convert world position of each player_ent into a view-relative position
std::optional<std::string> state::world_to_view_pos() {

    /*
     *  1) use lain player_ent's rotation and world_pos to get view_pos for all
     *     other ents. if lain, set view_pos to {0,0,0}
     */
}


/*
 *  public
 */

//initialiser
std::optional<std::string> state::init(offsets * offs) {

    /*
     *  1) assign offsets ptr
     *
     *  2) init mem_mngr
     *
     *  3) fill shared obj start addr cache (ask mem_mngr to do it)
     */
}

//finisher
std::optional<std::string> state::fini() {

    /*
     *  1) fini mem_mngr
     */
}


//re-read entity list ptr & re-check each controller in entity list
std::optional<std::string> state::update_core_state() {

    /*
     *  1) re-populate ent_list
     *
     *  2) re-populate controllers
     *
     *  3) re-populate player_ents.cached_addr using controllers.player_ent_index
     */
}


//update each player entity's health, armour, and world & view positions
std::optional<std::string> state::update_player_ent_state() {

    /*
     *  1) fill player_ents.vitals
     *
     *  2) fill player_ent.pos_world
     *
     *  3) call world_to_view_state() to convert coordinates for each player_ent
     */
}


//get ptr to controllers vector
std::vector<controller> * state::get_controllers() {
    return &this->controllers;
}

//get ptr to player_ent vector
std::vector<player_ent> * state::get_player_ents() {
    return &this->player_ents;
}
