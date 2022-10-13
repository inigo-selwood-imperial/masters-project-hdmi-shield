# HDMI Generator Shield for Arduino

#### Inigo Selwood | Master's Project 2022-2023

## Goal

To create a shield for the Arduino Uno capable of driving HDMI displays. The shield will be able to render rasterized text and images, and draw simple vector graphics.

## How it Works

The shield is a PCB that plugs directly into the Arduino Uno. By connecting a display to its HDMI port, the Arduino can be programmed to generate text and graphics. The FPGA can be flashed via the USB-C UART interface. 

The [Arduino API](arduino/) provides a rich feature set for the programmer to use in their graphics applications.

This library is a wrapped for the hardware API, documented [here](#api)

#### Assets

Large assets like font files and images that wouldn't fit in the Arduino's flash memory can be loaded onto the shield from PC via the SD card slot. These assets can then and manipulated from the Arduino to dynamically generate text and display pictures.

#### Surfaces

Surfaces, as referred to in this project, are analogous to textures in traditional graphics hardware. They are persistent and reusable assets which can be drawn on, transformed, blitted, and ultimately rendered on the display.



## API

![Timing Diagram](assets/timing-diagram.png)