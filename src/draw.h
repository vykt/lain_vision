#ifndef DRAW_H
#define DRAW_H

#include <string>
#include <optional>

#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/extensions/shape.h>
#include <X11/extensions/Xfixes.h>
#include <X11/extensions/Xcomposite.h>

#include <SDL2/SDL.h>

#include "state.h"


//color to draw
typedef SDL_Color draw_color;


class draw {

    //attributes
    private:
        //X11
        Display * x11_display;
        Window overlay_win;
        Colormap colormap;
        
        //SDL2
        SDL_Window * sdl_window;
        SDL_Renderer * sdl_renderer;

    //methods
    private:
        //internal init & fini
        std::optional<std::string> init_x11(unsigned int scr_width, unsigned int scr_height);
        void fini_x11();
        std::optional<std::string> init_sdl();
        void fini_sdl();

        //helper funcs
        std::string get_sdl_err(const char * err_msg);
    
    public:
        //init & fini
        std::optional<std::string> init(settings * sets);
        void fini();

        //update display & clear draw buffer
        void update();

        //draw requests
        void draw_blip(int pos_x, int pos_y, int size, draw_color * color);

};

#endif
