#ifndef STATE_H
#define STATE_H

#include <vector>
#include <string>
#include <optional>

#include <cstdint>

#include <libpwu.h>

#include "mem.h"
#include "config.h"


//shared object start addresses
typedef struct {

    //shared objects
    uintptr_t client_so;

} shared_obj_addrs;


//2d vector my more beloved
typedef struct {

    float x, y;

} vertex_2d;


//3d vector my beloved
typedef struct {

    float x, y, z;

} vertex_3d;


//player_ent health & armour
typedef struct {

    int32_t health;
    int32_t armour;

} vitality;


//controller entity
typedef struct {

    char name[CONTROLLER_NAME_LEN];
    uint32_t player_ent_index;  //inside remote entity list
    int player_ent_local_index; //inside local player_ents vector

} controller_ent;


//player entity
typedef struct {

    vitality vitals;

    vertex_3d pos_world;
    vertex_3d pos_view;

    uintptr_t cached_addr;

} player_ent;


//entity list
typedef struct {

    uintptr_t selector_list_addr; //aka 'base'

} ent_list;


//view related
typedef struct {

    vertex_3d view_angles;
    float theta;
    float sin_theta, cos_theta;

    uintptr_t cached_addr;
} view_data;


class state {

    //attributes
    private:
        //state_mngr has sole control over mem_mngr
        mem mem_mngr;

        //global addresses (e.g.: loaded shared object addresses)
        shared_obj_addrs so_addrs;

        //view related
        view_data view;

        //config_mngr produced offsets
        settings * sets;
        offsets * offs;

        //entity state
        ent_list e_list;
        std::vector<controller_ent> controller_ents;
        std::vector<player_ent> player_ents;
    
    //methods
    private:
        std::optional<std::string> traverse_ent_list(uint32_t index, 
                                                     uintptr_t * ent_addr);
        std::optional<std::string> traverse_view_structs();
        std::optional<std::string> read_remote_c_ent(uintptr_t c_ent_addr,
                                                     controller_ent * temp_c_ent_ptr);
        std::optional<std::string> read_remote_p_ent(player_ent * temp_p_ent_ptr);
        std::optional<std::string> update_view_data();
        std::optional<std::string> world_to_view_pos(player_ent * player_ent_ref,
                                                     player_ent * lain_ent_ref);

    public:
        //init & fini
        std::optional<std::string> init(settings * sets, offsets * offs);
        std::optional<std::string> fini();

        //important
        std::optional<std::string> update_core_state();
        std::optional<std::string> update_cached_state();

        //getters & setters
        std::vector<controller_ent> * get_controller_ents();
        std::vector<player_ent> * get_player_ents();
};

#endif
