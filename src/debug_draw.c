#define LOG_LVL LOG_LVL_ERROR
#include "util.h"
#include "debug_draw.h"
#include "physic.h"

#include <GL/freeglut.h>
#include <windows.h>
#include <math.h>

void draw_rectangle(float x, float y, float h, float w, float a) {
    glPushMatrix();
        glTranslatef(x, y, 0);
        glRotatef(a / M_PI * 180, 0, 0, 1);
        glBegin(GL_LINE_LOOP);
            glVertex2f(-w / 2, -h / 2);
            glVertex2f(-w / 2, +h / 2);
            glVertex2f(+w / 2, +h / 2);
            glVertex2f(+w / 2, -h / 2);
        glEnd();
    glPopMatrix();
}

#define AABB_LINE_WIDTH 1
void draw_aabb(float x, float y, float h, float w, bool is_collided)
{
    log_enter();
    logd("x: %f, y: %f, h: %f, w: %f", x, y, h, w);
    glLineWidth(AABB_LINE_WIDTH);
    if (is_collided) {
        glColor3f(1, 0, 0);
    } else {
        glColor3f(0, 1, 0);
    }
    draw_rectangle(x, y, h, w, 0);
}

#define OBJ_LINE_WIDTH 2
void draw_box(float x, float y, float h, float w, float a)
{
    log_enter();
    logd("x: %f, y: %f, h: %f, w: %f, a: %f", x, y, h, w, a);
    glLineWidth(OBJ_LINE_WIDTH);
    glColor3f(1, 1, 1);
    draw_rectangle(x, y, h, w, a);
    glBegin(GL_LINE_LOOP);
        glVertex2f(x, y);
        glVertex2f(x + w / 2 * cos(a), y + w / 2 * sin(a));
    glEnd();
}

void draw_circle(float x, float y, float r, float a)
{
    log_enter();
    logd("x: %f, y: %f, r: %f, a: %f", x, y, r, a);
    glLineWidth(OBJ_LINE_WIDTH);
    glBegin(GL_LINE_LOOP);
        glColor3f(1, 1, 1);
        glVertex2f(x, y);
        float step = (2 * M_PI) / 16;
        for (float i = 0; i < 2 * M_PI + step; i += step) {
            glVertex2f(x + r * cos(a+i), y + r * sin(a+i));
        }
    glEnd();
}
