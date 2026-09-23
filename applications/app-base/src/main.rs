#![no_std]
#![no_main]

use core::panic::PanicInfo;

// i know this is cursed, shut up, stop complaining
#[panic_handler]
fn panic(_info: &PanicInfo) -> ! {
    loop {}
}

#[unsafe(no_mangle)]
pub extern "C" fn app_main() -> ! {
    loop {}
}