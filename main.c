#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>
#include <math.h>


/* Deklaracije callback funkcija. */
static void on_keyboard(unsigned char key, int x, int y);
static void on_display(void);
void drawCoord();
void drawPlane();
void drawFrog();
void drawCar();
void drawLog(int);
void drawTurtle();

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


double frogBody = 1;
double frogHead=frogBody*0.6;
double frogLegs=frogBody*0.3;

double carLenght = 3;
double carWidth = 1.5;
double carHeight = 0.8;

double logWidth = planeWidth/5;
double logHeight = 1;


int main(int argc, char **argv)
{

    headX = 0;
    headY = 20;
    headZ = 25;

    atX = 0;
    atY = 0;
    atZ = 0;
    /* Inicijalizuje se GLUT. */
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    
    /* Kreira se prozor. */

    glutInitWindowSize(800, 800);
    glutInitWindowPosition(0, 0);
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
        case 'q':
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
        //change camera angle for debugging
        case 't':
            headX = 0;
            headY = 0.1;
            headZ = 20;
            glutPostRedisplay();
            break;
        case 'r':
            headX = 0;
            headY = 20;
            headZ = 0;
            glutPostRedisplay();
            break;
        case 'l':
            headX = 20;
            headY = 0;
            headZ = 0;
            glutPostRedisplay();
            break;
        case 'd':
            headX = 0;
            headY = 20;
            headZ = 25;
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
    glEnable(GL_DEPTH_TEST);
    gluPerspective(60, 1, 1, 800); 

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    

    gluLookAt(
        headX, headY, headZ,
        atX, atY, atZ,
        0, 0, 1
    );

    //drawCoord();
    drawPlane();
    drawFrog();
    drawCar();
    drawLog(6);

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

void drawFrog(){
    glColor3f(0.2, 0.9, 0.2);
    glPushMatrix();
    //body
    glTranslated(0, marginWidth+planeWidth, planeHeight/2+frogBody+frogLegs);
    glutSolidSphere(frogBody,20,20);
    //head
    glPushMatrix();
        glColor3f(0.5, 0.8, 0.5);
        glTranslated(0,-frogHead/2-frogHead/2, frogHead);
        glutSolidSphere(frogHead,20,20);
    glPopMatrix();
    //legs
    glPushMatrix();
        glColor3f(0.3, 0.8, 0.3);
        glTranslated(frogBody/2,frogBody/2,-frogBody);
        glutSolidSphere(frogLegs,20,20);
    glPopMatrix();

    glPushMatrix();
        glTranslated(-frogBody/2,frogBody/2,-frogBody);
        glutSolidSphere(frogLegs,20,20);
    glPopMatrix();

    glPushMatrix();
        glTranslated(-frogBody/2,-frogBody/2,-frogBody);
        glutSolidSphere(frogLegs,20,20);
    glPopMatrix();

    glPushMatrix();
        glTranslated(frogBody/2,-frogBody/2,-frogBody);
        glutSolidSphere(frogLegs,20,20);
    glPopMatrix();




    glPopMatrix();  

}

void drawCar(){
    glColor3f(0.8,0.3,0.3);
    glPushMatrix();

    glTranslated(0,planeWidth,planeHeight+carHeight/2+planeHeight);

    //main 
    glPushMatrix();
    glScaled(carLenght,carWidth,carHeight);
    glutSolidCube(1);
    glPopMatrix();

    //top
    glPushMatrix();
    glColor3f(0.9,0.2,0.2);
    glTranslated(0,0,carHeight);
    glScaled(carLenght/2,carWidth,carHeight);
    glutSolidCube(1);
    glPopMatrix();

    //wheels
    double tireOffset = carLenght/8;
    glPushMatrix();
    glColor3f(0,0,0);
    glTranslatef(carLenght/2-tireOffset,carWidth/2,-carHeight/2);
    glRotatef(90,1,0,0);
    glutSolidTorus(carHeight/5,carWidth/5,20,20);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-carLenght/2+tireOffset,carWidth/2,-carHeight/2);
    glRotatef(90,1,0,0);
    glutSolidTorus(carHeight/5,carWidth/5,20,20);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-carLenght/2+tireOffset,-carWidth/2,-carHeight/2);
    glRotatef(90,1,0,0);
    glutSolidTorus(carHeight/5,carWidth/5,20,20);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(carLenght/2-tireOffset,-carWidth/2,-carHeight/2);
    glRotatef(90,1,0,0);
    glutSolidTorus(carHeight/5,carWidth/5,20,20);
    glPopMatrix();

    glPopMatrix();
}

void drawLog(int logLenght){
    glColor3f(0.5,0.2,0);
    glPushMatrix();
    GLUquadricObj *quadratic;
    quadratic = gluNewQuadric();
    glTranslatef(0,-planeWidth/2,planeHeight/2+logHeight);
    glRotatef(90,0,1,0);
    gluCylinder(quadratic,logHeight,logHeight,logLenght,20,20);

    glPopMatrix();
}

void drawPlane(){
    glColor3f(0.1, 0.1, 0.1);
    glPushMatrix();
        glTranslated(0, marginWidth/2, 0);
        glPushMatrix();
            glTranslated(0, planeWidth/2, planeHeight/2);
            glScaled(planeLength, planeWidth, planeHeight);

            glutSolidCube(1);
        glPopMatrix();  
        
        glColor3f(0.8, 0.8, 1);
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

    glColor3f(0.3,0.3,9);
    glPushMatrix();
        glTranslated(0, -marginWidth/2, 0);
        glPushMatrix();
            glTranslated(0, -planeWidth/2, planeHeight/2);
            glScaled(planeLength, planeWidth, planeHeight);

            glutSolidCube(1);
        glPopMatrix();

        glColor3f(0.8, 0.8, 1);
        glPushMatrix();
            glTranslated(0, -planeWidth  - (marginWidth)/2, planeHeight/2);
            glScaled(planeLength, marginWidth, planeHeight);

            glutSolidCube(1);
        glPopMatrix();
    glPopMatrix();
}
