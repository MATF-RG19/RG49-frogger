#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>
#include <math.h>
#include <algorithm>
#include <list>
#include<time.h>


using namespace std;

#define TIMER_INTERVAL 20
#define TIMER_INTERVAL1 1000
#define TIMER_INTERVAL2 1000
#define TIMER_INTERVAL3 500
#define TIMER_ID 0
#define TIMER_ID1 1
#define TIMER_ID2 2
#define TIMER_ID3 3


/* Deklaracije callback funkcija. */
static void on_keyboard(unsigned char key, int x, int y);
static void on_display(void);
static void on_timer(int id);

void drawCoord();
void drawPlane();
void drawFrog(int);
void drawCar(int,int);
void drawLog(int,int);
void drawTurtle(int);
void drawTurtles(int,int,int);


int globalTimer;

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

double frogBody = planeWidth/10;
double frogHead=frogBody*0.6;
double frogLegs=frogBody*0.3;


double carLenght = planeWidth/3;
double carWidth = carLenght/2;
double carHeight = carWidth/2;

double logWidth = planeWidth/12;

double turtleSize = planeWidth/6;

//vars for randomizing car lanes
int myArray2[3] = {1,3,5};
int index2;
int lane2;

int myArray1[2] = {2,4};
int index1;
int lane1;
//ranomising turtles
int dissTurtle;


//var for detecting death
int movingOnLogThisCycle;
int movingOnTurtleThisCycle;


//structs for storing objects
typedef struct{
    float offset;
    int lane;
    float speed;
    int color;
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
    /*states used for dissapearing logic
    if turtle is randmly selected to dissapear(happens every second)
    state changes every half second for 4 cycles (changing the color of the turtle)
    and after that the turtle dissapears
    */
    int state;
    int red;
}Turtle;

typedef struct{
    int lane;
    float offset;
    float speed;
    int dead;
}Frog;

//Lists for storing objects
Frog frog;
list<Car> Cars;
list<Log> Logs;
list<Turtle> Turtles;

//tmp
list<Turtle>::iterator diss;


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

    //prekopiran deo za osvetljenje iz kostura koji je dat za kolokvijum
    glEnable(GL_NORMALIZE);
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE,1);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    float light_position[] = {-5, 3, 2, 1};
    float light_ambient[] = {.3f, .3f, .3f, 1};
    float light_diffuse[] = {.9f, .9f, .9f, 1};
    float light_specular[] = {.7f, .7f, .7f, 1};

    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);


    glEnable(GL_COLOR_MATERIAL);
    glClearColor(0.9, 0.9, 0.9, 0);

    //initialising frog at the beggining
    frog.lane=6;
    frog.offset=planeLength/2;
    frog.speed =-0;
    frog.dead=0;

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
            //headX +1= 0.5;
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
            glutTimerFunc(TIMER_INTERVAL, on_timer, TIMER_ID);
            glutTimerFunc(TIMER_INTERVAL1,on_timer, TIMER_ID1);
            glutTimerFunc(TIMER_INTERVAL2, on_timer, TIMER_ID2);
            glutTimerFunc(TIMER_INTERVAL3, on_timer, TIMER_ID3);
            break;
        
        //frog movement
        case 'a':
        frog.offset += planeLength/25;
        glutPostRedisplay();
        break;
        case 'd':
        frog.offset -= planeLength/25;
        glutPostRedisplay();
        break;
        case 's':
        frog.lane += 1;
        glutPostRedisplay();
        break;
        case 'w':
        frog.lane -= 1;
        glutPostRedisplay();
        break;
        
    }
}



