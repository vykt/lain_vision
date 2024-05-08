#ifndef STATE_H
#define STATE_H

#include <vector>
#include <string>
#include <optional>

#include <cstdint>

#include <libpwu.h>

#include "mem.h"
#include "config.h"


//shared object names
const char * so_client = "libclient.so";


//shared object start addresses
typedef struct {

    //shared objects
    uintptr_t client_so;

} shared_obj_addrs;


//coordinates
typedef struct {

    double x, y, z;

} vertex_3d;


//player_ent health & armour
typedef struct {

    int32_t health;
    int32_t armour;

} vitality;


//controller
typedef struct {

    char name[32];
    short player_ent_index;

} controller;


//player entity
typedef struct {

    vitality vitals;
    
    vertex_3d rotation;
    vertex_3d pos_world;
    vertex_3d pos_view;

    uintptr_t cached_addr;

} player_ent;


//entity list
typedef struct {

    uintptr_t ent_list;

} ent_list;



class state {

    //attributes
    private:
        //state_mngr has sole control over mem_mngr
        mem mem_mngr;

        //global addresses (e.g.: loaded shared object addresses)
        shared_obj_addrs so_addrs;

        //config_mngr produced offsets
        offsets * offs;

        //entity state
        std::vector<controller> controllers;
        std::vector<player_ent> player_ents;

    
    //methods
    private:
        std::optional<std::string> world_to_view_pos();

    public:
        //init & fini
        std::optional<std::string> init(offsets * offs);
        std::optional<std::string> fini();

        //important
        std::optional<std::string> update_core_state();
        std::optional<std::string> update_player_ent_state();

        //getters & setters
        std::vector<controller> * get_controllers();
        std::vector<player_ent> * get_player_ents();
};

#endif
