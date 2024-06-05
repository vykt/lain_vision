#include <string>
#include <optional>
#include <iostream>
#include <chrono>

#include <cstdlib>

#include <unistd.h>

#include "radar.h"
#include "state.h"
#include "draw.h"
#include "config.h"



//error reporting macros
#define RESET  "\033[0m"
#define RED    "\033[31m"
#define YELLOW "\033[33m"

//how many times to re-read cached data before re-reading all state
#define CACHE_READS_MAX 1000


void error_terminate(std::optional<std::string> err) {
    std::cout << RED << "Fatal error:\n" << RESET << err.value() << std::endl;
    exit(-1);
}


int main() {

    std::optional<std::string> ret;
    int refollow_count;


    //core components
    config conf_parser;
    state state_mngr;
    draw draw_mngr;
    
    //mod components
    radar radar_mod;

    
    /*
     *  setup
     */

    //read config
    ret = conf_parser.init();
    if (ret) error_terminate(ret);
    ret = conf_parser.parse_config();
    if (ret) error_terminate(ret);

    //init state manager
    ret = state_mngr.init(conf_parser.get_settings(), conf_parser.get_offsets());
    if (ret) error_terminate(ret);

    //init draw manager
    ret = draw_mngr.init(conf_parser.get_settings());
    if (ret) error_terminate(ret);

    //init radar mod
    radar_mod.init(conf_parser.get_settings(), &draw_mngr, state_mngr.get_player_ents());


    /*
     *  main loop
     */

    //bootstrap main loop
    refollow_count = 0;

    //main loop
    while (1) {
        
        //start timer
        //auto loop_start_time = std::chrono::high_resolution_clock::now();

        //update core state
        if (refollow_count == 0) {
            ret = state_mngr.update_core_state();
            if (ret) error_terminate(ret);
            refollow_count = CACHE_READS_MAX;
        }

        //updated cached state
        ret = state_mngr.update_cached_state();
        if (ret) error_terminate(ret);
        refollow_count--;

        //draw radar
        radar_mod.draw_radar();

        //update display
        draw_mngr.update();

        //end timer
        //auto loop_end_time = std::chrono::high_resolution_clock::now();

        //limit to 60fps
        //while (std::chrono::duration_cast<std::chrono::milliseconds>(loop_end_time - loop_start_time).count() < 17) {
        //    sleep(10000); //10ms
        //    loop_end_time = std::chrono::high_resolution_clock::now();
        //}
    } //end while
    

    //TODO TODO TODO
    // 
    // find some way to break the loop that isn't SIGTERM
    //
    //TODO TODO TODO
    
    /*
     *  clean up
     */

    //terminate draw manager
    draw_mngr.fini();

    //terminate state manager
    ret = state_mngr.fini();
    if (ret) error_terminate(ret);

    //terminate config parser
    conf_parser.fini();

    std::cout << YELLOW << "Goodbye, NaVi :)" << RESET << std::endl;
    return 0;
}