void on_timer(int id) {
    switch(id){
        case TIMER_ID1:

           //adding cars to the list at interval
            Car c;
            //randomize lane
            srand ( time(NULL) );
            index1 = rand() % 2;
            lane1 = myArray1[index1];

            c.offset=0;
            c.lane=lane1;
            c.speed=0.3;
            srand(time(NULL)+2);
            c.color = rand()%3;
            Cars.push_back(c); 

            //randomize lane for cars going from left to right
            index2 = rand() % 3;
            lane2 = myArray2[index2];

            Car c1;
            c1.offset=planeLength;
            c1.lane=lane2;
            c1.speed = -0.3;
            c1.color=rand()%3;
            Cars.push_back(c1); 
            
            /*this logic is in this timer(and not in timerid3 ) becasue we want this
            to happen every second, and that is this timer's TIME_INTERVAL
            */
            //get a random index from turtle list
            dissTurtle = rand() % (Turtles.size() + 1 - 0) + Turtles.size();
            //get a  Turtle object with that index
            diss= std::next(Turtles.begin(), dissTurtle);
            //if it isnt dissapearing already make it dissapear
            if(diss->state==0)
                diss->state=1;
            


            glutTimerFunc(TIMER_INTERVAL1,on_timer,TIMER_ID1);
            break;
        case TIMER_ID2:
            //doubled the timer-interval and added a random element to upper objects
            if(rand()%2){
                //adding logs to the list
                Log l1;
                l1.offset=planeLength;
                l1.lane=2;
                l1.size=3;
                l1.speed = -0.1 ;
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
                l3.speed = -0.15;
                Logs.push_back(l3); 

                //adding turtles to the list
                Turtle t1;
                t1.offset=0;
                t1.size=3;
                t1.lane=1;
                t1.speed=0.15;
                t1.red=0;
                t1.state=0;
                Turtles.push_back(t1);

                Turtle t2;
                t2.offset=0;
                t2.size=2;
                t2.lane=4;
                t2.speed=0.20;
                t2.red=0;
                t2.state=0;
                Turtles.push_back(t2);
            }
            glutTimerFunc(TIMER_INTERVAL2,on_timer,TIMER_ID2);
            break;
        case TIMER_ID3:
            /*special timer for dissapearing turtles, timer interval is 500ms
            so that every 500ms the chosen(to dissapear) turtle changes color
            and after 4 cycles of that it dissapears
            */
            for(auto it = Turtles.begin(); it != Turtles.end(); it++){
                if(it->state>4){
                    it = Turtles.erase(it);
                    continue;
                }
                if(it->state > 0){
                    if(it->state%2!=0){
                        it->red=1;
                    }
                    else{
                        it->red=0;
                    }  
                    it->state++;
                }
            }

            glutTimerFunc(TIMER_INTERVAL3, on_timer, TIMER_ID3);
            break;
        case TIMER_ID:
            //animate and delete objects that are no longer needed
            for(auto it = Cars.begin(); it != Cars.end(); it++){
                //changing offset by factor of speed
                it->offset += it->speed;

                //if branch needed beacause different dissapearing point deppenging on a side car comes from
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
            //
            for(auto it = Logs.begin(); it != Logs.end(); it++){
                it->offset += it->speed;
                if(it->offset < -it->size)
                    it = Logs.erase(it);
            }
            for(auto it = Turtles.begin(); it != Turtles.end(); it++){
                it->offset += it->speed;
                if(it->offset > planeLength + (turtleSize*it->size) + turtleSize || it->state>4)
                    it = Turtles.erase(it);
            }

            //frog only moves if it is on the upper part of the map(Thats where the lane bumer is negative)
            if(frog.lane<0){
                frog.offset += frog.speed;
            }
            //frog.speed is set to 0 after changing the offset in case that in the next itteration its not on the object anymore
            frog.speed=0;
            //checking if frog is dead RIP
            if(frog.dead){
                frog.lane=6;
                frog.offset=planeLength/2;
                frog.speed =-0;
            }
            glutPostRedisplay();
            glutTimerFunc(TIMER_INTERVAL, on_timer, TIMER_ID);
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
    frog.dead=0;
    //drawCoord();
    drawPlane();



    //going through all the cars in Cars list
    for(auto it = Cars.begin(); it != Cars.end(); it++){
        glPushMatrix();
        //changing the possition of car depengin on the offset and drawing said car depending on factors in Cars stuct.
        glTranslatef(it->offset,0,0);
        drawCar(it->lane,it->color);
        glPopMatrix();


        //collisions
        //if the frog is in the same lane as car and the offset is within the margin its a COLLISION!
        if(it->lane == frog.lane && abs(it->offset - frog.offset)<carLenght/2){
            frog.dead = 1;
        }
    }
    //used to determine if the frog is on any object during this cycle
    movingOnLogThisCycle=0;

    for(auto it = Logs.begin(); it != Logs.end(); it++){
        glPushMatrix();
        glTranslatef(it->offset,0,0);
        drawLog(it->lane,it->size);
        glPopMatrix();

        if(frog.lane==-2 || frog.lane==-3 || frog.lane==-5){
            //fprintf(stderr,"%d",frog.lane);
            if(it->lane == abs(frog.lane) && (frog.offset-it->offset)< it->size &&(frog.offset-it->offset) >0){
                frog.speed = it->speed;
                movingOnLogThisCycle=1;
            }
        }
    }
    //if it is on lanes where logs are, and it's not on a log, that means death
    if(!movingOnLogThisCycle && (frog.lane==-2 || frog.lane==-3 || frog.lane==-5)){
        frog.dead=1;
    }


    //death Var set to 0 at the beggining of every display cycle
    movingOnTurtleThisCycle=0;

    for(auto it = Turtles.begin(); it != Turtles.end(); it++){
        glPushMatrix();
        glTranslatef(it->offset,0,0);
        glColor3f(0.8,0.3,0.3);
        drawTurtles(it->lane,it->size,it->red);
        glPopMatrix();
        if(frog.lane==-1 || frog.lane ==-4){
            if(it->lane == abs(frog.lane) && (it->offset-frog.offset) < it->size*turtleSize && (it->offset-frog.offset) >0){
                frog.speed = it->speed;
                movingOnTurtleThisCycle=1;
            }
        }
    }
    //checking the death cond
    if(!movingOnTurtleThisCycle && (frog.lane==-1 || frog.lane ==-4)){
        frog.dead=1;
    }

    //death out out bounds
    if(frog.lane > 6 || frog.lane < -6 || frog.offset>planeLength || frog.offset<0){
        frog.dead=1;
    }


    //for debug
  //  printf("Imamo %d auta, % d zaba i %d drveca\n", Cars.size(),Turtles.size(),Logs.size());
    //fflush(stdout);
    glPushMatrix();
        glTranslatef(frog.offset,0,0);
        drawFrog(frog.lane);
    glPopMatrix();   
    // printf("%d is lane, %.2f is offset\n",frog.lane ,frog.offset);

    //printf("Imamo %d auta, % d kornjaca i %d drveca\n", Cars.size(),Turtles.size(),Logs.size());
        


    glColor3f(1, 1, 1);
    glLineWidth(4);

    glutSwapBuffers();
}



//draw cordinate grid for debugging
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

void drawFrog(int lane){
    glColor3f(0.2, 0.9, 0.2);
    glPushMatrix();
    //body
    glTranslated(-planeLength/2, marginWidth+(planeWidth)/5*(lane-1), planeHeight/2+frogBody+frogLegs);
    glutSolidSphere(frogBody,15,15);

    //head
    glPushMatrix();
        glColor3f(0.5, 0.8, 0.5);
        glTranslated(0,-frogHead/2-frogHead/2, frogHead);
        glutSolidSphere(frogHead,10,10);
    glPopMatrix();

    //legs
    glPushMatrix();
        glColor3f(0.3, 0.8, 0.3);
        glTranslated(frogBody/2,frogBody/2,-frogBody);
        glutSolidSphere(frogLegs,5,5);
    glPopMatrix();

    glPushMatrix();
        glTranslated(-frogBody/2,frogBody/2,-frogBody);
        glutSolidSphere(frogLegs,5,5);
    glPopMatrix();

    glPushMatrix();
        glTranslated(-frogBody/2,-frogBody/2,-frogBody);
        glutSolidSphere(frogLegs,5,5);
    glPopMatrix();

    glPushMatrix();
        glTranslated(frogBody/2,-frogBody/2,-frogBody);
        glutSolidSphere(frogLegs,5,5);
    glPopMatrix();

    glPopMatrix();  
}

void drawCar(int poss,int color){
    //switching the color
     switch (color){
        case 0:
            glColor3f(0.9,0.2,0.2);
            break;
        case 1:
            glColor3f(0.2,0.9,0.2);
            break;
        case 2:
            glColor3f(0.2,0.2,0.9);
            break;
    }
  
    glPushMatrix();
    //translate car to corr poss
    glTranslated(-planeLength/2,planeWidth/5*poss,planeHeight+carHeight/2+planeHeight);
   
    //main 
    glPushMatrix();
    glScaled(carLenght,carWidth,carHeight);
    glutSolidCube(1);
    glPopMatrix();

    //top
    glPushMatrix();
    //switch for color
   
    switch (color){
        case 0:
            glColor3f(0.8,0.3,0.3);
            break;
        case 1:
            glColor3f(0.3,0.8,0.3);
            break;
        case 2:
            glColor3f(0.3,0.3,0.8);
            break;
    }
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
    glutSolidTorus(carHeight/5,carWidth/5,8,8);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-carLenght/2+tireOffset,carWidth/2,-carHeight/2);
    glRotatef(90,1,0,0);
    glutSolidTorus(carHeight/5,carWidth/5,8,8);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-carLenght/2+tireOffset,-carWidth/2,-carHeight/2);
    glRotatef(90,1,0,0);
    glutSolidTorus(carHeight/5,carWidth/5,8,8);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(carLenght/2-tireOffset,-carWidth/2,-carHeight/2);
    glRotatef(90,1,0,0);
    glutSolidTorus(carHeight/5,carWidth/5,8,8);
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
    gluDisk(quadratic1,0,logWidth,15,15);

    glPushMatrix();
    glTranslatef(0,0,logLenght);
    gluDisk(quadratic1,0,logWidth,15,15);
    glPopMatrix();

    //drawing the cylinder  
    gluCylinder(quadratic,logWidth,logWidth,logLenght,15,15);

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

void drawTurtle(int red){
    glPushMatrix();

    //one turtle
    glPushMatrix();
    //body

    //check if red
    red ? glColor3f(0.7,0.3,0.1) : glColor3f(0.3,0.7,0.1);

    glTranslatef(0,-planeWidth/5,planeHeight+turtleSize/10);
    glutSolidTorus(turtleSize/5,turtleSize/5,15,15);
    //head
    glPushMatrix();
    //check if red
    red ? glColor3f(0.8,0.3,0.1) : glColor3f(0.3,0.8,0.1);

    glTranslatef(turtleSize/3,0,0);
    glutSolidTorus(turtleSize/10,turtleSize/10,10,10);
    glPopMatrix();
    //legs
    glPushMatrix();
    glColor3f(0.2,0.9,0.1);
    glTranslatef(turtleSize/3,turtleSize/3,0);
    glutSolidTorus(turtleSize/15,turtleSize/15,5,5);
    glPopMatrix();

    glPushMatrix();
    //glColor3f(0.2,0.9,0.1);
    glTranslatef(-turtleSize/3,turtleSize/3,0);
    glutSolidTorus(turtleSize/15,turtleSize/15,5,5);
    glPopMatrix();

    glPushMatrix();
    //glColor3f(0.2,0.9,0.1);
    glTranslatef(-turtleSize/3,-turtleSize/3,0);
    glutSolidTorus(turtleSize/15,turtleSize/15,5,5);
    glPopMatrix();

    glPushMatrix();
    //glColor3f(0.2,0.9,0.1);
    glTranslatef(turtleSize/3,-turtleSize/3,0);
    glutSolidTorus(turtleSize/15,turtleSize/15,5,5);
    glPopMatrix();



    glPopMatrix();

    glPopMatrix();



}
//this needs refactoring
void drawTurtles(int poss,int num,int red){
    if(num==2){
    glPushMatrix();
        glTranslatef(-planeLength/2-turtleSize*num/2,-(poss-1)*planeWidth/5,0);
        drawTurtle(red);
        glTranslatef(turtleSize,0,0);
        drawTurtle(red);
    glPopMatrix();   
    }else{
        glPushMatrix();
            glTranslatef(-planeLength/2-turtleSize*num/2,0,0);
            drawTurtle(red);
            glTranslatef(turtleSize,0,0);
            drawTurtle(red);
            glTranslatef(-2*turtleSize,0,0);
            drawTurtle(red);
        glPopMatrix();
    }
}