#include "xlilon.h"

// ============================================================================
// INTERNAL HELPERS
// ============================================================================

// Helper to pack Red, Green, Blue into a 32-bit integer.
// Format: 0xAARRGGBB (Alpha, Red, Green, Blue)
// This is the standard layout for Windows GDI and X11 TrueColor.
static inline uint32_t MakeColor(uint8_t r, uint8_t g, uint8_t b) {
    // 255 << 24 sets Alpha to full opacity (0xFF)
    return (255 << 24) | (r << 16) | (g << 8) | (b << 0);
}

// ============================================================================
// PUBLIC API IMPLEMENTATION
// ============================================================================

extern "C" {

    void Xlilon_Init() {
        // v0.0.1: No state initialization needed yet.
        // In the future, this is where we will pre-calculate 
        // sine tables or load font bitmaps into static arrays.
    }

    void Xlilon_Render(XlilonBuffer* buffer) {
        // --------------------------------------------------------------------
        // THE TEST PATTERN (v0.0.1)
        // --------------------------------------------------------------------
        // We will draw a "Weird Gradient" to prove we have pixel control.
        // If you see a smooth transition of colors, the engine works.
        
        // 'row' points to the start of the current row in bytes.
        uint8_t* row = (uint8_t*)buffer->memory;

        for (int y = 0; y < buffer->height; ++y) {
            
            // Cast the row pointer to a 32-bit pixel pointer
            uint32_t* pixel = (uint32_t*)row;

            for (int x = 0; x < buffer->width; ++x) {
                
                // 1. GENERATE DATA
                // Let's make the color depend on the coordinate.
                // Casting to uint8_t automatically handles wrapping (0-255).
                uint8_t green = (uint8_t)y; 
                uint8_t blue  = (uint8_t)x;
                
                // 2. RENDER PIXEL
                // Write directly to RAM. No API calls here.
                *pixel = MakeColor(0, green, blue);
                
                // Move to the next pixel
                pixel++;
            }

            // Move to the next row
            // We use 'pitch' (bytes per row) to jump safely.
            // This handles cases where the window width isn't perfectly aligned.
            row += buffer->pitch;
        }
        
        // --------------------------------------------------------------------
        // SIMPLE BOX DRAWING TEST (Optional - Uncomment to test)
        // --------------------------------------------------------------------
        /*
        int boxX = 100;
        int boxY = 100;
        int boxW = 50;
        int boxH = 50;
        
        // Simple bounding checks to avoid crashing if window is too small
        if (boxX + boxW < buffer->width && boxY + boxH < buffer->height) {
            uint32_t white = MakeColor(255, 255, 255);
            uint32_t* startPixel = (uint32_t*)((uint8_t*)buffer->memory + boxY * buffer->pitch) + boxX;
            
            for (int by = 0; by < boxH; ++by) {
                uint32_t* boxPixel = startPixel;
                for (int bx = 0; bx < boxW; ++bx) {
                    *boxPixel++ = white;
                }
                // Move startPixel down one row
                startPixel = (uint32_t*)((uint8_t*)startPixel + buffer->pitch);
            }
        }
        */
    }

}
