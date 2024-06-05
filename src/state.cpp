#include <vector>
#include <string>
#include <optional>

#include <cstdint>
#include <cstring>
#include <cmath>

#include <unistd.h>

#include <libpwu.h>

#include "state.h"
#include "mem.h"
#include "config.h"


//shared object names
const char * so_client = "libclient.so";

/*
 *  private
 */

//traverse entity list and fetch address of index entity
std::optional<std::string> state::traverse_ent_list(uint32_t index, 
                                                    uintptr_t * ent_addr) {

    /*
     *  this has been manually decompiled back into C :)
     */

    std::optional<std::string> ret;

    uint16_t short_index;
    uint32_t offset_index, temp_index;
    uintptr_t ent_list_ptr, temp_addr;

    
    //get short version of index for entity list selector calculation
    short_index = (uint16_t) (index & 0x7fff);
    
    //get entity list address
    temp_addr = this->e_list.selector_list_addr + 0x10 + (short_index >> 0x9)*0x8;
    ret = this->mem_mngr.read_addr(temp_addr, &ent_list_ptr);
    if (ret) return ret.value() + "\n from: [state::traverse_ent_list]";
    if (ent_list_ptr == 0)
        return "[state::traverse_ent_list] mem_mngr::read_addr() "
                                  "ent_list_ptr read NULL";

    //get offset into entity list
    offset_index = (uint32_t) (short_index & 0x1ff) * 0x78;
    
    //check if this is a valid entity(???)
    temp_addr = ent_list_ptr + offset_index + 0x10;
    ret = this->mem_mngr.read_addr(temp_addr, &temp_index);
    if (ret) return ret.value() + "\n from: [state::traverse_ent_list]";
    if ((uint16_t) (temp_index & 0x7fff) != short_index) {
        //if the (???) check doesn't pass, return null address
        *ent_addr = 0;
        return std::nullopt;
    }

    //get entity address
    temp_addr = ent_list_ptr + offset_index;
    ret = this->mem_mngr.read_addr(temp_addr, ent_addr);
    if (ret) return ret.value() + "\n from: [state::traverse_ent_list]";

    return std::nullopt;
}


//traverse view structures source to cache addrs of view struct members
inline std::optional<std::string> state::traverse_view_structs() {

    std::optional<std::string> ret;

    uintptr_t view_angle_base_ptr, temp_addr;

    //fetch base address of view angle structure
    temp_addr = this->so_addrs.client_so + this->offs->view_angles_source_ptr;
    ret = this->mem_mngr.read_addr(temp_addr, &view_angle_base_ptr);
    if (ret) return ret.value() + "\n from: [state:traverse_view_source]";

    //set pointer to view angles
    this->view.cached_addr = view_angle_base_ptr + this->offs->view_angles;

    return std::nullopt;
}


//read controller_ent values from remote process into local state
std::optional<std::string> state::read_remote_c_ent(uintptr_t c_ent_addr,
                                                    controller_ent * temp_c_ent_ptr) {

    std::optional<std::string> ret;
    uintptr_t temp_addr;

    //read controller name
    temp_addr = c_ent_addr + this->offs->ctrl_name;
    ret = this->mem_mngr.read_array_addr(temp_addr, 
                                         temp_c_ent_ptr->name, CONTROLLER_NAME_LEN);
    if (ret) return ret.value() + "\n from: [state::read_remote_c_ent]";

    //read player index for this controller
    temp_addr = c_ent_addr + this->offs->ctrl_play_index;
    ret = this->mem_mngr.read_addr(temp_addr, &temp_c_ent_ptr->player_ent_index);
    if (ret) return ret.value() + "\n from: [state::read_remote_c_ent]";

    return std::nullopt;
}


