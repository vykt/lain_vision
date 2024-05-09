#include <iostream>
#include <string>
#include <optional>

#include "config.h"


//constructor
config::config(std::string config_path) {

    this->config_path = config_path;
}

//read the config
std::optional<std::string> config::parse_config() {

    //read config file
    try {
        this->cfg.readFile(this->config_path.c_str());
    } catch (const libconfig::FileIOException &f_io_excp) {
        return "[config::parge_config] libconfig::readFile() "
               "FileIOException";
    } catch (const libconfig::ParseException &p_excp) { 
        return "[config::parge_config] libconfig::readFile() "
               "ParseException";
    }

    //get entries
    try {
        //entity list
        this->offs.entity_list_selector_ptr 
            = this->cfg.lookup("offsets.entity_list_selector_list_ptr");

        //controller structure
        this->offs.ctrl_name       = this->cfg.lookup("offsets.ctrl_name");
        this->offs.ctrl_play_index = this->cfg.lookup("offsets.ctrl_play_index");

        //player_ent structure
        this->offs.play_hp_ap      = this->cfg.lookup("offsets.play_hp_ap");
        this->offs.play_pos        = this->cfg.lookup("offsets.play_pos");
        this->offs.play_rotation     = this->cfg.lookup("offsets.play_rotation");
    
    } catch (const libconfig::SettingNotFoundException &s_n_f_excp) {
        return "[config::parge_config] libconfig::lookup() "
               "SettngNotFoundException";
    }

    return std::nullopt;
}


//get pointer to offsets structure
offsets * config::get_offsets() {
    return &this->offs;
}
