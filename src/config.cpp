#include <iostream>
#include <string>
#include <optional>

#include <cstdlib>

#include <unistd.h>
#include <fcntl.h>

#include "config.h"


//config paths
const char * local_conf_path = "/.config/lain_vision/lain_vision.cfg";
const char * system_conf_path = "/etc/lain_vision/lain_vision.cfg";


/*
 *  private
 */

bool config::exists(const char * path) {

    int ret_fd;

    ret_fd = open(path, 0, O_RDONLY);
    if (ret_fd == -1) {
        return false;
    } else {
        close(ret_fd);
        return true;
    }
}


/*
 *  public
 */

//open config, priority given to local
std::optional<std::string> config::init() {

    std::string conf_path, full_local_path;

    //convert tilda path to absolute path
    full_local_path = getenv("HOME");
    full_local_path += local_conf_path; 

    //check config exists
    if(exists(full_local_path.c_str())) {
        conf_path = full_local_path;
    } else if (exists(system_conf_path)) {
        conf_path = system_conf_path;
    } else {
        return "[config::init] std::filesystem::exists() "
               "both local and global configs missing";
    }

    //read config file
    try {
        this->cfg.readFile(conf_path.c_str());
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



/*
 *  Returns of lookup() are cast to signed integers, despite being assigned to unsigned integers
 *  because libconfig++ does not support unsigned integers.
 */

//read the config
std::optional<std::string> config::parse_config() {

    //get offsets
    try {

        //entity list
        this->offs.entity_list_selector_list_ptr 
            = (long int) this->cfg.lookup("offsets.entity_list_selector_list_ptr");

        //view
        this->offs.view_angles_source_ptr
            = (long int) this->cfg.lookup("offsets.view_angles_source_ptr");
        this->offs.view_angles
            = (long int) this->cfg.lookup("offsets.view_angles");

        //controller structure
        this->offs.ctrl_name       = (long int) this->cfg.lookup("offsets.ctrl_name");
        this->offs.ctrl_play_index = (long int) this->cfg.lookup("offsets.ctrl_play_index");

        //player_ent structure
        this->offs.play_hp         = (long int) this->cfg.lookup("offsets.play_hp");
        this->offs.play_pos        = (long int) this->cfg.lookup("offsets.play_pos");
        this->offs.play_ap         = (long int) this->cfg.lookup("offsets.play_ap");
   
    } catch (const libconfig::SettingNotFoundException &s_n_f_excp) {
        return "[config::parge_config] libconfig::lookup() "
               "SettingNotFoundException raised while parsing offsets";
    }

    //get settings
    try {

        //player name
        std::string temp = this->cfg.lookup("settings.lain_name");
        this->sets.lain_name = temp;

        //screen resolution
        this->sets.scr_width = (int) this->cfg.lookup("settings.scr_width");
        this->sets.scr_height = (int) this->cfg.lookup("settings.scr_height");
        
        //radar mod
        this->sets.radar_pos_x = this->cfg.lookup("settings.radar_pos_x");
        this->sets.radar_pos_y = this->cfg.lookup("settings.radar_pos_y");
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
