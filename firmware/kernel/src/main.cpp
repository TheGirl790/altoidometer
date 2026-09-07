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
    } else if (v_just == "centre") {
        startY = y + charHeight / 2;
    } else if (v_just == "bottom") {
        startY = y - charHeight / 2;
    } else {
        throw std::invalid_argument("Invalid horizontal justification");
    }

    display.setCursor(startX, startY);
    display.setTextSize(size);
    display.setTextColor(colour);
    display.print(text);
}


void drawBMP(const char *filename, int16_t x, int16_t y, uint8_t scale) {
    File bmpFile = SD.open(filename);

    if (!bmpFile) {
        Serial.println("Could not open BMP");
        return;
    }

    if (bmpFile.read() != 'B' || bmpFile.read() != 'M') {
        Serial.println("Not a BMP file");
        bmpFile.close();
        return;
    }

    bmpFile.seek(10);

    uint32_t imageOffset;
    bmpFile.read((uint8_t *)&imageOffset, 4);

    bmpFile.seek(18);

    int32_t width;
    int32_t height;

    bmpFile.read((uint8_t *)&width, 4);
    bmpFile.read((uint8_t *)&height, 4);

    uint16_t planes;
    bmpFile.read((uint8_t *)&planes, 2);

    uint16_t bitsPerPixel;
    bmpFile.read((uint8_t *)&bitsPerPixel, 2);

    if (bitsPerPixel != 24) {
        Serial.println("Only 24-bit BMP supported");
        bmpFile.close();
        return;
    }

    bmpFile.seek(imageOffset);

    uint32_t rowSize = (width * 3 + 3) & ~3;

    uint8_t *rowBuffer = (uint8_t *)malloc(rowSize);

    if (!rowBuffer) {
        Serial.println("Not enough memory");
        bmpFile.close();
        return;
    }

    for (int32_t row = 0; row < height; row++) {

        int32_t actualRow = height - 1 - row;

        bmpFile.seek(imageOffset + actualRow * rowSize);
        bmpFile.read(rowBuffer, rowSize);

        for (int32_t col = 0; col < width; col++) {

            uint8_t b = rowBuffer[col * 3 + 0];
            uint8_t g = rowBuffer[col * 3 + 1];
            uint8_t r = rowBuffer[col * 3 + 2];

            uint16_t colour = display.color565(r, g, b);

            display.fillRect(
                x + col * scale,
                y + row * scale,
                scale,
                scale,
                colour
            );
        }
    }

    free(rowBuffer);
    bmpFile.close();
}


void setup()
{
    Serial.begin(115200);
    
    display.begin();
    display.setRotation(3);

    uint8_t madctl = display.readcommand8(ILI9341_RDMADCTL);

    display.fillScreen(ILI9341_BLACK);

    if (!SD.begin(SD_CS)) {
        drawText(160, 120, ILI9341_RED, "SD Init Failed!", "centre", "centre", 2);
        drawText(160, 135, ILI9341_RED, "Ensure that the SD card is inserted correctly.", "centre", "centre");
        return;
    }
    
    // Draw boot screen
    drawText(160, 70, ILI9341_WHITE, "AltoidOS", "centre", "bottom", 2);
    drawBMP("/bootimg.bmp", 128, 90, 2);
}

void loop()
{

}