#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <optional>

#include <cstdint>

#include <libconfig.h++>


#define CONTROLLER_NAME_LEN 32


//offset in cs2
typedef struct {

    //global structures
    uintptr_t entity_list_selector_list_ptr;

    //controller structure
    uintptr_t ctrl_name;
    uintptr_t ctrl_play_index;

    //player_ent structure
    uintptr_t play_hp_ap;
    uintptr_t play_pos;
    uintptr_t play_rotation;

} offsets;


//read config, return controller
class config {

    //attributes
    private:
        libconfig::Config cfg;
        offsets offs;
        std::string config_path;

    //methods
    public:
        //important
        config(std::string config_path);
        std::optional<std::string> parse_config(); 

        //getters & setters
        offsets * get_offsets();
};


#endif
