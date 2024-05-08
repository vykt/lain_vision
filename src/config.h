#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <optional>

#include <cstdint>

#include <libconfig.h++>


//offset in cs2
typedef struct {

    //global structures
    uintptr_t entity_list_ptr;

    //controller structure
    uintptr_t ctrl_name;
    uintptr_t ctrl_pent_index;

    //player_ent structure
    uintptr_t pent_hp;
    uintptr_t pent_ap;
    uintptr_t pent_x;
    uintptr_t pent_y;
    uintptr_t pent_z;

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
