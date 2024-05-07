#include <iostream>
#include <string>
#include <optional>

#include "config_mngr.h"


//constructor
config_mngr::config_mngr(std::string config_path) {

    this->config_path = config_path;
}

//read the config
std::optional<std::string> config_mngr::parse_config() {

    //read config file
    try {
        this->cfg.readFile(this->config_path.c_str());
    } catch (const libconfig::FileIOException &f_io_excp) {
        return "[config_mngr::parge_config] libconfig::readFile() : "
               "FileIOException";
    } catch (const libconfig::ParseException &p_excp) { 
        return "[config_mngr::parge_config] libconfig::readFile() : "
               "ParseException";
    }

    //get entries
    try {
        //entity list
        this->offs.entity_list_ptr = this->cfg.lookup("offsets.entity_list_ptr");

        //controller structure
        this->offs.ctrl_name       = this->cfg.lookup("offsets.ctrl_name");
        this->offs.ctrl_pent_index = this->cfg.lookup("offsets.ctrl_pent_index");

        //player_ent structure
        this->offs.pent_hp         = this->cfg.lookup("offsets.pent_hp");
        this->offs.pent_ap         = this->cfg.lookup("offsets.pent_ap");
        this->offs.pent_x          = this->cfg.lookup("offsets.pent_x");
        this->offs.pent_y          = this->cfg.lookup("offsets.pent_y");
        this->offs.pent_z          = this->cfg.lookup("offsets.pent_z");
    } catch (const libconfig::SettingNotFoundException &s_n_f_excp) {
        return "[config_mngr::parge_config] libconfig::lookup() : "
               "SettngNotFoundException";
    }

    return std::nullopt;
}


//get pointer to offsets structure
offsets * config_mngr::get_offsets() {
    return &this->offs;
}
