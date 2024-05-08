//https://github.com/gamedevtech/X11OpenGLWindow

    /*
     *  behold, ye faithless
     */

#include <iostream>

#include <cstdio>
#include <cstring>

#include <unistd.h>

#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/extensions/shape.h>
#include <X11/extensions/Xfixes.h>
#include <X11/extensions/Xcomposite.h>

#include <SDL2/SDL.h>


int main() {

    int ret;

    //X11 specific 
    //display
    Display * x11_display;
    
    //screen values
    int screen_index;
    unsigned long border, background;
    
    //windows & window attributes
    Window root_win, overlay_win;
    XSetWindowAttributes overlay_attr;

    //graphics context
    //GC graphics_ctx;
    //XGCValues graphics_ctx_values;

    //overlay on-top & input-pass-through & transparent vars
    Atom wm_state_above;
    XserverRegion region;
    unsigned long opacity;

    //colors & shapes
    Colormap colormap;
    GC gfx_ctx;
    XColor draw_color;

    //transparency
    XVisualInfo * visual_info;
    XVisualInfo visual_template;
    int num_visual_infos;
    Visual * visual;

    //SDL2
    char overlay_win_str_id[64] = {};
    SDL_Window * sdl_window;
    SDL_Renderer * sdl_renderer;
    
    SDL_Rect sdl_rect;

    // --- TEXT ---


    //X11 init display & get screen
    x11_display = XOpenDisplay(nullptr);
    if (x11_display == nullptr) {
        std::cerr << "[error] XOpenDisplay()" << std::endl;
        return -1;
    }

    //screen values 
    screen_index = DefaultScreen(x11_display);
    border       = 0;//BlackPixel(x11_display, screen_index);
    background   = 0;//BlackPixel(x11_display, screen_index);
    
    //setup root window
    root_win = RootWindow(x11_display, screen_index);

    //get ARGB visual
    visual_template.screen = screen_index;
    visual_template.depth = 32;
    visual_template.c_class = TrueColor;
    visual_info = XGetVisualInfo(x11_display, VisualScreenMask | VisualDepthMask | VisualClassMask,
                                 &visual_template, &num_visual_infos);

    if (!visual_info) {
        std::cout << "XGetVisualInfo() ERRR\n";
        return -1;
    }
    
    visual = nullptr;
    for (int i = 0; i < num_visual_infos; ++i) {
        if ((visual_info[i].red_mask == 0xff0000) && (visual_info[i].green_mask == 0x00ff00) && 
            (visual_info[i].blue_mask == 0x0000ff)) {
                visual = visual_info[i].visual;
                break;
        }
    }

    if (visual == nullptr) {
        std::cout << "Some bullshit\n";
        return -1;
    }

    XFree(visual_info);

    //create colormap & setup draw_color & set foreground color
    colormap = XCreateColormap(x11_display, root_win, visual, AllocNone);
    draw_color.red = 25500;
    draw_color.green = 25500;
    draw_color.blue = 25500;
    draw_color.flags = DoRed | DoGreen | DoBlue;
    XAllocColor(x11_display, colormap, &draw_color);

    //setup window attributes
    overlay_attr.border_pixel      = border;
    overlay_attr.background_pixel  = background;
    overlay_attr.override_redirect = True; //unmanaged by WM
    overlay_attr.colormap          = colormap;
    overlay_attr.event_mask        = ExposureMask; // | KeyPressMask;

    //setup overlay window
    overlay_win = XCreateWindow(x11_display, root_win, 0, 0, 2560, 1440, 0, 32, InputOutput, visual,
                                CWBorderPixel | CWBackPixel | CWEventMask | CWOverrideRedirect | CWColormap, &overlay_attr);

    //enable overlay on-top & input-pass-through
    wm_state_above = XInternAtom(x11_display, "_NET_WM_STATE_ABOVE", 1);
    XChangeProperty(x11_display, overlay_win, XInternAtom(x11_display, "_NET_WM_STATE", 1), XA_ATOM, 32,
                    PropModeReplace, (unsigned char *)&wm_state_above, 1);
    
    region = XFixesCreateRegion(x11_display, nullptr, 0);
    XFixesSetWindowShapeRegion(x11_display, overlay_win, ShapeInput, 0, 0, region);
    XFixesDestroyRegion(x11_display, region);

    //enable SDL
    ret = SDL_Init(SDL_INIT_VIDEO);
    if (ret) {
        std::cout << "SDL_Init();\n";
        return -1;
    }

    //TODO TODO TODO TODO TODO TODO THIS IS CAUSING ERROR

    //attach SDL to overlay
    sdl_window = SDL_CreateWindowFrom((void *) overlay_win);
    if (!sdl_window) {
        std::cout << "SDL_CreateWindowFrom();\n";
        return -1;
    }

    //TODO TODO TODO TODO TODO TODO

    //create renderer for X11/SDL window
    sdl_renderer = SDL_CreateRenderer(sdl_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if(!sdl_renderer) {
        std::cout << "SDL_CreateRenderer();\n";
        return -1;
    }

    //enable blending (transparency) in the SDL renderer
    ret = SDL_SetRenderDrawBlendMode(sdl_renderer, SDL_BLENDMODE_BLEND);
    if (ret) {
        std::cout << "SDL_SetRenderDrawBlendMode();\n";
        return -1;
    }

    // --- DRAW --- 

    //clear background
    SDL_SetRenderDrawColor(sdl_renderer, 0, 0, 0, 0);
    SDL_RenderClear(sdl_renderer);

    //draw square
    sdl_rect.x = 100;
    sdl_rect.y = 100;
    sdl_rect.w = 100;
    sdl_rect.h = 100;
    
    while (1) {
        SDL_SetRenderDrawColor(sdl_renderer, 128, 0, 0, 255);
        SDL_RenderFillRect(sdl_renderer, &sdl_rect);

        //update display
        SDL_RenderPresent(sdl_renderer);

        //map overlay window
        XMapWindow(x11_display, overlay_win);
 
        //send to screen
        XFlush(x11_display);
    }
    /*graphics_ctx = XCreateGC(x11_display, overlay_win, 0, &graphics_ctx_values);

    //ensure the foreground color is fully opaque for visibility
    XSetForeground(x11_display, graphics_ctx, 0xffffffff); // White and fully opaque

    //select font
    XFontStruct* font = XLoadQueryFont(x11_display, "fixed");
    if (font) {
        XSetFont(x11_display, graphics_ctx, font->fid);
    }

    //draw some text
    XDrawString(x11_display, overlay_win, graphics_ctx, 50, 50, "Hello, transparent world!", 27);

    //cleanup
    XFreeGC(x11_display, graphics_ctx);
    if (font) {
        XFreeFont(x11_display, font);
    }*/



    //press [enter] to terminate
    std::cin.get();


    //SDL2 clean up
    SDL_DestroyWindow(sdl_window);
    SDL_Quit();

    //X11 clean up
    XDestroyWindow(x11_display, overlay_win);
    XCloseDisplay(x11_display);

    return 0;
}
