#pragma once

#include <cstdint>
#include <cstdio>
#include <cstring>
#include <cstdarg>

namespace HDMIShield {

enum Error {
    NONE = 0,

    NOT_CONNECTED,
    UNRESPONSIVE,

    PACKET_OVERFLOW,
    RETRIES_EXCEEDED,
    PACKET_MALFORMED,

    OUT_OF_MEMORY,
};

// Represents a 24-bit colour + alpha channel
struct Colour {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
    uint8_t alpha;
};

// A point in 2D space. By convention, (0, 0) is at the top-left
struct Point {
    uint16_t x;
    uint16_t y;
};

// A line between two points
struct Line {
    Point start;
    Point end;
};

// A rectangle with width and height
struct Rectangle {
    uint16_t width;
    uint16_t height;
};

// A rectangle at a specific point in 2D space, where the origin is its 
// top-left corner
struct Box {
    Point origin;
    Rectangle area;
};

// A handle for a surface resource on the shield. Surfaces allow for complex
// images to be built and reused
struct Surface {
    uint16_t ID;
    Rectangle area;
};

// A handle for a font resource on the shield
typedef uint8_t Font;

/* Configures the Arduino's I/O, and checks the board is ready

Returns
-------
error
    if an error occured
*/
Error initialize();

// Soft-resets the entire board
Error reset();

/* Checks whether a display is connected to the board

Arguments
---------
connected
    result emplaced here

Returns
-------
error
    if an error occured
*/
Error displayConnected(bool &connected);

/* Loads a font into memory 

Can be used in creating text surfaces

Arguments
---------
font
    the font handle is emplaced here
name
    the name of the font, already loaded onto the board
size
    the size of the font (in pixels)

Returns
-------
error
    if an error occured
*/
Error loadFont(Font &font, const char *name, const uint8_t &size);

/* Releases the memory held by a font file

Arguments
---------
font
    the handle of the font to free

Returns
-------
error
    if an error occured
*/
Error freeFont(Font &font);

/* Creates a surface of a given size

Arguments
---------
surface
    populated with its handle and size on success
area
    the size of the surface to create

Returns
-------
error
    if an error occured
*/
Error createSurface(Surface &surface, const Rectangle &area);

/* Renders text to a surface

Arguments
---------
surface
    the surface to render to
font
    the font to render with
text
    the text to render
colour
    the colour to make the text. At present only solid text is supported

Returns
-------
error
    if an error occured
*/
Error createTextSurface(Surface &surface, 
        const Font &font, 
        const char *text, 
        const Colour colour = Colour{0, 0, 0});

/* Blits one surface onto another

The blitting process superimposes one surface onto another using bitwise 
arithmetic. Transparency is preserved.

Arguments
---------
source
    the surface to blit from
target
    the surface to blit onto
sourceRegion
    the area of the source surface to blit with; if none is given, the entire
    surface is used
targetDestination
    the point on the target surface to copy the blitted surface. If none is 
    given, the top-left corner is used.

Returns
-------
error
    if an error occured
*/
Error blitSurface(const Surface &source,
        const Surface &target,
        const Box *sourceRegion,
        const Point *targetDestination);

/* Frees the memory held by a surface

Arguments
---------
surface
    the surface to free

Returns
-------
error
    if an error occured
*/
Error freeSurface(Surface &surface);

/* Draws a point on a surface

Arguments
---------
surface
    the surface to draw on
point
    the point at which to draw
colour
    the colour of the pixel to set. If none is given, black is used

Returns
-------
error
    if an error occured
*/
Error drawPoint(const Surface &surface, 
        const Point &point, 
        const Colour colour = Colour{0, 0, 0});

/* Draws a line on a surface

Arguments
---------
surface
    the surface to draw on
line
    the line to draw
colour
    the line's colour. If none is given, black is used

Returns
-------
error
    if an error occured
*/
Error drawLine(const Surface &surface,
        const Line &line,
        const Colour colour = Colour{0, 0, 0});

/* Draws a box on a surface

Arguments
---------
surface
    the surface to draw on
box
    the box to draw
colour
    the colour to use. If none is given, black is used
fill
    whether to fill the inside of the box, or just draw its outline

Returns
-------
error
    if an error occured
*/
Error drawBox(const Surface &surface,
        const Box &box,
        const Colour colour = Colour{0, 0, 0},
        const bool fill = false);

/* Renders a surface

The surface is copied to the region in memory where the frame buffer is stored.
When the update function is called, this buffer is made active

Arguments
---------
surface
    the surface to copy
sourceRegion
    the area of the source surface to blit with; if none is given, the entire
    surface is used
targetDestination
    the point in the display buffer to copy the blitted surface to. If none is
    given, the top-left corner is used.

Returns
-------
error
    if an error occured
*/
Error render(const Surface &surface, 
        const Box *sourceRegion, 
        const Point *targetDestination);

// Clears the display buffer
Error clear();

// Swaps the frame and active buffers
Error update();

}; // namesapce HDMIShield