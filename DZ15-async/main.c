#include <GL/glew.h>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

int window_width = 700;
int window_height = 700;
int fps = 30;

void reshape(const int width, const int height) {
    glEnable(GL_DEPTH_TEST);

    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-1, 1, -1, 1, 0, 10);

    window_width = width;
    window_height = height;
}

void draw() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    static int rotate_x = 0;
    static int rotate_y = 0;
    static int rotate_z = 0;
    rotate_x = (rotate_x) % 360 + 1;
    rotate_y = (rotate_y) % 360 + 1;
    rotate_z = (rotate_z) % 360 + 1;
    glRotatef((float)rotate_x, 1.0f, 0.0f, 0.0f);
    glRotatef((float)rotate_y, 0.0f, 1.0f, 0.0f);
    glRotatef((float)rotate_z, 0.0f, 0.0f, 1.0f);

    glBegin(GL_POLYGON);
    glColor3f(0.25f, 0.87f, 0.81f);
    glVertex3f(0.5f, -0.5f, -0.5f);
    glVertex3f(0.5f, 0.5f, -0.5f);
    glVertex3f(-0.5f, 0.5f, -0.5f);
    glVertex3f(-0.5f, -0.5f, -0.5f);
    glEnd();

    glBegin(GL_POLYGON);
    glColor3f(0.0f,  0.98f, 0.6f);
    glVertex3f(0.5f, -0.5f, 0.5f);
    glVertex3f(0.5f, 0.5f, 0.5f);
    glVertex3f(-0.5f, 0.5f, 0.5f);
    glVertex3f(-0.5f, -0.5f, 0.5f);
    glEnd();

    glBegin(GL_POLYGON);
    glColor3f(0.94f, 0.90f, 0.55f);
    glVertex3f(0.5f, -0.5f, -0.5f);
    glVertex3f(0.5f, 0.5f, -0.5f);
    glVertex3f(0.5f, 0.5f, 0.5f);
    glVertex3f(0.5f, -0.5f, 0.5f);
    glEnd();

    glBegin(GL_POLYGON);
    glColor3f(0.58f, 0.0f, 0.82f);
    glVertex3f(-0.5f, -0.5f, 0.5f);
    glVertex3f(-0.5f, 0.5f, 0.5f);
    glVertex3f(-0.5f, 0.5f, -0.5f);
    glVertex3f(-0.5f, -0.5f, -0.5f);
    glEnd();

    glBegin(GL_POLYGON);
    glColor3f(0.85f, 0.64f, 0.12f);
    glVertex3f(0.5f, 0.5f, 0.5f);
    glVertex3f(0.5f, 0.5f, -0.5f);
    glVertex3f(-0.5f, 0.5f, -0.5f);
    glVertex3f(-0.5f, 0.5f, 0.5f);
    glEnd();

    glBegin(GL_POLYGON);
    glColor3f(0.12f, 0.56f, 0.99f);
    glVertex3f(0.5f, -0.5f, -0.5f);
    glVertex3f(0.5f, -0.5f, 0.5f);
    glVertex3f(-0.5f, -0.5f, 0.5f);
    glVertex3f(-0.5f, -0.5f, -0.5f);
    glEnd();

    glFlush();
    glutSwapBuffers();
}

void timer() {
    glutPostRedisplay();
    glutTimerFunc(1000 / fps, timer, 0);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitWindowSize(window_width, window_height);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    glutCreateWindow("3D Cube");

    glutReshapeFunc(reshape);
    glutDisplayFunc(draw);
    glutTimerFunc(1000 / fps, timer, 0);
    glClearColor(0, 0, 0, 0);

    glutMainLoop();

    return 0;
}