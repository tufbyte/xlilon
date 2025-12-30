#ifndef XLILON_H
#define XLILON_H

#include <cstdint> // For uint32_t, int32_t

// ============================================================================
// XLILON: Bare Metal Data Visualization Library (v0.0.1)
// ============================================================================

// The Canvas
// This struct represents the block of memory we are drawing into.
// The Platform Layer (Win32/Linux) owns this memory and passes it to us.
struct XlilonBuffer {
    uint32_t* memory; // Pointer to the raw pixel data (0xAARRGGBB format)
    int width;        // Width of the window/buffer in pixels
    int height;       // Height of the window/buffer in pixels
    int pitch;        // Bytes per row (usually width * 4, but good to be explicit)
};

// ============================================================================
// PUBLIC API
// ============================================================================

// NOTE: We use "extern C" to prevent C++ name mangling. 
// This makes it trivial to bind to Rust, Python, or C later.
extern "C" {

    // 1. Initialization (Reserved for future allocator setup)
    void Xlilon_Init();

    // 2. The Main Render Function
    // The Platform Layer calls this every frame (or when a repaint is needed).
    // It is Xlilon's job to fill the 'buffer->memory' with colors.
    void Xlilon_Render(XlilonBuffer* buffer);

}

#endif // XLILON_H
