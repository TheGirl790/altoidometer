#pragma once

#include <stdint.h>

struct AltoidOSApi {
    uint32_t abi_version;
    uint32_t struct_size;

    void (*disp_draw_text)(
        int16_t x, int16_t y,
        uint16_t colour,
        const char *text,
        const char *h_just,
        const char *v_just,
        uint16_t size
    );

    void (*disp_draw_bmp)(
        const char *path,
        int16_t x, int16_t y,
        uint8_t scale
    );
};