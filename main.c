#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>
#include <math.h>
#include <unordered_set>


#define TIMER_INTERVAL 20
#define TIMER_ID 0
#define TIMER_ID2 1

/* Deklaracije callback funkcija. */
static void on_keyboard(unsigned char key, int x, int y);
static void on_display(void);
static void on_timer(int id);

void drawCoord();
void drawPlane();
void drawFrog();
void drawCar(int);
void drawLog(int,int);
void drawTurtle();
void drawTurtles(int,int);
void drawRandomCars();
void drawRandomLogs();
void drawRandomTurtles();

float animation_parameter = 0;
int animation_ongoing = 0;

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

int oldValue = 0;

double frogBody = 1;
double frogHead=frogBody*0.6;
double frogLegs=frogBody*0.3;
double frogXpar=0;
double frogYpar=0;

double carLenght = 3;
double carWidth = 1.5;
double carHeight = 0.8;

double logWidth = planeWidth/6;
double logHeight = 1;

double turtleSize = planeWidth/6;

typedef struct{
    float offset;
    int lane;
}Car;

unordered_set<Car> Cars;

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

    //prekopiran deo za isveteljenje iz kostura koji je dat za kolokvijum
    glEnable(GL_NORMALIZE);
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE,1);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    float light_position[] = {-1, 3, 2, 1};
    float light_ambient[] = {.3f, .3f, .3f, 1};
    float light_diffuse[] = {.7f, .7f, .7f, 1};
    float light_specular[] = {.7f, .7f, .7f, 1};

    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);


    glEnable(GL_COLOR_MATERIAL);
    glClearColor(0.7, 0.7, 0.7, 0);
   

    glutMainLoop();

    return 0;
}

static void on_keyboard(unsigned char key, int x, int y){
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
        case 'p':
            headX = 0;
            headY = 20;
            headZ = 25;
            glutPostRedisplay();
            break;
        case 'g':
        case 'G':
            if (!animation_ongoing) {
                animation_ongoing = 1;
                glutTimerFunc(TIMER_INTERVAL, on_timer, TIMER_ID);
            }
             glutTimerFunc(TIMER_INTERVAL, on_timer, TIMER_ID2);
            break;
        case ';':
            animation_ongoing = 0;
            break;
        case 'z':
        case 'Z':
            animation_parameter = 0;
            glutPostRedisplay();
            break;
        //frog movement
        case 'a':
        frogXpar += planeLength/12;
        glutPostRedisplay();
        break;
        case 'd':
        frogXpar -= planeLength/12;
        glutPostRedisplay();
        break;
        case 's':
        frogYpar += planeWidth/5;
        glutPostRedisplay();
        break;
        case 'w':
        frogYpar -= planeWidth/5;
        glutPostRedisplay();
        break;
        
    }
}

void on_timer(int id) {
    /* if (id == TIMER_ID) {

        animation_parameter += 1;
    }

    glutPostRedisplay();
    if (animation_ongoing) {
        glutTimerFunc(TIMER_INTERVAL, on_timer, TIMER_ID);
    } */
    switch(id){
        case TIMER_ID2:
        printf("OVde sam\n");
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
    for(int i =1;i<6;i++){
        glPushMatrix();
        glTranslatef(animation_parameter*0.1,0,0);
        drawCar(i);
        glPopMatrix();
    }


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

void drawCar(int poss){
    glColor3f(0.8,0.3,0.3);
    glPushMatrix();
    //translacija na pocetnu poziciju
    glTranslated(-planeLength/2,planeWidth/5*poss,planeHeight+carHeight/2+planeHeight);

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

void drawLog(int poss,int logLenght){
    glColor3f(0.5,0.2,0);
    glPushMatrix();
    GLUquadricObj *quadratic;
    quadratic = gluNewQuadric();
    GLUquadricObj *quadratic1;
    quadratic1 = gluNewQuadric();
    glTranslatef(-planeLength/2-2*logLenght,-poss*planeWidth/5,planeHeight/2+logHeight);
    glRotatef(90,0,1,0);

    //filling the cylinder from both sides
    gluDisk(quadratic1,0,logHeight,20,20);

    glPushMatrix();
    glTranslatef(0,0,logLenght);
    gluDisk(quadratic1,0,logHeight,20,20);
    glPopMatrix();

    //drawing the clylinder
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

void drawTurtle(){
    glPushMatrix();

    //one turtle
    glPushMatrix();
    //body
    glColor3f(0.3,0.7,0.1);
    glTranslatef(0,-planeWidth/5,planeHeight+turtleSize/10);
    glutSolidTorus(turtleSize/5,turtleSize/5,20,20);
    //head
    glPushMatrix();
    glColor3f(0.3,0.8,0.1);
    glTranslatef(turtleSize/3,0,0);
    glutSolidTorus(turtleSize/10,turtleSize/10,20,20);
    glPopMatrix();
    //legs
    glPushMatrix();
    glColor3f(0.2,0.9,0.1);
    glTranslatef(turtleSize/3,turtleSize/3,0);
    glutSolidTorus(turtleSize/15,turtleSize/15,20,20);
    glPopMatrix();

    glPushMatrix();
    glColor3f(0.2,0.9,0.1);
    glTranslatef(-turtleSize/3,turtleSize/3,0);
    glutSolidTorus(turtleSize/15,turtleSize/15,20,20);
    glPopMatrix();

    glPushMatrix();
    glColor3f(0.2,0.9,0.1);
    glTranslatef(-turtleSize/3,-turtleSize/3,0);
    glutSolidTorus(turtleSize/15,turtleSize/15,20,20);
    glPopMatrix();

    glPushMatrix();
    glColor3f(0.2,0.9,0.1);
    glTranslatef(turtleSize/3,-turtleSize/3,0);
    glutSolidTorus(turtleSize/15,turtleSize/15,20,20);
    glPopMatrix();



    glPopMatrix();

    glPopMatrix();



}

void drawTurtles(int pos,int num){
    if(num==2){
    glPushMatrix();
    glTranslatef(-planeLength/2-turtleSize*num,0,0);
    drawTurtle();
    glTranslatef(turtleSize,0,0);
    drawTurtle();
    glPopMatrix();   
    }else{
        glPushMatrix();
            glTranslatef(-planeLength/2-turtleSize*num,0,0);

        drawTurtle();
        glTranslatef(turtleSize,0,0);
        drawTurtle();
        glTranslatef(-2*turtleSize,0,0);
        drawTurtle();

        glPopMatrix();
    }
}