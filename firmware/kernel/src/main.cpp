#include <Arduino.h>
#include <SPI.h>
#include <FS.h>
#include <SD.h>

#include <string>
#include <stdexcept>

#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>

// display pins
#define TFT_CS  5
#define TFT_DC  2
#define TFT_RST 4

// SD pins
#define SD_CS 32

Adafruit_ILI9341 display(TFT_CS, TFT_DC, TFT_RST);

// Single function to draw text on the display using a variety of parameters
void drawText(int16_t x, int16_t y, uint16_t colour, String text, String h_just="left", String v_just="bottom", uint16_t size=1) {
    uint16_t startX;
    uint16_t startY;

    int charWidth  = size * 6;
    int charHeight = size * 8;

    if (h_just == "left") {
        startX = x;
    } else if (h_just == "centre") {
        startX = x - text.length() * charWidth / 2;
    } else if (h_just == "right") {
        startX = x + text.length() * charWidth / 2;
    } else {
        throw std::invalid_argument("Invalid horizontal justification");
    }

    if (v_just == "top") {
        startY = y;
    } else if (h_just == "centre") {
        startY = y - charHeight / 2;
    } else if (h_just == "bottom") {
        startY = y + charHeight / 2;
    } else {
        throw std::invalid_argument("Invalid horizontal justification");
    }

    display.setCursor(startX, startY);
    display.setTextSize(size);
    display.setTextColor(colour);
    display.print(text);
}


void setup()
{
    Serial.begin(115200);
    
    display.begin();
    display.setRotation(3);

    display.fillScreen(ILI9341_BLACK);

    if (!SD.begin(SD_CS)) {
        drawText(160, 120, ILI9341_RED, "SD Init Failed!", "centre", "centre", 2);
        drawText(160, 135, ILI9341_RED, "Ensure that the SD card is inserted correctly.", "centre", "centre");
        return;
    }
    
    drawText(160, 120, ILI9341_WHITE, "AltoidOS", "centre", "centre", 2);
}

void loop()
{

}