#include <iostream>
#include <string>
#include <optional>
#include <filesystem>

#include "config.h"


//config paths
const char * local_conf_path = "~/.config/lain_vision/lain_vision.cfg";
const char * system_conf_path = "/etc/lain_vision/lain_vision.cfg";



//open config, priority given to local
std::optional<std::string> config::init() {

    char * conf_path;

    //check config exists
    if(std::filesystem::exists(local_conf_path)) {
        conf_path = (char *) local_conf_path;
    } else if (std::filesystem::exists(system_conf_path)) {
        conf_path = (char *) system_conf_path;
    } else {
        return "[config::init] std::filesystem::exists() "
               "both local and global configs missing";
    }

    //read config file
    try {
        this->cfg.readFile(conf_path);
    } catch (const libconfig::FileIOException &f_io_excp) {
        return "[config::parge_config] libconfig::readFile() "
               "FileIOException";
    } catch (const libconfig::ParseException &p_excp) { 
        return "[config::parge_config] libconfig::readFile() "
               "ParseException";
    }

    return std::nullopt;
}


//nothing to do!
void config::fini() {

}


//read the config
std::optional<std::string> config::parse_config() {

    //get offsets
    try {
        //entity list
        this->offs.entity_list_selector_list_ptr 
            = this->cfg.lookup("offsets.entity_list_selector_list_ptr");

        //view
        this->offs.view_angles_source_ptr
            = this->cfg.lookup("offsets.view_angles_source_ptr");
        this->offs.view_angles = this->cfg.lookup("offsets.view_angles");

        //controller structure
        this->offs.ctrl_name       = this->cfg.lookup("offsets.ctrl_name");
        this->offs.ctrl_play_index = this->cfg.lookup("offsets.ctrl_play_index");

        //player_ent structure
        this->offs.play_hp_ap      = this->cfg.lookup("offsets.play_hp_ap");
        this->offs.play_pos        = this->cfg.lookup("offsets.play_pos");
    
    } catch (const libconfig::SettingNotFoundException &s_n_f_excp) {
        return "[config::parge_config] libconfig::lookup() "
               "SettingNotFoundException raised while parsing offsets";
    }

    //get settings
    try {
        //screen resolution
        this->sets.scr_width = this->cfg.lookup("settings.scr_width");
        this->sets.scr_height = this->cfg.lookup("settings.scr_height");
        
        //radar mod
        this->sets.radar_pos_x = this->cfg.lookup("settings.radar_pos_x");
        this->sets.radar_pos_x = this->cfg.lookup("settings.radar_pos_x");
        this->sets.radar_diameter = this->cfg.lookup("settings.radar_diameter");
        this->sets.radar_view_distance_limit = this->cfg.lookup("settings.radar_view_distance_limit");
        this->sets.radar_height_diff_limit = this->cfg.lookup("settings.radar_height_diff_limit");
        this->sets.radar_blip_size = this->cfg.lookup("settings.radar_blip_size");
        this->sets.radar_coords_in_pixel = this->cfg.lookup("settings.radar_coords_in_pixel");

    } catch (const libconfig::SettingNotFoundException &s_n_f_excp) {
        return "[config::parse_config] libconfig::lookup() "
               "SettingNotFoundException raised while parsing settings";
    }

    return std::nullopt;
}


//get pointer to offsets structure
offsets * config::get_offsets() {
    return &this->offs;
}


//get pointer to settings structure
settings * config::get_settings() {
    return &this->sets;
}
