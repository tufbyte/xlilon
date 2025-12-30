#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <stdio.h>
#include "../xlilon.h"

// ============================================================================
// GLOBAL STATE
// ============================================================================
static XlilonBuffer GlobalBackBuffer;
static XImage* GlobalXImage = nullptr;

// ============================================================================
// ENTRY POINT
// ============================================================================
int main() {
    // 1. Open connection to the X Server
    Display* display = XOpenDisplay(NULL);
    if (!display) {
        fprintf(stderr, "Cannot open X display\n");
        return 1;
    }

    int screen = DefaultScreen(display);
    int depth = DefaultDepth(display, screen);
    Visual* visual = DefaultVisual(display, screen);

    // 2. Create the Window
    int width = 1280;
    int height = 720;
    
    Window window = XCreateSimpleWindow(
        display, RootWindow(display, screen),
        10, 10, width, height,
        1, BlackPixel(display, screen), WhitePixel(display, screen)
    );

    // Listen for Close events and Key presses
    XSelectInput(display, window, ExposureMask | KeyPressMask | StructureNotifyMask);
    
    // Set Window Title
    XStoreName(display, window, "Xlilon v0.0.1 (Linux X11)");
    XMapWindow(display, window);

    // 3. Setup the Memory Buffer
    // We allocate memory for the pixels (RGBA)
    GlobalBackBuffer.width = width;
    GlobalBackBuffer.height = height;
    GlobalBackBuffer.pitch = width * 4;
    
    char* memory = (char*)malloc(width * height * 4);
    GlobalBackBuffer.memory = (uint32_t*)memory;

    // Create an XImage structure that points to our memory
    GlobalXImage = XCreateImage(
        display, visual, depth, ZPixmap, 0,
        memory, width, height, 32, 0
    );

    // Initialize Library
    Xlilon_Init();

    // 4. The Main Loop
    bool running = true;
    XEvent event;

    while (running) {
        // Check for events (Non-blocking check could be done with XPending)
        // For v0.0.1, we block on XNextEvent to save CPU when nothing happens.
        // For a real-time game loop, use XPending(display).
        XNextEvent(display, &event);

        if (event.type == KeyPress) {
            // Close on Escape key
            // KeySym keysym = XLookupKeysym(&event.xkey, 0);
            // if (keysym == XK_Escape) running = false;
        }
        
        if (event.type == ClientMessage) {
             // Handle "X" button click (requires setting WM_DELETE_WINDOW protocol)
        }

        // RENDER:
        // 1. Update our pixel array
        Xlilon_Render(&GlobalBackBuffer);

        // 2. Blit to Screen
        XPutImage(
            display, window, DefaultGC(display, screen),
            GlobalXImage,
            0, 0, 0, 0,
            width, height
        );
    }

    // Cleanup
    XDestroyWindow(display, window);
    XCloseDisplay(display);
    free(GlobalBackBuffer.memory);
    
    return 0;
}
