#include "hdmi_shield.hpp"

namespace HDMIShield {

enum Code {

    // 0x00-0x1F: System
    RESET      = 0x01,
    RETRANSMIT = 0x02,

    // 0x10-0x1F: Display metadata
    DISPLAY_CONNECTED = 0x10,

    // 0x20-0x2F: Reserved for future use (further metadata)
    // 0x30-0x3F: Asset handling (fonts)
    LOAD_FONT = 0x30,
    FREE_FONT = 0x3F,

    // 0x40-0x5F Reserved for future use (images, audio)
    // 0x60-0x6F: Surface manipulation
    CREATE_SURFACE = 0x60,
    CREATE_TEXT_SURFACE = 0x61,
    BLIT_SURFACE = 0x61,
    FREE_SURFACE = 0x6F,

    // 0x70-0x7F: Reserved for future use (textures)
    // 0x80-0x8F: Naive draw functions functions
    DRAW_POINT = 0x80,
    DRAW_LINE = 0x81,
    DRAW_BOX = 0x82,

    // 0x90-0xEF: Reserved for future use (meshes, texture mapping, etc.)
    // 0xF0-0xFF: Display functions
    RENDER = 0xF0,
    CLEAR = 0xF1,
    UPDATE = 0xF2,
};

/* Waits for the shield to be ready

Checks that the shield is plugged in, and waits for it to give a signal that
it's ready to recieve data. After a set period of time, if the ready signal 
hasn't been given, the board is presumed unresponsive.

Returns
-------
error
    if an error occured
*/
Error awaitReady() {

    // To-do: perform some sort of check that verifies the board is there
    if(false) {
        return Error::NOT_CONNECTED;
    }

    // Wait for the shield to be ready
    for(uint16_t index = 0; index < 100; index += 1) {

        // To-do: check that a 'ready' pin or similar goes high
        if(true) {
            return Error::NONE;
        }

        // To-do: delay here a set no. of milliseconds
    }

    return Error::UNRESPONSIVE;
}

/* Transmits a formatted string of characters

Creates a statically allocated buffer of memory (up to 128 bytes large) to 
format and store the packet in. Transmits the packet as well as an error 
checksum to the board, and waits for acknowledgement. If the acknowledgement
fails more than a cetain number of times, the transmission is abandoned

Arguments
---------
format
    the packet format, as one might use with printf or similar
...
    a variadic argument pack that gets unwound

Returns
-------
error
    if an error occured
*/
Error transmit(const char *format, ...) {
    va_list arguments;
    va_start(arguments, format);

    uint8_t size = 2;
    while(true) {
        
        // Unpack the formatted packet into a buffer of gradually increasing
        // size
        char buffer[size];
        if(vsnprintf(buffer, size, format, arguments) < 0) {
            if(size == 128) {
                return Error::PACKET_OVERFLOW;
            }

            size *= 2;
            continue;
        }
        va_end(arguments);

        // Wait for the board to be ready
        Error readyError = awaitReady();
        if(readyError) {
            return readyError;
        }

        // Transmit the packet
        for(char *character = buffer; character; character += 1) {
            // To-do: transmit that character
        }

        break;
    }

    // Wait for acknowledgement
    if(false) {
        return Error::RETRIES_EXCEEDED;
    }

    // Report packet formatting mistakes
    if(false) {
        return Error::PACKET_MALFORMED;
    }

    return Error::NONE;
}

/* Reads a packet from the board

Waits until the board has transmitted a full packet, before returning it. If
the checksum fails, will request the board retransmits. After a certain number
of retries, the read will be abandoned.

Note: the buffer size should be the number of bytes expected for the response,
_not_ including a null-terminator.

Arguments
---------
buffer
    the buffer to write to
bufferSize
    the capacity of the buffer

Returns
-------
error
    if an error occured
*/
Error receive(char *buffer, const uint16_t bufferSize) {
    return Error::NONE;
}

/* Both transmits and receives a packet in one call

For more details on these two operations, see the transmit and receive 
functions

Arguments
---------
buffer
    the buffer to write to
bufferSize
    the capacity of the buffer
format
    the packet format, as one might use with printf or similar
...
    a variadic argument pack that gets unwound

Returns
-------
error
    if an error occured
*/
Error transcieve(char *buffer,
            const uint16_t bufferSize,
            const char *format,
            ...) {
    
    va_list arguments;
    va_start(arguments, format);
    Error transmitError = transmit(format, arguments);
    if(transmitError) {
        return transmitError;
    }
    va_end(arguments);

    return receive(buffer, bufferSize);
}

// Configures the Arduino's I/O, and checks the board is ready
Error initialize() {
    // Set up pin directions and modes

    Error readyError = awaitReady();
    if(readyError) {
        return readyError;
    }
}

// Checks whether a display is connected to the board
Error displayConnected(bool &connected) {

    char response;
    Error transceiveError = transcieve(&response, 
            1, 
            "%c", 
            Code::DISPLAY_CONNECTED);
    if(transceiveError) {
        return transceiveError;
    }

    connected = (bool)response;
    return Error::NONE;
}

// Loads a font into memory
Error loadFont(Font &font, const char *name, const uint8_t &size) {
    
    char response;
    Error transceiveError = transcieve(&response, 
            1, 
            "%c%s%c", 
            Code::LOAD_FONT,
            name,
            size);
    if(transceiveError) {
        return transceiveError;
    }

    font = (Font)response;
    return Error::NONE;
}

// Releases the memory held by a font file
Error freeFont(Font &font) {
    Error transmitError = transmit("%c%c", Code::FREE_FONT, font);
    if(transmitError) {
        return transmitError;
    }

    font = 0;
    return Error::NONE;
}

// Creates a surface of a given size
Error createSurface(Surface &surface, const Rectangle &area) {

    char response[2];
    Error transceiveError = transcieve(response, 
            2, 
            "%c%u%u", 
            Code::CREATE_SURFACE,
            area.width,
            area.height);
    if(transceiveError) {
        return transceiveError;
    }

    surface.ID = response[0] << 8 | response[1];
    surface.area = area;
    return Error::NONE;
}

// Renders text to a surface
Error createTextSurface(Surface &surface,
        const Font &font,
        const char *text,
        const Colour colour) {
    
    const uint16_t textLength = strlen(text);

    char response[6];
    Error transceiveError = transcieve(response, 
            4, 
            "%c%c%u%s%c%c%c%c", 
            Code::CREATE_TEXT_SURFACE,
            font,
            textLength,
            text,
            colour.red,
            colour.green,
            colour.blue,
            colour.alpha);
    if(transceiveError) {
        return transceiveError;
    }

    surface.ID = response[0] << 8 | response[1];
    surface.area.width = response[2] << 8 | response[3];
    surface.area.height = response[4] << 8 | response[5];

    return Error::NONE;
}

// Frees the memory held by a surface
Error freeSurface(Surface &surface) {
    Error transmitError = transmit("%c%u", Code::FREE_SURFACE, surface.ID);
    if(transmitError) {
        return transmitError;
    }

    surface.ID = 0;
    return Error::NONE;
}

// Blits one surface onto another 
Error blitSurface(const Surface &source, 
        const Surface &target, 
        const Box *sourceRegion, 
        const Point *targetDestination) {

    // Evaluate region of source to copy
    Box region;
    if(sourceRegion) {
        region = *sourceRegion;
    } else {
        region.area = source.area;
        region.origin = Point{0, 0};
    }

    // Evaluate point on target to copy it to
    Point destination;
    if(targetDestination) {
        destination = *targetDestination;
    } else {
        destination = Point{0, 0};
    }

    return transmit("%c%u%u%u%u%u%u%u%u",
            Code::BLIT_SURFACE,
            source.ID,
            target.ID,
            region.origin.x,
            region.origin.y,
            region.area.width,
            region.area.height,
            destination.x,
            destination.y);
}

// Draws a point on a surface
Error drawPoint(const Surface &surface,
        const Point &point,
        const Colour colour) {
    
    return transmit("%c%u%u%u%c%c%c%c",
            Code::DRAW_POINT,
            surface.ID,
            point.x,
            point.y,
            colour.red,
            colour.green,
            colour.blue,
            colour.alpha);
}

// Draws a line on a surface
Error drawLine(const Surface &surface,
        const Line &line,
        const Colour colour) {
    
    return transmit("%c%u%u%u%u%u%c%c%c%c",
            Code::DRAW_LINE,
            surface.ID,
            line.start.x,
            line.start.y,
            line.end.x,
            line.end.y,
            colour.red,
            colour.green,
            colour.blue,
            colour.alpha);
}

// Draws a box on a surface
Error drawBox(const Surface &surface,
        const Box &box,
        const Colour colour,
        const bool fill) {
    
    return transmit("%c%u%u%u%u%u%c%c%c%c",
            Code::DRAW_BOX,
            surface.ID,
            box.origin.x,
            box.origin.y,
            box.area.width,
            box.area.height,
            colour.red,
            colour.green,
            colour.blue,
            colour.alpha);
}

// Renders a surface
Error render(const Surface &surface,
        const Box *sourceRegion, 
        const Point *targetDestination) {

    // Evaluate region of source to copy
    Box region;
    if(sourceRegion) {
        region = *sourceRegion;
    } else {
        region.area = surface.area;
        region.origin = Point{0, 0};
    }

    // Evaluate point in display to copy to
    Point destination;
    if(targetDestination) {
        destination = *targetDestination;
    } else {
        destination = Point{0, 0};
    }

    return transmit("%c%u%u%u%u%u%u%u",
            Code::RENDER,
            surface.ID,
            region.origin.x,
            region.origin.y,
            region.area.width,
            region.area.height,
            destination.x,
            destination.y);
}

// Clears the frame buffer
Error clear() {
    return transmit("%c", Code::CLEAR);
}

// Swaps the frame and active buffers
Error update() {
    return transmit("%c", Code::UPDATE);
}

// Soft-resets the board
Error reset() {
    return transmit("%c", Code::RESET);
}

}; // namespace HDMIShield