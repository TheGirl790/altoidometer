#![no_std]
#![no_main]

use core::panic::PanicInfo;
use altoidos::disp;

// i know this is cursed, shut up, stop complaining
#[panic_handler]
fn panic(_info: &PanicInfo) -> ! {
    loop {}
}

#[unsafe(no_mangle)]
pub extern "C" fn app_setup() {
    disp::draw_text(
        0, 0,
        disp::Colour(0.0, 1.0, 0.0),
        "This is an app!",
        disp::HJust::Left,
        disp::VJust::Top,
        2,
    )
}

#[unsafe(no_mangle)]
pub extern "C" fn app_loop() {
    
}