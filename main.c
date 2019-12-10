#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>
#include <math.h>


/* Deklaracije callback funkcija. */
static void on_keyboard(unsigned char key, int x, int y);
static void on_display(void);
void drawCoord();
void drawPlane();

double headX;
double headY;
double headZ;

double atX;
double atY;
double atZ;

double planeLength = 40;
double planeWidth = 10;
double planeHeight = 0.3;

double marginWidth = planeWidth/5;



int main(int argc, char **argv)
{

    headX = 0;
    headY = 15;
    headZ = 20;

    atX = 0;
    atY = 0;
    atZ = 0;
    /* Inicijalizuje se GLUT. */
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);

    /* Kreira se prozor. */

    glutInitWindowSize(1000, 1000);
    glutInitWindowPosition(100, 100);
    glutCreateWindow(argv[0]);

    /* Registruju se funkcije za obradu dogadjaja. */
    glClearColor(0.75, 0.75, 0.75, 0);
    glutKeyboardFunc(on_keyboard);
    glutDisplayFunc(on_display);

    /* Obavlja se OpenGL inicijalizacija. */

    /* Ulazi se u glavnu petlju. */
    glutMainLoop();

    return 0;
}

static void on_keyboard(unsigned char key, int x, int y)
{
    switch(key){
        case 27:
            exit(0);
            break;
        case 'i':
            //headX -= 0.5;
            headY -= 0.5;
            headZ -= 0.5;
            glutPostRedisplay();
            break;

        case 'k':
            //headX += 0.5;
            headY += 0.5;
            headZ += 0.5;
            glutPostRedisplay();
            break;
    }
}

static void on_display(void)
{
   /* Brise se prethodni sadrzaj prozora. */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    
    gluPerspective(60, 1, 1, 800); 

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    

    gluLookAt(
        headX, headY, headZ,
        atX, atY, atZ,
        0, 0, 1
    );

    drawCoord();
    drawPlane();

    glColor3f(1, 1, 1);
    glLineWidth(4);

    glutSwapBuffers();
}

//draw corinate grid for debugging
void drawCoord(){
    glLineWidth(1);
    glColor3f(0, 0, 1.0);
    //X AXIS IS BLUE
    glBegin(GL_LINES);
        glVertex3f(0, 0, 0);
        glVertex3f(300.0, 0, 0);
    glEnd();
    glBegin(GL_LINES);
        glVertex3f(0, 0, 0);
        glVertex3f(-300.0, 0, 0);
    glEnd();
    glColor3f(0, 1.0, 0);
    //Y AXIS IS GREEN
    glBegin(GL_LINES);
        glVertex3f(0, 0, 0);
        glVertex3f(0, 300.0, 0);
    glEnd();
    glBegin(GL_LINES);
        glVertex3f(0, 0, 0);
        glVertex3f(0, -300.0, 0);
    glEnd();

    glColor3f(1.0, 0, 0);
    //Z AXIS IS RED
    glBegin(GL_LINES);
        glVertex3f(0, 0, 0);
        glVertex3f(0, 0, 300.0);
    glEnd();
    glBegin(GL_LINES);
        glVertex3f(0, 0, 0);
        glVertex3f(0, 0, -300.0);
    glEnd();
}


void drawPlane(){
    glColor3f(1, 1, 1);
    glPushMatrix();
        glTranslated(0, marginWidth/2, 0);
        glPushMatrix();
            glTranslated(0, planeWidth/2, planeHeight/2);
            glScaled(planeLength, planeWidth, planeHeight);

            glutSolidCube(1);
        glPopMatrix();  
        
        glColor3f(0, 0, 0);
        glPushMatrix();
            glTranslated(0, planeWidth  + (marginWidth)/2, planeHeight/2);
            glScaled(planeLength, marginWidth, planeHeight);

            glutSolidCube(1);
        glPopMatrix();
    glPopMatrix();

    glPushMatrix();
        glTranslated(0, 0, planeHeight/2);
        glScaled(planeLength, marginWidth, planeHeight);
        glutSolidCube(1);
    glPopMatrix();

    glColor3f(1, 1, 1);
    glPushMatrix();
        glTranslated(0, -marginWidth/2, 0);
        glPushMatrix();
            glTranslated(0, -planeWidth/2, planeHeight/2);
            glScaled(planeLength, planeWidth, planeHeight);

            glutSolidCube(1);
        glPopMatrix();

        glColor3f(0, 0, 0);
        glPushMatrix();
            glTranslated(0, -planeWidth  - (marginWidth)/2, planeHeight/2);
            glScaled(planeLength, marginWidth, planeHeight);

            glutSolidCube(1);
        glPopMatrix();
    glPopMatrix();
}
