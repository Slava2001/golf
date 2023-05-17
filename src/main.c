#define LOG_LVL LOG_LVL_INFO
#include "util.h"
#include "debug_draw.h"
#include "physic.h"

#include <GL/freeglut.h>
#include <windows.h>
#include <math.h>

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
        .aabb = draw_aabb,
        .box = draw_box,
        .circle = draw_circle
    };
    try_do(physic_set_debug_draw_api(&world, &draw_api));
    try_do(physic_set_debug_draw_flag(&world, PDDF_AABB | PDDF_OBJECTS));

    struct physic_object_description obj_desc = {
        .pos_x = 5,
        .pos_y = 5,
        .angle = 0,
        .mass = 1,
        .speed_a = M_PI/5
    };
    int od = physic_add_object(&world, &obj_desc);
    err_if(od == -1, "Failed to add object in world");

    struct physic_primitive_description primitive_circle_desc = {
        .type = PPT_CIRCLE,
        .offset_x = 1,
        .offset_y = 0,
        .angle = M_PI_2,
        .r = 1
    };
    try_do(-1 == physic_add_primitive(&world, od, &primitive_circle_desc));
    primitive_circle_desc.offset_x = -2;
    primitive_circle_desc.r = 2;
    try_do(-1 == physic_add_primitive(&world, od, &primitive_circle_desc));
    struct physic_primitive_description primitive_rectangle_desc = {
        .type = PPT_RECTANGLE,
        .offset_x = 0,
        .offset_y = 0,
        .angle = 0,
        .h = 4,
        .w = 0.5
    };
    try_do(-1 == physic_add_primitive(&world, od, &primitive_rectangle_desc));

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
    glutSwapBuffers();

    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);
    try_do_exit(physic_debug_draw(&world));
}
