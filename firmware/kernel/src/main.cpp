#include <Arduino.h>
#include <SPI.h>
#include <FS.h>
#include <SD.h>

#include <string>
#include <stdexcept>

#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>

#include <esp_partition.h>

// display pins
#define TFT_CS  5
#define TFT_DC  2
#define TFT_RST 4

// SD pins
#define SD_CS 32

// AAP header checks
#define AAP_MAGIC 0x534F5441
#define AAP_SUP_VERSION 1

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
        startY = y - charHeight / 2;
    } else if (v_just == "bottom") {
        startY = y - charHeight;
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

struct __attribute__((packed)) AapHeader {
    uint32_t magic;
    uint16_t version;
    uint32_t entry_point;
    uint32_t program_size;
};

enum class LoadError {
    None,
    FileNotFound,
    HeaderReadFailed,
    InvalidMagic,
    UnsupportedVersion,
    InvalidSize,
    FlashWriteFailed,
    NoAppPartition,
    ProgramReadFailed
};

struct LoadResult {
    bool success;
    LoadError error;
};

void disp_load_error(LoadError error) {
    String errText;

    switch (error) {
        case LoadError::FileNotFound:
            errText = "App load failed!\n\nCould not find app binary.\nThe app may have been installed incorrectly.";
            break;

        case LoadError::HeaderReadFailed:
            errText = "App load failed!\n\nFailed to read app header.\nThe app binary may be formatted incorrectly.";
            break;

        case LoadError::InvalidMagic:
            errText = "App load failed!\nThis is not an AltoidOS app.";
            break;

        case LoadError::UnsupportedVersion:
            errText = "App load failed!\n\nUnsupported app version.\nTry updating this app.";
            break;

        case LoadError::InvalidSize:
            errText = "App load failed!\n\nProgram size does not match specification.\nThe app binary may be formatted incorrectly.";
            break;

        case LoadError::FlashWriteFailed:
            errText = "App load failed!\n\nFailed to write this app to flash.\n";
            break;

        case LoadError::NoAppPartition:
            errText = "App load failed!\n\nApp partition not found.\nThis kernel was flashed incorrectly.";
            break;

        case LoadError::ProgramReadFailed:
            errText = "App load failed!\n\nCould not read program from file.";
            break;
    }

    drawText(
        0, 0,
        ILI9341_RED,
        errText,
        "left", "top",
        1
    );
}

// all-in-one function to verify and load an app into flash
LoadResult load_app(const char* path) {
    File file = SD.open(path, FILE_READ);

    // Check if file loaded correctly
    if (!file) {
        return LoadResult {
            false,
            LoadError::FileNotFound
        };
    }

    AapHeader header;

    // Try to read the header
    if (file.read((uint8_t *)&header, sizeof(header)) != sizeof(header)) {
        file.close();
        return LoadResult {
            false,
            LoadError::HeaderReadFailed
        };
    }

    // Check magic and verison
    if (header.magic != AAP_MAGIC) {
        return LoadResult {
            false,
            LoadError::InvalidMagic
        };
    }

    if (header.version != AAP_SUP_VERSION) {
        return LoadResult {
            false,
            LoadError::UnsupportedVersion
        };
        
    }

    // Check that program size matches what the header says it should be
    if (file.size() - sizeof(AapHeader) != header.program_size) {
        return LoadResult {
            false,
            LoadError::InvalidSize
        };
    }

    // Check partition and load the app program
    const esp_partition_t *app_partition =
        esp_partition_find_first(
            ESP_PARTITION_TYPE_APP,
            ESP_PARTITION_SUBTYPE_APP_OTA_0,
            nullptr
        );

    if (app_partition == nullptr) {
        return LoadResult {
            false,
            LoadError::NoAppPartition
        };
    }

    // erase partition
    esp_err_t err = esp_partition_erase_range(
        app_partition,
        0,
        app_partition->size
    );

    if (err != ESP_OK) {
        return LoadResult {
            false,
            LoadError::FlashWriteFailed
        };
    }

    // read file and write to partition
    uint8_t buffer[4096];
    size_t remaining = header.program_size;
    size_t offset = 0;

    while (remaining > 0) {
        size_t chunk_size = min(remaining, sizeof(buffer));

        if (file.read(buffer, chunk_size) != chunk_size) {
            return LoadResult {
                false,
                LoadError::ProgramReadFailed
            };
        }

        if (esp_partition_write(
                app_partition,
                offset,
                buffer,
                chunk_size
            ) != ESP_OK) {
            return LoadResult {
                false,
                LoadError::FlashWriteFailed
            };
        }

        offset += chunk_size;
        remaining -= chunk_size;
    }

    return LoadResult {
        true,
        LoadError::None
    };
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
    
    // Draw boot screen
    drawText(160, 85, ILI9341_WHITE, "AltoidOS", "centre", "bottom", 2);
    drawBMP("/bootimg.bmp", 128, 90, 2);

    // Load launcher app
    LoadResult result = load_app("/apps/launcher/app.aap");

    if (!result.success) {
        disp_load_error(result.error);
    }
}

void loop()
{
    
}