#include <Cocoa/Cocoa.h>
#include "../xlilon.h"

// ============================================================================
// CUSTOM VIEW (The "Canvas")
// ============================================================================
// This class overrides the standard Mac window drawing to display our pixels.
@interface XlilonView : NSView {
    @public
    XlilonBuffer* backBuffer;
}
@end

@implementation XlilonView
- (void)drawRect:(NSRect)dirtyRect {
    if (backBuffer && backBuffer->memory) {
        // 1. Render the pixels using our C++ engine
        Xlilon_Render(backBuffer);

        // 2. Create a CoreGraphics Image from our buffer
        CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
        CGContextRef bitmapContext = CGBitmapContextCreate(
            backBuffer->memory,
            backBuffer->width,
            backBuffer->height,
            8,                  // bits per component
            backBuffer->pitch,  // bytes per row
            colorSpace,
            kCGImageAlphaPremultipliedFirst | kCGBitmapByteOrder32Little // BGRA/ARGB config
        );

        CGImageRef cgImage = CGBitmapContextCreateImage(bitmapContext);
        
        // 3. Draw it to the Mac Window
        CGContextRef currentContext = [[NSGraphicsContext currentContext] CGContext];
        CGContextDrawImage(currentContext, CGRectMake(0, 0, backBuffer->width, backBuffer->height), cgImage);

        // Cleanup temp objects
        CGImageRelease(cgImage);
        CGContextRelease(bitmapContext);
        CGColorSpaceRelease(colorSpace);
    }
}
@end

// ============================================================================
// APP DELEGATE (Handles lifecycle)
// ============================================================================
@interface XlilonDelegate : NSObject <NSApplicationDelegate, NSWindowDelegate> {
    NSWindow* window;
    XlilonView* view;
    XlilonBuffer buffer;
}
@end

@implementation XlilonDelegate

- (void)applicationDidFinishLaunching:(NSNotification *)notification {
    // 1. Setup Window Frame
    NSRect screenRect = [[NSScreen mainScreen] frame];
    NSRect windowRect = NSMakeRect(0, 0, 1280, 720);
    
    // Center it roughly
    windowRect.origin.x = (screenRect.size.width - windowRect.size.width) / 2;
    windowRect.origin.y = (screenRect.size.height - windowRect.size.height) / 2;

    // 2. Create Window
    window = [[NSWindow alloc] initWithContentRect:windowRect
                                         styleMask:(NSWindowStyleMaskTitled |
                                                    NSWindowStyleMaskClosable |
                                                    NSWindowStyleMaskResizable |
                                                    NSWindowStyleMaskMiniaturizable)
                                           backing:NSBackingStoreBuffered
                                             defer:NO];
    [window setTitle:@"Xlilon v0.0.1 (macOS Cocoa)"];
    [window makeKeyAndOrderFront:nil];
    [window setDelegate:self];

    // 3. Setup Buffer
    buffer.width = 1280;
    buffer.height = 720;
    buffer.pitch = buffer.width * 4;
    buffer.memory = (uint32_t*)malloc(buffer.width * buffer.height * 4);

    // 4. Setup View
    view = [[XlilonView alloc] initWithFrame:windowRect];
    view->backBuffer = &buffer;
    [window setContentView:view];
    
    // Initialize Engine
    Xlilon_Init();

    // 5. Start a Timer to redraw at 60 FPS
    [NSTimer scheduledTimerWithTimeInterval:0.016
                                     target:view
                                   selector:@selector(setNeedsDisplay:)
                                   userInfo:nil
                                    repeats:YES];
}

- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)sender {
    return YES;
}

@end

// ============================================================================
// ENTRY POINT
// ============================================================================
int main(int argc, const char * argv[]) {
    @autoreleasepool {
        NSApplication *app = [NSApplication sharedApplication];
        XlilonDelegate *delegate = [[XlilonDelegate alloc] init];
        [app setDelegate:delegate];
        [app setActivationPolicy:NSApplicationActivationPolicyRegular];
        [app run];
    }
    return 0;
}
