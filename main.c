#include <gb/gb.h>
#include <gbdk/incbin.h>
#if defined __INTELLISENSE__
#undef INCBIN
#define INCBIN(x,y)
#endif
#include "physics.h"

INCBIN(visualmap, "res/visualmap.tilemap")
INCBIN_EXTERN(visualmap)
INCBIN(visualmap_tiles, "res/visualmap_tiles.2bpp")
INCBIN_EXTERN(visualmap_tiles)

const unsigned char ball_sprite[] =
{
    0x3C,0x3C,0x4E,0x4E,0xDF,0xDF,0xFF,0xFF,
    0xFF,0xFF,0xFF,0xFF,0x7E,0x7E,0x3C,0x3C
};

uint8_t buttons_current = 0;
uint8_t buttons_previous = 0;

void main(void) {
    DISPLAY_ON;
    SHOW_SPRITES;
    SHOW_BKG;

    init_physics();

    set_bkg_data(0u, INCBIN_SIZE(visualmap_tiles) / 16, visualmap_tiles);
    set_bkg_tiles(0u, 0u, 32u, 32u, visualmap);

    set_sprite_data(0u, 1u, ball_sprite);


    // Loop forever
    while (1) {
        buttons_previous = buttons_current;
        buttons_current = joypad();

        if((buttons_current & J_UP) && !(buttons_previous & J_UP)) {
        apply_y_force(0xFF00);
        }else if((buttons_current & J_DOWN) && !(buttons_previous & J_DOWN)) {
        apply_y_force(0x0100);
        }else if((buttons_current & J_LEFT) && !(buttons_previous & J_LEFT)) {
        apply_x_force(0xFF00);
        }else if((buttons_current & J_RIGHT) && !(buttons_previous & J_RIGHT)) {
        apply_x_force(0x0100);
        }

        update_ball();
        
        move_sprite(0u, ball_pos.x.h + 8, ball_pos.y.h + 16);

        // Done processing, yield CPU and wait for start of next frame
        vsync();
    }
}
