#include <windows.h>
#include <stdint.h>
#include "../xlilon.h"

// ============================================================================
// GLOBAL STATE
// ============================================================================
static bool GlobalRunning = true;       // Is the app running?
static XlilonBuffer GlobalBackBuffer;   // Our pixel container
static BITMAPINFO GlobalBitmapInfo;     // Windows needs this to understand our pixels

// ============================================================================
// BUFFER MANAGEMENT
// ============================================================================

// Resize/Create the pixel buffer when the window starts or resizes.
static void Win32ResizeDIBSection(XlilonBuffer* buffer, int width, int height) {
    // 1. Free old memory if it exists
    if (buffer->memory) {
        VirtualFree(buffer->memory, 0, MEM_RELEASE);
    }

    // 2. Set dimensions
    buffer->width = width;
    buffer->height = height;
    buffer->pitch = width * 4; // 4 bytes per pixel (RGBA)

    // 3. Fill the BITMAPINFO struct (The "Header" for our image)
    GlobalBitmapInfo.bmiHeader.biSize = sizeof(GlobalBitmapInfo.bmiHeader);
    GlobalBitmapInfo.bmiHeader.biWidth = buffer->width;
    
    // NOTE: Negative height tells Windows this is a "Top-Down" bitmap.
    // That means the first byte is the Top-Left pixel. 
    // If positive, it's Bottom-Left (standard BMP format).
    // We want Top-Left because that's how we read charts (0,0 at top-left).
    GlobalBitmapInfo.bmiHeader.biHeight = -buffer->height; 
    
    GlobalBitmapInfo.bmiHeader.biPlanes = 1;
    GlobalBitmapInfo.bmiHeader.biBitCount = 32; // 32 bits (8 Red, 8 Green, 8 Blue, 8 Padding)
    GlobalBitmapInfo.bmiHeader.biCompression = BI_RGB;

    // 4. Allocate new memory
    // We request (width * height * 4) bytes from the OS.
    int bitmapMemorySize = (buffer->width * buffer->height) * 4;
    
    // VirtualAlloc gives us a raw page of memory.
    buffer->memory = (uint32_t*)VirtualAlloc(0, bitmapMemorySize, MEM_COMMIT, PAGE_READWRITE);
}

// Copy our buffer to the actual window (The "Blit")
static void Win32UpdateWindow(HDC deviceContext, int windowWidth, int windowHeight, XlilonBuffer* buffer) {
    StretchDIBits(
        deviceContext,
        0, 0, windowWidth, windowHeight, // Destination (Screen)
        0, 0, buffer->width, buffer->height, // Source (Our Buffer)
        buffer->memory,
        &GlobalBitmapInfo,
        DIB_RGB_COLORS,
        SRCCOPY
    );
}

// ============================================================================
// WINDOW EVENT CALLBACK
// ============================================================================

LRESULT CALLBACK Win32MainWindowCallback(HWND window, UINT message, WPARAM wParam, LPARAM lParam) {
    LRESULT result = 0;

    switch (message) {
        case WM_CLOSE: {
            GlobalRunning = false;
        } break;

        case WM_DESTROY: {
            GlobalRunning = false;
        } break;

        case WM_SIZE: {
            // Window was resized! We technically should resize the buffer here.
            // For v0.0.1 simplicity, we just initialize size in WinMain.
            // But getting the rect is useful.
        } break;

        default: {
            result = DefWindowProc(window, message, wParam, lParam);
        } break;
    }
    return result;
}

// ============================================================================
// ENTRY POINT (WinMain)
// ============================================================================

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
    
    // 1. Create the Window Class
    WNDCLASS windowClass = {};
    windowClass.style = CS_HREDRAW | CS_VREDRAW; // Redraw if size changes
    windowClass.lpfnWndProc = Win32MainWindowCallback; // Who handles messages?
    windowClass.hInstance = hInstance;
    windowClass.lpszClassName = "XlilonWindowClass";

    if (RegisterClass(&windowClass)) {
        
        // 2. Create the Window
        HWND window = CreateWindowEx(
            0,
            windowClass.lpszClassName,
            "Xlilon v0.0.1 (Bare Metal)",
            WS_OVERLAPPEDWINDOW | WS_VISIBLE, // Standard window with borders
            CW_USEDEFAULT, CW_USEDEFAULT,     // x, y position
            1280, 720,                        // Width, Height
            0, 0, hInstance, 0
        );

        if (window) {
            
            // 3. Initialize Graphics Buffer
            // We lock it to 1280x720 for now.
            Win32ResizeDIBSection(&GlobalBackBuffer, 1280, 720);
            
            Xlilon_Init(); // Init our library

            // 4. The Main Loop (Real-time Game Loop)
            while (GlobalRunning) {
                
                // A. Handle Windows Messages (Input, Close, etc.)
                MSG message;
                while (PeekMessage(&message, 0, 0, 0, PM_REMOVE)) {
                    if (message.message == WM_QUIT) {
                        GlobalRunning = false;
                    }
                    TranslateMessage(&message);
                    DispatchMessage(&message);
                }

                // B. RENDER (Call our library)
                Xlilon_Render(&GlobalBackBuffer);

                // C. BLIT (Push to screen)
                HDC deviceContext = GetDC(window);
                
                // Get current window dimensions so we stretch correctly
                RECT clientRect;
                GetClientRect(window, &clientRect);
                int windowWidth = clientRect.right - clientRect.left;
                int windowHeight = clientRect.bottom - clientRect.top;

                Win32UpdateWindow(deviceContext, windowWidth, windowHeight, &GlobalBackBuffer);
                
                ReleaseDC(window, deviceContext);
            }
        }
    }

    return 0;
}
