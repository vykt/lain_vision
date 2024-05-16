#include <string>
#include <optional>

#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/extensions/shape.h>
#include <X11/extensions/Xfixes.h>
#include <X11/extensions/Xcomposite.h>

#include <SDL2/SDL.h>

#include "draw.h"
#include "config.h"


/*
 *  private
 */

//start X11 connection
std::optional<std::string> draw::init_x11(unsigned int scr_width, unsigned int scr_height) {

    // --- DATA

    //screen vars
    int screen_index;
    unsigned long color_black;
    Window root_win;
    XSetWindowAttributes overlay_attr;

    //visual vars
    int num_visual_infos;
    XVisualInfo visual_template, * visual_info;
    Visual * visual;

    //colourmap vars
    XColor draw_color;

    //overlay on top & input passthrough vars
    Atom wm_state_above;
    XserverRegion region;


    // --- TEXT

    //open X11 display
    this->x11_display = XOpenDisplay(nullptr);
    if (this->x11_display == nullptr) return "[draw::init_x11] XOpenDisplay() "
                                             "x11_display is null";
    //get default X11 screen
    screen_index = DefaultScreen(x11_display);

    //setup some default values
    color_black = BlackPixel(x11_display, screen_index);

    //fetch the root window for current screen
    root_win = RootWindow(x11_display, screen_index);

    //define a visual_info with support for transparency
    visual_template.screen = screen_index;
    visual_template.depth = 32;
    visual_template.c_class = TrueColor;
    visual_info = XGetVisualInfo(x11_display, 
                                 VisualScreenMask | VisualDepthMask | VisualClassMask, &visual_template, &num_visual_infos);
    if (visual_info == nullptr) return "[draw::init_x11] XGetVisualInfo() "
                                       "visual_info is null";

    //setup a visual for use with window creation
    visual = nullptr;
    for (int i = 0; i < num_visual_infos; ++i) {
        if (   (visual_info[i].red_mask   == 0xff0000) 
            && (visual_info[i].green_mask == 0x00ff00) 
            && (visual_info[i].blue_mask  == 0x0000ff)) {
                visual = visual_info[i].visual;
                break;
        }
    }
    if (visual == nullptr) return "[draw::init_x11] " 
                                  "no visual_info that satisfies transarency requirements";
    XFree(visual_info);

    //setup colourmap for graphics context for new windows
    this->colormap = XCreateColormap(this->x11_display, root_win, visual, AllocNone);
    draw_color.red = 25500;
    draw_color.green = 25500;
    draw_color.blue = 25500;
    draw_color.flags = DoRed | DoGreen | DoBlue;
    XAllocColor(this->x11_display, this->colormap, &draw_color);

    //setup window attributes
    overlay_attr.border_pixel = color_black;
    overlay_attr.background_pixel = color_black;
    overlay_attr.override_redirect = True;
    overlay_attr.colormap = this->colormap;
    overlay_attr.event_mask = ExposureMask;

    //create overlay window
    this->overlay_win = XCreateWindow(this->x11_display, root_win, 0, 0, scr_width, scr_height, 0, 32, InputOutput, visual,
                                      CWBorderPixel | CWBackPixel | CWEventMask | CWOverrideRedirect | CWColormap, &overlay_attr);
    
    //place overlay window on top & enable input passthrough
    wm_state_above = XInternAtom(x11_display, "_NET_WM_STATE_ABOVE", 1);
    XChangeProperty(this->x11_display, this->overlay_win, XInternAtom(this->x11_display, "_NET_WM_STATE", 1), 
                    XA_ATOM, 32, PropModeReplace, (unsigned char *) &wm_state_above, 1);
    region = XFixesCreateRegion(this->x11_display, nullptr, 0);
    XFixesSetWindowShapeRegion(this->x11_display, this->overlay_win, ShapeInput, 0, 0, region);
    XFixesDestroyRegion(this->x11_display, region);

    //map window
    XMapWindow(this->x11_display, this->overlay_win);

    return std::nullopt;
}


//terminate X11 connection & clean up
void draw::fini_x11() {

    XFreeColormap(this->x11_display, this->colormap);
    XDestroyWindow(this->x11_display, this->overlay_win);
    XCloseDisplay(this->x11_display);
}


//initialise SDL2 within the context of an X11 window overlay (call _only_ after init_x11())
std::optional<std::string> draw::init_sdl() {

    int ret;

    //init SDL2
    ret = SDL_Init(SDL_INIT_VIDEO);
    if (ret) return get_sdl_err("[draw::init_sdl] SQL_Init() returned non-zero: ");

    //attach SDL2 to overlay
    this->sdl_window = SDL_CreateWindowFrom((void *) this->overlay_win);
    if (this->sdl_window == nullptr) return get_sdl_err("[draw::init_sdl] SDL_CreateWindowFrom() sdl_window is null: ");

    //create an SDL2 renderer for the SDL2 window
    this->sdl_renderer = SDL_CreateRenderer(this->sdl_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (this->sdl_renderer == nullptr) return get_sdl_err("[draw::init_sdl] SDL_CreateRenderer() sdl_renderer is null: ");

    //enable transparency (blending) in the SDL2 renderer
    ret = SDL_SetRenderDrawBlendMode(this->sdl_renderer, SDL_BLENDMODE_BLEND);
    if (ret) return get_sdl_err("[draw::init_sdl] SDL_SetRenderDrawBlendMode() returned non-zero: ");


    return std::nullopt;
}


//terminate SDL2 session & clean up
void draw::fini_sdl() {

    SDL_DestroyRenderer(this->sdl_renderer);
    SDL_DestroyWindow(this->sdl_window);
    SDL_Quit();
}


//get error message for SDL2 to propagate up the stack
std::string draw::get_sdl_err(const char * err_msg) {

    std::string msg(err_msg);
    return msg += SDL_GetError();
}


/*
 *  public
 */

//initialise in correct order in correct order
std::optional<std::string> draw::init(settings * sets) {

    std::optional<std::string> ret;

    //initialise X11
    ret = init_x11(sets->scr_width, sets->scr_height);
    if (ret) return ret.value() + "\n from: [draw::init]";

    //initialise SDL2 & attach to X11 overlay
    ret = init_sdl();
    if (ret) return ret.value() + "\n from: [draw::init]";

    return std::nullopt;
}


//finish in correct order
void draw::fini() {
    
    fini_sdl();
    fini_x11();
}


//update display & clear draw buffer
void draw::update() {

    //update display & flush
    SDL_RenderPresent(this->sdl_renderer);
    XFlush(this->x11_display);

    //reset draw buffer
    SDL_SetRenderDrawColor(this->sdl_renderer, 0, 0, 0, 0);
    SDL_RenderClear(this->sdl_renderer);
}


//draw radar blip request
void draw::draw_blip(int pos_x , int pos_y, int size, draw_color * color) {

    SDL_Rect blip_rect;

    //convert request into SDL2 rectangle & color
    blip_rect.x = pos_x - (size / 2);
    blip_rect.y = pos_y - (size / 2);
    blip_rect.w = size;
    blip_rect.h = size;
    SDL_SetRenderDrawColor(this->sdl_renderer, color->r, color->g, color->b, color->a);

    //draw blip
    SDL_RenderFillRect(this->sdl_renderer, &blip_rect);
}
