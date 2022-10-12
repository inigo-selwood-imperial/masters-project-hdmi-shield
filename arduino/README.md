# Arduino Library

This library implements an interface for communicating with the HDMI shield from your Arduino.

---

## System

#### Initialize

Configures the Arduino's I/O to communicate with the board, and asserts that the board is functioning properly

```c++
HDMIShield::Error error = HDMIShield::initialize();
if(error) {
    Serial.println(HDMIShield::errorMessage(error));
    return 1;
}
```

#### Reset

If a fatal error has occured, the board can be soft-reset

```c++
HDMIShield::Error error = HDMIShield::reset();
if(error) {
    Serial.println(HDMIShield::errorMessage(error));
    return 1;
}
```

---

## Display Metadata

#### Display Connected

Checks whether an HDMI display is currently plugged into the shield

```c++
bool connected;
HDMIShield::Error error = HDMIShield::displayConnected(connected);
if(error) {
    Serial.println(HDMIShield::errorMessage(error));
    return 1;
}

Serial.printf("display connected? %s", connected ? "true" : "false");
```

---

## Font Handling

#### Load Font

Fonts are stored on the board's SD card. To render text using them, they need to be loaded into RAM. 

```c++
HDMIShield::Font font;
HDMIShield::Error error = HDMIShield::loadFont(font, "font-name.pcf", 12);
if(error) {
    Serial.println(HDMIShield::errorMessage(error));
    return 1;
}
```

#### Free Font 

When you're finished using a font, it can be evicted from the cache by freeing it

```c++
HDMIShield::Error error = HDMIShield::freeFont(font);
if(error) {
    Serial.println(HDMIShield::errorMessage(error));
    return 1;
}
```

---

## Surface Manipulation

#### Create Surface

Before a surface can be manipulated, it has to be allocated in the board's cache.

```c++
HDMIShield::Surface surface;
HDMISheild::Rectangle area {
    .width(256),
    .height(192),
};
HDMIShield::Error error = HDMIShield::createSurface(surface, area);
if(error) {
    Serial.println(HDMIShield::errorMessage(error));
    return 1;
}
```

#### Create Text Surface

Displaying text is done by creating a surface from a font file and a string.

```c++
const char *text = "lorem ipsum dolor sit amet"
HDMIShield::Colour colour {
    .red(255),
    .green(255),
    .blue(255),
    .alpha(255),
};

HDMIShield::Surface surface;
HDMIShield::Error error = HDMIShield::createSurface(surface, 
        font, 
        text,
        colour);
if(error) {
    Serial.println(HDMIShield::errorMessage(error));
    return 1;
}
```

#### Blit Surface

Copying surfaces (or parts of surfaces) is accomplished using boolean arithmetic called "blitting". Transparency is preserved. Regions of the source surface, as well as a point on the target surface, can be specified.

If the region doesn't fully intersect the source surface, the out-of-bounds regions are ignored. 

```c++
HDMIShield::Box region {
    origin.x(16),
    origin.y(32),
    area.width(64),
    area.height(128),
};

HDMIShield::Point destination {
    x(0),
    y(0),
};

HDMIShield::Error error = HDMIShield::blitSurface(source,
        target,
        &region,
        &destination);
if(error) {
    Serial.println(HDMIShield::errorMessage(error));
    return 1;
}
```

#### Free Surface

When done using a surface, it can be evicted from the cache by freeing it

```c++
HDMIShield::Error error = HDMIShield::freeSurface(surface);
if(error) {
    Serial.println(HDMIShield::errorMessage(error));
    return 1;
}
```

---

## Naive Draw Functions

#### Draw Point

Draws a single pixel on a surface

```c++
HDMIShield::Point point {
    .x(32),
    .y(32),
};

HDMIShield::Colour colour {
    .red(255),
    .green(255),
    .blue(255),
    .alpha(255),
};

HDMIShield::Error error = HDMIShield::drawPoint(surface, point, colour);
if(error) {
    Serial.println(HDMIShield::errorMessage(error));
    return 1;
}
```

#### Draw Line

Draws a line between two points on a surface

```c++
HDMIShield::Line line {
    .start.x(0),
    .start.y(0),
    .end.x(1920),
    .end.y(1080),
};

HDMIShield::Colour colour {
    .red(255),
    .green(255),
    .blue(255),
    .alpha(255),
};

HDMIShield::Error error = HDMIShield::drawLine(surface, line, colour);
if(error) {
    Serial.println(HDMIShield::errorMessage(error));
    return 1;
}
```

#### Draw Box

Draws a box on a surface, with an optional fill

```c++
HDMIShield::Box box {
    .origin.x(0),
    .origin.y(0),
    .area.width(256),
    .area.height(192),
};

HDMIShield::Colour colour {
    .red(255),
    .green(255),
    .blue(255),
    .alpha(255),
};

HDMIShield::Error error = HDMIShield::drawBox(surface, box, colour, fill);
if(error) {
    Serial.println(HDMIShield::errorMessage(error));
    return 1;
}
```

---

## Display Functions

#### Render

Video memory is split into active and buffer regions. Rendering copies to the buffer. When all surfaces that need to be have been rendered, the `update` function can be called to swap these buffers.

```c++
HDMIShield::Box region {
    origin.x(0),
    origin.y(0),
    area.width(surface.area.width),
    area.height(surface.area.height),
};

HDMIShield::Point destination {0, 0};

HDMIShield::Error error = HDMIShield::render(surface,
        &region,
        &destination);
if(error) {
    Serial.println(HDMIShield::errorMessage(error));
    return 1;
}
```

#### Update

When all surfaces have been rendered, the update function presents the buffered frame to the display.

```c++
HDMIShield::Error error = HDMIShield::update(surface);
if(error) {
    Serial.println(HDMIShield::errorMessage(error));
    return 1;
}
```