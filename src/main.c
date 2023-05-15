#define LOG_LVL LOG_LVL_INFO
#include "util.h"
#include "debug_draw.h"
#include "physic.h"

#include <GL/freeglut.h>
#include <windows.h>

#define WINDOW_H 420
#define WINDOW_W 420
#define WORLD_W_M 10
#define WORLD_H_M 10
#define FPS 60
#define FRAME_PERIOD_MS (1000/FPS)

struct physic_world world;

void update(int unused);
void display_func(void);

int main(int argc, char *argv[])
{
    logi("Start");

    logi("Init physic");
    try_do(physic_init_world(&world));
    struct physic_debug_drawer_api draw_api = {
        .box = draw_box,
        .obj = draw_obj
    };
    try_do(physic_set_debug_draw_api(&world, &draw_api));
    try_do(physic_set_debug_draw_flag(&world, PDDF_OBJECTS));
    struct physic_object_description ground = {
        .mass = PHYSIC_INFINITY_MASS,
        .pos_x = 5,
        .pos_y = 1,
        .point_arr = (struct point2f[]) {
            { -4,  0.5 },
            {  4,  0.5 },
            {  4, -0.5 },
            { -4, -0.5 }
        },
        .points_count = 4,
    };
    try_do(-1 == physic_add_object(&world, &ground));
    struct physic_object_description box = {
        .mass = 1,
        .pos_x = 5,
        .pos_y = 7,
        .point_arr = (struct point2f[]) {
            { -0.5,  0.5 },
            {  0.5,  0.5 },
            {  0.5, -0.5 },
            { -0.5, -0.5 }
        },
        .points_count = 4,
    };
    int od;
    try_do(-1 == (od = physic_add_object(&world, &box)));
    try_do(physic_object_apply_impulse(&world, od, &(struct point2f) { 0, -1 }));

    logi("Init physic: ok");

    logi("Init openGL");
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(WINDOW_H, WINDOW_W);
    glutCreateWindow("test");
    glutDisplayFunc(display_func);
    glOrtho(0, WORLD_W_M, 0, WORLD_H_M, 0, 1);
    logi("Init openGL: ok");

    glutTimerFunc(FRAME_PERIOD_MS, update, 0);

    logi("Start main loop");
    glutMainLoop();
    return 0;
}

void update(int unused)
{
    (void)unused;
        try_do_exit(physic_update(&world, FRAME_PERIOD_MS / 1000.f));
    glutPostRedisplay();
    glutTimerFunc(FRAME_PERIOD_MS, update, 0);
}

void display_func(void)
{
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);
        try_do_exit(physic_debug_draw(&world));
    glutSwapBuffers();
}