//read player_ent values from remote process into local store
std::optional<std::string> state::read_remote_p_ent(player_ent * temp_p_ent_ptr) {

    std::optional<std::string> ret;
    uintptr_t temp_addr;


    //minimising reads just a little here

    //read player hp
    temp_addr = temp_p_ent_ptr->cached_addr + this->offs->play_hp;
    ret = this->mem_mngr.read_addr(temp_addr, (uint32_t *) &temp_p_ent_ptr->vitals.health);
    if (ret) return ret.value() + "\n from: [state::read_remote_p_ent]";

    //read player position
    temp_addr = temp_p_ent_ptr->cached_addr + this->offs->play_pos;
    ret = this->mem_mngr.read_array_addr(temp_addr, (byte *) &temp_p_ent_ptr->pos_world, sizeof(temp_p_ent_ptr->pos_world));
    if (ret) return ret.value() + "\n from: [state::read_remote_p_ent]";

    //read player ap
    temp_addr = temp_p_ent_ptr->cached_addr + this->offs->play_ap;
    ret = this->mem_mngr.read_addr(temp_addr, (uint32_t *) &temp_p_ent_ptr->vitals.armour);
    if (ret) return ret.value() + "\n from: [state::read_remote_p_ent]";

    return std::nullopt;
}


//read view_angles from remote process into local store
inline std::optional<std::string> state::update_view_data() {

    std::optional<std::string> ret;

    //read remote view angles
    ret = this->mem_mngr.read_array_addr(this->view.cached_addr, (byte *) &this->view.view_angles, sizeof(this->view.view_angles));
    if (ret) return ret.value() + "\n from: [state::update_player_ent_state]";

    //get theta
    if (this->view.view_angles.y < 0.0f) {
        this->view.theta = 360.0f + this->view.view_angles.y;
    } else {
        this->view.theta = this->view.view_angles.y;
    }

    //invert theta to make the rotations correct (invert = clockwise)
    this->view.theta = this->view.theta * -1.0f;

    //convert theta to radians for cmath
    this->view.theta = this->view.theta * M_PI / 180.0f;

    //calculate rotation vertex using new theta
    this->view.sin_theta = sin(this->view.theta);
    this->view.cos_theta = cos(this->view.theta);

    return std::nullopt;
}


//convert world position of each player_ent into a view-relative position, then rotate with rotation vertex
std::optional<std::string> state::world_to_view_pos(player_ent * player_ent_ref,
                                                    player_ent * lain_ent_ref) {

    float x_dash, y_dash;

    //check if lain
    if (player_ent_ref == lain_ent_ref) {
        player_ent_ref->pos_view.x = 0;
        player_ent_ref->pos_view.y = 0;
        player_ent_ref->pos_view.z = 0;
        return std::nullopt;
    }

    //convert world position to view-relative position (unrotated)
    player_ent_ref->pos_view.x = player_ent_ref->pos_world.x - lain_ent_ref->pos_world.x;
    player_ent_ref->pos_view.y = player_ent_ref->pos_world.y - lain_ent_ref->pos_world.y;
    player_ent_ref->pos_view.z = player_ent_ref->pos_world.z - lain_ent_ref->pos_world.z;

    //apply rotation vertex to view-relative coordinates
    x_dash =   player_ent_ref->pos_view.x * this->view.cos_theta 
             - player_ent_ref->pos_view.y * this->view.sin_theta;

    y_dash =   player_ent_ref->pos_view.y * this->view.cos_theta 
             + player_ent_ref->pos_view.x * this->view.sin_theta;         

    player_ent_ref->pos_view.x = x_dash;
    player_ent_ref->pos_view.y = y_dash;

    return std::nullopt;
}


/*
 *  public
 */

//initialiser
std::optional<std::string> state::init(settings * sets, offsets * offs) {

    std::optional<std::string> ret;

    //store reference to config settings and offsets
    this->sets = sets;
    this->offs = offs;

    //start memory manager
    this->mem_mngr.init();

    //get shared object addresses
    ret = this->mem_mngr.get_backing_file_addr(so_client, &this->so_addrs.client_so);
    if (ret) return ret.value() + "\n from: [state::init]";

    return std::nullopt;
}


