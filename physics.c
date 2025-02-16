#include "physics.h"
#include <gb/gb.h>
#include <stdio.h>
#include <gbdk/incbin.h>
#if defined __INTELLISENSE__
#undef INCBIN
#define INCBIN(x,y)
#endif
#include <gbdk/emu_debug.h>

INCBIN(normals_x, "res/normals_x.bin")
extern const int16_t normals_x[];
INCBIN(normals_y, "res/normals_y.bin")
extern const int16_t normals_y[];
INCBIN(collision_bitmap, "res/collision_bitmap.bin")
INCBIN_EXTERN(collision_bitmap)

fixed2 ball_pos;
fixed2 ball_vel;

fixed2 desired_pos;

fixed2 divided_velocity;

uint8_t points_x[8] = {5u, 7u, 7u, 5u, 2u, 0u, 0u, 2u};
uint8_t points_y[8] = {7u, 5u, 2u, 0u, 0u, 2u, 5u, 7u};

inline int16_t mul_q88(int16_t a, int16_t b) {
    int32_t temp = (int32_t)a * (int32_t)b;
    return (temp >> 8) + ((temp >> 7) & 1);
}

inline int16_t mul_q88_noround(int16_t a, int16_t b) {
    return ((int32_t)a * (int32_t)b) >> 8;
}

void init_physics(void) {
    ball_pos.x.h = BALL_INIT_POS_X;
    ball_pos.y.h = BALL_INIT_POS_Y;

    ball_vel.y.w = 0L;
    ball_vel.x.w = 0L;
    ball_vel.x.l = 0xF5;
    ball_vel.y.l = 0x85;
}

uint16_t collision_index = 0;
uint8_t collision_byte = 0;
uint8_t offset_into_byte = 0;
uint8_t collision_mask = 0;

fixed2 collision_normal;

int16_t mul_q88_asm(int16_t a, int16_t b); // in mul_q88_asm.s

void update_ball(void) {
    ball_vel.y.w += GRAVITY;

    divided_velocity.x.w = mul_q88_asm(ball_vel.x.w, TIMESTEPDIV);
    divided_velocity.y.w = mul_q88_asm(ball_vel.y.w, TIMESTEPDIV);

    //uint8_t counter = TIMESTEP;
    for (uint8_t c = TIMESTEP; c != 0; c--) {
        desired_pos.x.w = ball_pos.x.w + divided_velocity.x.w;
        desired_pos.y.w = ball_pos.y.w + divided_velocity.y.w;

        //EMU_printf("current position: %d,%d desired position: %d,%d \n", ball_pos.x.h, ball_pos.y.h, desired_pos.x.h, desired_pos.y.h);

        collision_mask = 0u;
        //the sicko part
        for (int8_t i = 7; i >= 0; i--) {
            collision_byte = get_collision_byte(desired_pos.x.h + points_x[i], desired_pos.y.h + points_y[i]);
            offset_into_byte = 7 - ((desired_pos.x.h + points_x[i]) % 8u);
            if ((collision_byte & (1 << offset_into_byte)) > 0) { // (byte & offset_into_byte) > 0
                collision_mask += 1u;
                //EMU_printf("point %d at position %d,%d read collision byte %hx with offset %d \n", i, desired_pos.x.h + points_x[i], desired_pos.y.h + points_y[i], (unsigned char)collision_byte, offset_into_byte);
            }
            if(i != 0) { collision_mask = collision_mask << 1u; }
        }

        if (collision_mask != 0) {
            // collision response
            collision_normal.x.w = normals_x[collision_mask]; // guess what these are stored little endian
            uint8_t tmp = collision_normal.x.h; // but loading them in as a full 16-bit word means they stay that way
            collision_normal.x.h = collision_normal.x.l; // now all the integers and fractions are flipped
            collision_normal.x.l = tmp; // it's terrible and i'm just swapping the high and low byte here
            collision_normal.y.w = normals_y[collision_mask]; // as a lil hack to avoid having to think any more about this
            tmp = collision_normal.y.h;
            collision_normal.y.h = collision_normal.y.l;
            collision_normal.y.l = tmp;
            //EMU_printf("collision normal at offset %hx is X:%hx%hx, Y:%hx%hx \n", (unsigned char) collision_mask, (unsigned char) collision_normal.x.h, (unsigned char) collision_normal.x.l, (unsigned char) collision_normal.y.h, (unsigned char) collision_normal.y.l);
            
            int16_t v_dot_n = vdotn();
            fixed2 u;
            u.x.w = mul_q88_asm(collision_normal.x.w, v_dot_n);
            u.y.w = mul_q88_asm(collision_normal.y.w, v_dot_n);
            fixed2 w;
            w.x.w = ball_vel.x.w - u.x.w;
            w.y.w = ball_vel.y.w - u.y.w;

            ball_vel.x.w = w.x.w - mul_q88_asm(u.x.w, (int16_t)0x0080);
            ball_vel.y.w = w.y.w - mul_q88_asm(u.y.w, (int16_t)0x0080);

            divided_velocity.x.w = mul_q88_asm(ball_vel.x.w, TIMESTEPDIV);
            divided_velocity.y.w = mul_q88_asm(ball_vel.y.w, TIMESTEPDIV);
            desired_pos.x.w = ball_pos.x.w + divided_velocity.x.w;
            desired_pos.y.w = ball_pos.y.w + divided_velocity.y.w;

            desired_pos.x.w -= collision_normal.x.w;
            desired_pos.y.w -= collision_normal.y.w;
        }

        ball_pos.x.w = desired_pos.x.w;
        ball_pos.y.w = desired_pos.y.w;
    }
}

inline uint8_t get_collision_byte(uint8_t x, uint8_t y) {
    return collision_bitmap[(uint16_t)(y*20u) + (uint16_t)(x >> 3)];
}

inline int16_t vdotn(void) {
    return mul_q88_asm(ball_vel.x.w, collision_normal.x.w) + mul_q88_asm(ball_vel.y.w, collision_normal.y.w);
}

void apply_x_force(int16_t force) {
    ball_vel.x.w += force;
}

void apply_y_force(int16_t force) {
    ball_vel.y.w += force;
}