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

    //view
    uintptr_t view_angles_source_ptr;
    uintptr_t view_angles;

    //controller structure
    uintptr_t ctrl_name;
    uintptr_t ctrl_play_index;

    //player_ent structure
    uintptr_t play_hp_ap;
    uintptr_t play_pos;

} offsets;


//fine-tuning settings
typedef struct {

    //screen resolution
    unsigned int scr_width;
    unsigned int scr_height;

    //radar cheat
    int radar_pos_x;
    int radar_pos_y;
    float radar_diameter;
    float radar_height_diff_limit;
    float radar_blip_size;
    float coords_in_pixel;

} settings;


//read config, return controller
class config {

    //attributes
    private:
        libconfig::Config cfg;
        offsets offs;
        settings sets;
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
