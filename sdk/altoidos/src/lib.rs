#![no_std]

#[repr(C)]
pub struct AltoidOSApi {
    pub abi_version: u32,
    pub struct_size: u32,

    pub disp_draw_text: unsafe extern "C" fn(
        x: i16, y: i16,
        colour: u16,
        text: *const u8,
        h_just *const u8,
        v_just *const u8,
        size: u16,
    ),

    pub disp_draw_bmp: unsafe extern "C" fn(
        path: *const u8,
        x: i16, y: i16,
        size: u8
    )
}

static mut API: *const AltoidOSApi = core::ptr::null();

pub unsafe fn init(api: *const AltoidOSApi) {
    API = api;
}

pub mod disp;