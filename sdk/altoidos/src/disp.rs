use crate::API;

pub struct Colour(pub f32, pub f32, pub f32);

pub enum HJust {
    Left,
    Centre,
    Right,
} 

pub enum VJust {
    Top,
    Centre,
    Bottom,
}

pub fn rgb565(c: Colour) -> u16 {
    let Colour(r, g, b) = c;

    let r = (r.clamp(0.0, 1.0) * 31.0) as u16;
    let g = (g.clamp(0.0, 1.0) * 63.0) as u16;
    let b = (b.clamp(0.0, 1.0) * 31.0) as u16;

    (r << 11) | (g << 5) | b
}

pub fn draw_text(
    x: i16, y: i16,
    colour: Colour,
    text: &str,
    h_just: HJust,
    v_just: VJust,
    size: u16,
) {
    let mut buffer = [0u8; 1024];
    
    let bytes = text.as_bytes();

    if bytes.len() >= buffer.len() {
        return;
    }

    buffer[..bytes.len()].copy_from_slice(bytes);
    buffer[bytes.len()] = 0;
    
    unsafe {
        ((*API).disp_draw_text)(
            x, y,
            rgb565(colour),
            buffer.as_ptr(),
            match h_just {
                HJust::Left => b"left\0".as_ptr(),
                HJust::Centre => b"centre\0".as_ptr(),
                HJust::Right => b"right\0".as_ptr(),
            },
            match v_just {
                VJust::Top => b"top\0".as_ptr(),
                VJust::Centre => b"centre\0".as_ptr(),
                VJust::Bottom => b"bottom\0".as_ptr(),
            },
            size,
        )
    }
}

pub fn draw_bmp(
    path: &str,
    x: i16, y: i16,
    scale: u8,
) {
    let mut buffer = [0u8; 256];
    
    let bytes = path.as_bytes();

    if bytes.len() >= buffer.len() {
        return;
    }

    buffer[..bytes.len()].copy_from_slice(bytes);
    buffer[bytes.len()] = 0;

    unsafe {
        ((*API).disp_draw_bmp)(
            buffer.as_ptr(),
            x, y,
            scale,
        )
    }
}