//finisher
std::optional<std::string> state::fini() {

    std::optional<std::string> ret;
    
    //end memory manager
    ret = this->mem_mngr.fini();
    if (ret) return ret.value() + "\n from: [state::fini]";

    return std::nullopt;
}


//re-read entity list ptr & re-check each controller in entity list
std::optional<std::string> state::update_core_state() {

    /*
     *  1) re-populate entity list selector list address
     *
     *  2) re-populate controllers
     *
     *  3) re-populate player_ents.cached_addr using controllers.player_ent_index
     */

    std::optional<std::string> ret;

    uintptr_t temp_addr;
    controller_ent temp_controller_ent;
    player_ent temp_player_ent;


    //re-cache view related pointers
    ret = traverse_view_structs();
    if (ret) return ret.value() + "\n from: [state::update_core_state]";


    //get address of entity list selector list
    temp_addr = this->so_addrs.client_so + this->offs->entity_list_selector_list_ptr;
    ret = this->mem_mngr.read_addr(temp_addr, &this->e_list.selector_list_addr);
    if (ret) return ret.value() + "\n from [state::update_core_state]";

    //clear controller & player vectors
    this->controller_ents.clear();
    this->player_ents.clear(); 
    memset(&temp_player_ent, 0, sizeof(temp_player_ent));


    //for every possible controller
    for (uint32_t i = 1; i < 64; ++i) {

        //traverse this i index
        ret = traverse_ent_list(i, &temp_addr);
        if (ret) continue; //return ret.value() + "\n from: [state::update_core_state]";

        //if invalid entity, continue
        if (temp_addr == 0) continue;

        //read controller_ent values
        ret = read_remote_c_ent(temp_addr, &temp_controller_ent);
        if (ret) return ret.value() + "\n from: [state::update_core_state]";
        temp_controller_ent.player_ent_local_index = i;

        //use controller_ent's player index to get the player entity address
        ret = traverse_ent_list(temp_controller_ent.player_ent_index, &temp_addr);
        if (ret) continue; //return ret.value() + "\n from: [state::update_core_state]";

        //cache player_ent's address inside its own struct
        temp_player_ent.cached_addr = temp_addr;

        //add controller and player entities to their respective vectors
        if (!strcmp(temp_controller_ent.name, this->sets->lain_name.c_str())) {
            this->controller_ents.insert(this->controller_ents.begin(), temp_controller_ent);
            this->player_ents.insert(this->player_ents.begin(), temp_player_ent);
        } else {
            this->controller_ents.push_back(temp_controller_ent);
            this->player_ents.push_back(temp_player_ent);
        }

    } //end for

    return std::nullopt;
}


//update each player entity's health, armour, and world & view positions
std::optional<std::string> state::update_cached_state() {

    /*
     *  0) read view angles
     *
     *  0.5) calculate theta & rotation_vertex
     *
     *  1) fill player_ents.vitals
     *
     *  2) fill player_ent.pos_world
     *
     *  3) call world_to_view_state() to convert coordinates for each player_ent
     */

    std::optional<std::string> ret;

    //update view data

    //for every controller, update corresponding player entity
    for (unsigned long i = 0; i < this->player_ents.size(); ++i) {

        ret = read_remote_p_ent(&this->player_ents[i]);
        if (ret) return ret.value() + "\n from: [state::update_player_ent_state]";

        //if lain update theta & rotation_vertex for use with view-relative conversions
        if (i == 0) {
            ret = update_view_data();
            if (ret) return ret.value() + "\n from: [state::update_player_ent_state]";
        }

        ret = world_to_view_pos(&this->player_ents[i], &this->player_ents[0]);
        if (ret) return ret.value() + "\n from: [state::update_player_ent_state]";

    } //end for

    return std::nullopt;
}


//get ptr to controllers vector
std::vector<controller_ent> * state::get_controller_ents() {
    return &this->controller_ents;
}


//get ptr to player_ent vector
std::vector<player_ent> * state::get_player_ents() {
    return &this->player_ents;
}
