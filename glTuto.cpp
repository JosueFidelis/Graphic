#include <GL/gl.h>
#include <GL/glut.h>


double translate_x = 0, translate_y = 0, translate_z = 0, rotate_x = 0, rotate_y = 0, rotate_z = 0;

void drawCube() {
    glBegin(GL_QUADS);
        //front
        glColor3f(1.0, 0.0, 0.0);
        glVertex3f(1.0, 1.0, -2.0);
        glVertex3f(-1.0, 1.0, -2.0);
        glVertex3f(-1.0, -1.0, -2.0);
        glVertex3f(1.0, -1.0, -2.0);

        //back
        glColor3f(0.0, 0.0, 1.0);
        glVertex3f(1.0, 1.0, -4.0);
        glVertex3f(-1.0, 1.0, -4.0);
        glVertex3f(-1.0, -1.0, -4.0);
        glVertex3f(1.0, -1.0, -4.0);

        //left
        glColor3f(0.0, 1.0, 0.0);
        glVertex3f(-1.0, 1.0, -2.0);
        glVertex3f(-1.0, 1.0, -4.0);
        glVertex3f(-1.0, -1.0, -4.0);
        glVertex3f(-1.0, -1.0, -2.0);

        //right
        glColor3f(1.0, 1.0, 0.0);
        glVertex3f(1.0, 1.0, -2.0);
        glVertex3f(1.0, -1.0, -2.0);
        glVertex3f(1.0, -1.0, -4.0);
        glVertex3f(1.0, 1.0, -4.0);

        //top
        glColor3f(1.0, 1.0, 1.0);
        glVertex3f(-1.0, 1.0, -2.0);
        glVertex3f(1.0, 1.0, -2.0);
        glVertex3f(1.0, 1.0, -4.0);
        glVertex3f(-1.0, 1.0, -4.0);

        //bot
        glColor3f(0.5, 0.5, 1.0);
        glVertex3f(-1.0, -1.0, -2.0);
        glVertex3f(-1.0, -1.0, -4.0);
        glVertex3f(1.0, -1.0, -4.0);
        glVertex3f(1.0, -1.0, -2.0);
    glEnd();
}

void display () {

    glPushMatrix();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glRotatef(rotate_x, 1.0, 0.0, 0.0);
    glRotatef(rotate_y, 0.0, 1.0, 0.0);
    glRotatef(rotate_z, 0.0, 0.0, 1.0);

    glTranslatef(translate_x, 0, 0);
    glTranslatef(0, translate_y, 0);
    glTranslatef(0, 0, translate_z);
    drawCube();
    
    glPopMatrix();

    glutSwapBuffers();
}

void key (int key, int x, int y) {

    //translação
    if (key == GLUT_KEY_RIGHT) {
        translate_x -= 1;
    } else if (key == GLUT_KEY_LEFT) {
        translate_x += 1;
    }
    if (key == GLUT_KEY_UP) {
        translate_y -= 1;
    } else if (key == GLUT_KEY_DOWN) {
        translate_y += 1;
    }
    if (key == GLUT_KEY_F1) {
        translate_z -= 1;
    } else if (key == GLUT_KEY_F2) {
        translate_z += 1;
    }

    //rotação
    if (key == GLUT_KEY_F4) {
        rotate_y += 5;
    } else if (key == GLUT_KEY_F5) {
        rotate_y -= 5;
    }
    if (key == GLUT_KEY_F8) {
        rotate_x += 5;
    } else if (key == GLUT_KEY_F9) {
        rotate_x -= 5;
    }
    if (key == GLUT_KEY_F11) {
        rotate_z += 5;
    } else if (key == GLUT_KEY_F12) {
        rotate_z -= 5;
    } 

    glutPostRedisplay();
}

void reshape (int w, int h) {
    glViewport(0, 0, w, h);
}

int main (int argc, char * argv[]) {

    glutInit(&argc, argv);

    glutInitWindowSize(500,500);
    glutInitWindowPosition(0,0);
    
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH); //
    glutCreateWindow("Objeto"); //
    glEnable(GL_DEPTH_TEST); //

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutSpecialFunc(key);

    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60, 1, 1, 20);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(-4.0, 5.0, 5.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

    glutMainLoop();
    return 0;
} 