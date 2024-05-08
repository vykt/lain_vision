#include <iostream>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysymdef.h>
#include <GL/gl.h>
#include <GL/glx.h>

int main(int argc, char** argv) {
	Display* display;
	Window window;
	Screen* screen;
	int screenId;
	XEvent ev;

	// Open the display
	display = XOpenDisplay(NULL);
	if (display == NULL) {
		std::cout << "Could not open display\n";
		return 1;
	}
	screen = DefaultScreenOfDisplay(display);
	screenId = DefaultScreen(display);

	
	// Check GLX version
	GLint majorGLX, minorGLX = 0;
	glXQueryVersion(display, &majorGLX, &minorGLX);
	if (majorGLX <= 1 && minorGLX < 2) {
		std::cout << "GLX 1.2 or greater is required.\n";
		XCloseDisplay(display);
		return 1;
	}
	else {
		std::cout << "GLX version: " << majorGLX << "." << minorGLX << '\n';
	}

	// GLX, create XVisualInfo, this is the minimum visuals we want
	GLint glxAttribs[] = {
		GLX_RGBA,
		GLX_DOUBLEBUFFER,
		GLX_DEPTH_SIZE,     24,
		GLX_STENCIL_SIZE,   8,
		GLX_RED_SIZE,       8,
		GLX_GREEN_SIZE,     8,
		GLX_BLUE_SIZE,      8,
		GLX_SAMPLE_BUFFERS, 0,
		GLX_SAMPLES,        0,
		None
	};
	XVisualInfo* visual = glXChooseVisual(display, screenId, glxAttribs);
	
	if (visual == 0) {
		std::cout << "Could not create correct visual window.\n";
		XCloseDisplay(display);
		return 1;
	}

	// Open the window
	XSetWindowAttributes windowAttribs;
	windowAttribs.border_pixel = BlackPixel(display, screenId);
	windowAttribs.background_pixel = WhitePixel(display, screenId);
	windowAttribs.override_redirect = True;
	windowAttribs.colormap = XCreateColormap(display, RootWindow(display, screenId), visual->visual, AllocNone);
	windowAttribs.event_mask = ExposureMask;
	window = XCreateWindow(display, RootWindow(display, screenId), 0, 0, 320, 200, 0, visual->depth, InputOutput, visual->visual, CWBackPixel | CWColormap | CWBorderPixel | CWEventMask, &windowAttribs);

	// Create GLX OpenGL context
	GLXContext context = glXCreateContext(display, visual, NULL, GL_TRUE);
	glXMakeCurrent(display, window, context);

	std::cout << "GL Vendor: " << glGetString(GL_VENDOR) << "\n";
	std::cout << "GL Renderer: " << glGetString(GL_RENDERER) << "\n";
	std::cout << "GL Version: " << glGetString(GL_VERSION) << "\n";
	std::cout << "GL Shading Language: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << "\n";

	// Show the window
	XClearWindow(display, window);
	XMapRaised(display, window);

	// Set GL Sample stuff
	glClearColor(0.5f, 0.6f, 0.7f, 1.0f);

	// Enter message loop
	while (true) {
		XNextEvent(display, &ev);
		if (ev.type == Expose) {
			XWindowAttributes attribs;
			XGetWindowAttributes(display, window, &attribs);
			glViewport(0, 0, attribs.width, attribs.height);
		}
		
		// OpenGL Rendering
		glClear(GL_COLOR_BUFFER_BIT);

		glBegin(GL_TRIANGLES);
			glColor3f(  1.0f,  0.0f, 0.0f);
			glVertex3f( 0.0f, -1.0f, 0.0f);
			glColor3f(  0.0f,  1.0f, 0.0f);
			glVertex3f(-1.0f,  1.0f, 0.0f);
			glColor3f(  0.0f,  0.0f, 1.0f);
			glVertex3f( 1.0f,  1.0f, 0.0f);
		glEnd();

		// Present frame
		glXSwapBuffers(display, window);
	}

	// Cleanup GLX
	glXDestroyContext(display, context);

	// Cleanup X11
	XFree(visual);
	XFreeColormap(display, windowAttribs.colormap);
	XDestroyWindow(display, window);
	XCloseDisplay(display);
	return 0;
}
