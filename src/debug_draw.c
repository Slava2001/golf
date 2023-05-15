#define LOG_LVL LOG_LVL_ERROR
#include "util.h"
#include "debug_draw.h"
#include "physic.h"

#include <GL/freeglut.h>
#include <windows.h>

#define LINE_WIDTH 1

void draw_box(float x, float y, float h, float w)
{
    log_enter();
    logd("x: %f, y: %f, h: %f, w: %f", x, y, h, w);
    glLineWidth(LINE_WIDTH);
    glBegin(GL_LINE_LOOP);
        glColor3f(1, 1, 1);
        glVertex2f(x - w / 2, y - h / 2);
        glVertex2f(x - w / 2, y + h / 2);
        glVertex2f(x + w / 2, y + h / 2);
        glVertex2f(x + w / 2, y - h / 2);
    glEnd();
}

void draw_obj(const struct point2f *center, const struct point2f *points, unsigned points_count)
{
    log_enter();
    glLineWidth(LINE_WIDTH);
    glPushMatrix();
        glTranslatef(center->x, center->y, 0);
        glBegin(GL_LINE_LOOP);
            glColor3f(1, 1, 1);
            for (unsigned i = 0; i < points_count; i++) {
                glVertex2f(points[i].x, points[i].y);
            }
        glEnd();
    glPopMatrix();
}
