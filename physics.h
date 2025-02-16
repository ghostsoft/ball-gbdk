#ifndef PHYSICS_H
#define PHYSICS_H

#include <gb/gb.h>

#define GRAVITY 0x000A
#define BALL_INIT_POS_X 17u
#define BALL_INIT_POS_Y 75u
#define TIMESTEP 2u
#define TIMESTEPDIV 0x0080 // 1/timestep = 1/2 = 0.5 = 0x0080

typedef struct { fixed x; fixed y; } fixed2;

extern fixed2 ball_pos;

inline int16_t mul_q88(int16_t a, int16_t b);
inline int16_t mul_q88_noround(int16_t a, int16_t b);
void update_ball(void);
void init_physics(void);
inline uint8_t get_collision_byte(uint8_t x, uint8_t y);
inline int16_t vdotn(void);

void apply_y_force(int16_t force);
void apply_x_force(int16_t force);

#define POINT0_X 5u
#define POINT0_Y 7u
#define POINT1_X 7u
#define POINT1_Y 5u
#define POINT2_X 7u
#define POINT2_Y 2u
#define POINT3_X 5u
#define POINT3_Y 0u
#define POINT4_X 2u
#define POINT4_Y 0u
#define POINT5_X 0u
#define POINT5_Y 2u
#define POINT6_X 0u
#define POINT6_Y 5u
#define POINT7_X 2u
#define POINT7_Y 7u


#endif