#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>
#include <math.h>
#include <algorithm>
#include <list>
#include<time.h>


using namespace std;

#define TIMER_INTERVAL 2000
#define TIMER_ID 0
#define TIMER_ID2 1

/* Deklaracije callback funkcija. */
static void on_keyboard(unsigned char key, int x, int y);
static void on_display(void);
static void on_timer(int id);

void drawCoord();
void drawPlane();
void drawFrog();
void drawCar(int,int);
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

//vars for randomising car lanes
int myArray2[3] = {1,3,5};
int index2;
int lane2;

int myArray1[2] = {2,4};
int index1;
int lane1;



//structs for storing objects
typedef struct{
    float offset;
    int lane;
    int side;
    float speed;
}Car;

typedef struct{
    float offset;
    int lane;
    int size;
    float speed;
}Log;

typedef struct{
    float offset;
    int lane;
    int size;
    float speed;
}Turtle;


list<Car> Cars;
list<Log> Logs;
list<Turtle> Turtles;

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
            glutTimerFunc(20, on_timer, TIMER_ID);
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

            //adding cars to the list at 2000ms intervals
            Car c;
            //radnomise lane
            srand ( time(NULL) );
            index1 = rand() % 2;
            lane1 = myArray1[index1];

            c.offset=0;
            c.side = 1;
            c.lane=lane1;
            c.speed=0.3;
            Cars.push_back(c);


            //randomise lane for cars going from left to right

            index2 = rand() % 3;
            lane2 = myArray2[index2];

            Car c1;
            c1.offset=planeLength;
            c1.side = 0;
            c1.lane=lane2;
            c1.speed = -0.3;
            Cars.push_back(c1);


            //adding logs to the list
            Log l1;
            l1.offset=planeLength;
            l1.lane=2;
            l1.size=3;
            l1.speed = -0.08 ;
            Logs.push_back(l1);

            Log l2;
            l2.offset=planeLength;
            l2.lane=3;
            l2.size=7;
            l2.speed = -0.20;
            Logs.push_back(l2);

            Log l3;
            l3.offset=planeLength;
            l3.lane=5;
            l3.size=5;
            l3.speed = -0.14;
            Logs.push_back(l3);

            //ading turtles to the list
            Turtle t1;
            t1.offset=0;
            t1.size=3;
            t1.lane=1;
            t1.speed=0.15;
            Turtles.push_back(t1);

            Turtle t2;
            t2.offset=0;
            t2.size=2;
            t2.lane=3;
            t2.speed=0.20;
            Turtles.push_back(t2);



            glutTimerFunc(TIMER_INTERVAL, on_timer, TIMER_ID2);
            break;
        case TIMER_ID:
            for(auto it = Cars.begin(); it != Cars.end(); it++){
                it->offset += it->speed;
    
                //if branch beacuse different dissapearing point depening on side
                if(it->speed < 0 ){
                    if(it->offset < carLenght/2){
                        it = Cars.erase(it);
                    }
                }
                else{
                    if(it->offset > planeLength - carLenght/2){
                        it = Cars.erase(it);
                    }
                }
            }
            for(auto it = Logs.begin(); it != Logs.end(); it++){
                it->offset += it->speed;
                if(it->offset<-it->size)
                it = Logs.erase(it);
            }
            for(auto it = Turtles.begin(); it != Turtles.end(); it++){
                it->offset += it->speed;
                if(it->offset>planeLength+(turtleSize*it->size)+turtleSize)
                it = Turtles.erase(it);
            }

            //change
            glutPostRedisplay();
            glutTimerFunc(20, on_timer, TIMER_ID);
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
    
    for(auto it = Cars.begin(); it != Cars.end(); it++){
        glPushMatrix();
        glTranslatef(it->offset,0,0);
        drawCar(it->lane,it->side);
        glPopMatrix();
    }

    for(auto it = Logs.begin(); it != Logs.end(); it++){
        glPushMatrix();
        glTranslatef(it->offset,0,0);
        drawLog(it->lane,it->size);
        glPopMatrix();
    }

    for(auto it = Turtles.begin(); it != Turtles.end(); it++){
        glPushMatrix();
        glTranslatef(it->offset,0,0);
        drawTurtles(it->lane,it->size);
        glPopMatrix();
    }

    drawFrog();

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

void drawCar(int poss,int side){
    glColor3f(0.8,0.3,0.3);
    glPushMatrix();
    //translacija na pocetnu poziciju u zavisnosti od strane
    if(side)
        glTranslated(-planeLength/2,planeWidth/5*poss,planeHeight+carHeight/2+planeHeight);
    else
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
    glTranslatef(-planeLength/2,-poss*planeWidth/5,planeHeight/2);
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

void drawTurtles(int poss,int num){
    if(num==2){
    glPushMatrix();
    glTranslatef(-planeLength/2-turtleSize*num/2,-poss*planeWidth/5,0);
    drawTurtle();
    glTranslatef(turtleSize,0,0);
    drawTurtle();
    glPopMatrix();   
    }else{
        glPushMatrix();
            glTranslatef(-planeLength/2-turtleSize*num/2,0,0);

        drawTurtle();
        glTranslatef(turtleSize,0,0);
        drawTurtle();
        glTranslatef(-2*turtleSize,0,0);
        drawTurtle();

        glPopMatrix();
    }
}