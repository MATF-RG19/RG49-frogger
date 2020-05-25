#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>
#include <math.h>
#include <algorithm>
#include <list>
#include <time.h>
#include <iterator>
#include <random>
#include <string.h>
#include "image.h"


/*  Brief explanation of the code 

    objects are genereted in timers and their values updated
    collisions are done in on_display()


    I tried to separate functions to different files but because of the use of global vars at the beggingin of the project
     i couldn't do it, so to compensate i commented as much as i could to hopefully make it as less confusing as possible


 */

using namespace std;
/* DEFINING TIMERS */
#define TIMER_INTERVAL 15
#define TIMER_INTERVAL1 1000
#define TIMER_INTERVAL3 500
#define TIMER_INTERVAL4 5500
#define TIMER_ID 0
#define TIMER_ID1 1
#define TIMER_ID3 3
#define TIMER_ID4 4
//texture

#define FILENAME0 ("water.bmp")
int texture;
static GLuint names[1];




/* Declaration if callback functions. */
static void on_keyboard(unsigned char key, int x, int y);
static void on_display(void);
static void on_timer(int id);
static void on_reshape(int,int);
//taken from the course materials
static void initializeTexture(void);
/**/

void drawText();
void drawPlane();
void drawFrog(int);
void drawCar(int,int);
void drawLog(int,int);
void drawTurtle(int);
void drawTurtles(int,int,int);
void drawheart(int,int);
void moveFrog();

//taken from the internet for randomly selecting an item from collection
template<typename Iter, typename RandomGenerator>
Iter select_randomly(Iter start, Iter end, RandomGenerator& g) {
    std::uniform_int_distribution<> dis(0, std::distance(start, end) - 1);
    std::advance(start, dis(g));
    return start;
}
template<typename Iter>
Iter select_randomly(Iter start, Iter end) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    return select_randomly(start, end, gen);
}

/*Vars for camara control*/
double headX;
double headY;
double headZ;

double atX;
double atY;
double atZ;

/*Vars for drawing the game plane */
double planeLength = 40;
double planeWidth = 10;
double planeHeight = 0.3;
double marginWidth = planeWidth/5;

/* Vars for drawing the frog */
double frogBody = planeWidth/10;
double frogHead=frogBody*0.6;
double frogLegs=frogBody*0.3;

//used for dissapearing
int numberofBlinks;

/*Vars for drawing cars,logs,and turtles */
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

//list for randomising heart possition 
list<double> heartsArray = {planeLength*0.05, planeLength*0.20, planeLength*0.35, planeLength*0.50, planeLength*0.65, planeLength*0.80};

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
    /*states used for dissapearing logic.
    if turtle is randomly selected to dissapear(happens every second)
    state changes every half second for *int numberofBlinks* cycles (changing the color of the turtle)
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
    int movingOnLogThisCycle;
    int movingOnTurtleThisCycle;
    int coughtHeartThisCycle;
    int noOfLives;
    //vars used to smoothly move frog back and forth (see frogMove())
    int shouldMove;
    double laneOffset;
    double laneDirection;
}Frog;

typedef struct{
    int rotParm;
    double offset;
    int taken;
}Heart;

//Lists for storing objects
Frog frog;
list<Car> Cars;
list<Log> Logs;
list<Turtle> Turtles;
list<Heart> Hearts;

/* Iterators used for randomly selecting turtles and hearts */
list<Turtle>::iterator turtleIt;
list<double>::iterator heartIt;


/* Vars used to make the window size rescalable */
int WinWidth;
int WinHeight;
double WinRatio;

//Var for the game ending
int gameOver;

//Var for game difficulty 
int gameDifficulty;
int livesTotal;


int main(int argc, char **argv)
{
    headX = 0;
    headY = 30;
    headZ = 35;

    atX = 0;
    atY = 0;
    atZ = 0;
    //
    gameDifficulty=1;
    livesTotal=3;
    /* Initialise  GLUT. */
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    
    /*Create a window */

    glutInitWindowSize(900, 900);
    glutInitWindowPosition(0, 0);
    glutCreateWindow(argv[0]);

    /* Functions for handling events. */
    glClearColor(0.75, 0.75, 0.75, 0);
    glutKeyboardFunc(on_keyboard);
    glutDisplayFunc(on_display);
    glutReshapeFunc(on_reshape);

    /* Timers */
    glutTimerFunc(TIMER_INTERVAL, on_timer, TIMER_ID);
    glutTimerFunc(TIMER_INTERVAL1,on_timer, TIMER_ID1);
    glutTimerFunc(TIMER_INTERVAL3, on_timer, TIMER_ID3);
    glutTimerFunc(TIMER_INTERVAL4, on_timer, TIMER_ID4);

    //Lighting part taken from the course materials
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

    /* initialising frog at the beggining */
    frog.lane=6;
    frog.offset=planeLength/2;
    frog.speed =0;
    frog.dead=0;
    frog.noOfLives=livesTotal;
    

    initializeTexture();
    glutMainLoop();

    return 0;
}

static void on_reshape(int width,int height){
    WinWidth=width;
    WinHeight=height;
    WinRatio = (double)width/height;
    glViewport(0,0,width,height);
    glutPostRedisplay();
}

static void on_keyboard(unsigned char key, int x, int y){
    switch(key){
        case 'q':
            exit(0);
            break;

        case 't':
            headX = 0;
            headY = 0.1;
            headZ = planeLength;
            glutPostRedisplay();
            break;
        case 'n':
            headX = 0;
            headY = 30;
            headZ = 35;
            glutPostRedisplay();
            break;
        case 'r':
        //in case of restart, delete all the hearts,kill the frog, and reset the lives;
            for(auto it = Hearts.begin(); it != Hearts.end(); ) {
                heartsArray.push_back(it->offset);
                it = Hearts.erase(it);
            }
            frog.dead=1;
            frog.noOfLives=livesTotal;
            break;
        case 'o':
            //toggle for the texture in case someone prefers to play without it
            texture = !texture;
            break;

        case 'l':

            //toggle the difficulty
            if(gameDifficulty==0){
                gameDifficulty++;
                livesTotal=3;
            }
            else if(gameDifficulty==1){
                gameDifficulty++;
                livesTotal=2;
            }
            else{
                gameDifficulty=0;
                livesTotal=5;
            }
            frog.dead=1;
            frog.noOfLives=livesTotal;
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
        /* Forward and backward movement is done diffenrely to make the game smoother,
        it used to be just like movement from left to right (see moveFrog()) */
        case 's':
        frog.shouldMove=1;
        frog.laneDirection=1;
        glutPostRedisplay();
        break;
        case 'w':
        frog.shouldMove=1;
        frog.laneDirection=0;
        glutPostRedisplay();
        break;
        
    }
}



void on_timer(int id) {
    switch(id){
        /* This timer is used to control the heart spawning */
        case TIMER_ID4:
            //if we have 6 hearts we are done
            if(Hearts.size()!=6){
                //randomly select from a list of possible possitions
                heartIt = select_randomly(heartsArray.begin(), heartsArray.end());
                //fill the Heart stuct
                Heart h;
                h.offset=*heartIt;
                h.taken=0;
                h.rotParm=0;
                
                //remove the current possition of the heart from the list
                //so that the next heart doesn't spawing in that place
                heartIt = heartsArray.erase(heartIt);

                for(auto it = Hearts.begin(); it != Hearts.end(); it++){
                    //removes all hearts that are not taken (its always just one heart)
                    if(!it->taken){
                        //before deleting the heart return it's possition to the list so
                        //that the next hart can possibly take that possition
                        heartsArray.push_back(it->offset);
                        //erase the heart
                        it= Hearts.erase(it);
                    }
                }
                //push back the heart now, and not eralier so that it doesn't get deleted in the for loop
                Hearts.push_back(h);
            }

            glutTimerFunc(TIMER_INTERVAL4,on_timer,TIMER_ID4);
            break;

        /* Timer used for creating cars, turtles, and logs */
        case TIMER_ID1:
            //debug
            //fprintf(stderr,"Auta ima %d, logova %d kornjaca %d\n,",Cars.size() ,Logs.size(), Turtles.size());



            //difficulty speed settings
            float speedModify;

            if(gameDifficulty==0){
                speedModify=0.8;
            }
            else if(gameDifficulty==2){
                speedModify=1.2;
            }
            else {
                speedModify=1;
            }


            /* add one car going from left to right and one car going from right to left in randomly selected lanes */
            Car c;
            //randomize lane
            srand ( time(NULL) );
            index1 = rand() % 2;
            lane1 = myArray1[index1];
            
            /* fill the car struct */

            //offset is 0 for car going from left to right
            c.offset=0;
            c.lane=lane1;
            c.speed=0.3*speedModify;
            srand(time(NULL)+2);
            c.color = rand()%3;

            //add it to the list
            Cars.push_back(c); 

            //randomize lane for cars going from right to left
            index2 = rand() % 3;
            lane2 = myArray2[index2];

            //dadsadasdada

            Car c1;
            //car offset is planeLenght for cars going from right to left;
            c1.offset=planeLength;
            c1.lane=lane2;
            c1.speed = -0.3*speedModify;
            c1.color=rand()%3;
            Cars.push_back(c1); 
            
        
            //if  condition used  to make the appearance of logs and turtles random
            if(rand()%2){
                //adding logs to the list

                //one log from every possible lane
                Log l1;
                l1.offset=planeLength;
                l1.lane=-2;
                l1.size=3;
                l1.speed = -0.1*speedModify ;
                Logs.push_back(l1);

                Log l2;
                l2.offset=planeLength;
                l2.lane=-3;
                l2.size=7;
                l2.speed = -0.20*speedModify;
                Logs.push_back(l2);

                Log l3;
                l3.offset=planeLength;
                l3.lane=-5;
                l3.size=5;
                l3.speed = -0.15*speedModify;
                Logs.push_back(l3); 

                //adding turtles to the list
                Turtle t1;
                t1.offset=0;
                t1.size=3;
                t1.lane=-1;
                t1.speed=0.15*speedModify;
                t1.red=0;
                t1.state=0;
                Turtles.push_back(t1);

                Turtle t2;
                t2.offset=0;
                t2.size=2;
                t2.lane=-4;
                t2.speed=0.20*speedModify;
                t2.red=0;
                t2.state=0;
                Turtles.push_back(t2);

            }

            /* Choose a random turtle every second to disappear */


            if(Turtles.size()!=0){
                //if there are turtles in the list one gets chosen to dissapear
                turtleIt = select_randomly(Turtles.begin(),Turtles.end());

                //if it isn't in the process of dissapearing already. Start the dissapearing cycle
                if(turtleIt->state==0)
                    //if the difficulty isn't easy, make it dissapear
                    if(gameDifficulty)
                        turtleIt->state=1;
            }

            glutTimerFunc(TIMER_INTERVAL1,on_timer,TIMER_ID1);
            break;

        /* Timer for dissapearing turtles, timer interval is 500ms
        so that every 500ms every chosen(to dissapear) turtle changes color and eventually dissapears
        */    
        case TIMER_ID3:
            //difficulty
            if(gameDifficulty==2)
                numberofBlinks=4;
            else
                numberofBlinks=6;


            for(auto it = Turtles.begin(); it != Turtles.end(); it++){
                //if the turtle has blinked enough times its time to go!
                if(it->state>numberofBlinks){
                    it = Turtles.erase(it);
                    continue;
                }
                //if it is still alive we change the color depengin on the state and increment the state
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
        
        /* The quickest timer used to update the object possition during the game, and to delete objects that are no longer needed */
        case TIMER_ID:

            //go through all the cars and update possition  and check if the need to be deleted
            for(auto it = Cars.begin(); it != Cars.end(); it++){
                //changing offset by factor of speed
                it->offset += it->speed;
                
                //if branch needed beacause different dissapearing point is deppenging on a side car comes from
                if(it->speed < 0 ){
                    if(it->offset < 0){
                        it = Cars.erase(it);
                    }
                }
                else{
                    if(it->offset > planeLength){
                        it = Cars.erase(it);
                    }
                }
            }

            //animating and removing logs
            for(auto it = Logs.begin(); it != Logs.end(); it++){
                it->offset += it->speed;
                if(it->offset < -it->size)
                    it = Logs.erase(it);
            }

            //animating and removing turtles
            for(auto it = Turtles.begin(); it != Turtles.end(); it++){
                it->offset += it->speed;
                if(it->offset > planeLength + (turtleSize*it->size) + turtleSize || it->state>numberofBlinks)
                    it = Turtles.erase(it);
            }
 
            
           //if frog runs out of lives remove all hearts that are not taken, or just update the rotPar
            for(auto it = Hearts.begin(); it != Hearts.end(); ) {
                if (frog.noOfLives<=0 && it->taken && !frog.coughtHeartThisCycle) {
                    heartsArray.push_back(it->offset);
                    it = Hearts.erase(it);
                }
                else {
                    it->rotParm = (it->rotParm + 1)%180;
                    ++it;
                }
            }

            /* FROG  */
            

            //chagne the frog offset by frog.speed which gets set if frog is on a log or a turtle
            //frog.speed gets set during the collisions check which are done in on_display()
            frog.offset += frog.speed;

            //special smooth movemnt of frog back and forth (see moveFrog() for more info)
            if(frog.shouldMove){
                moveFrog();
            }

            /*frog.speed is set back to 0 after changing the offset in case that in the next iteration 
            the frog is not on a log/turtle anymore*/
            frog.speed=0;


            //checking if frog is dead RIP
            if(frog.dead){
                //place it back to the beggining
                frog.lane=6;
                frog.offset=planeLength/2;
                frog.speed =0;
                frog.dead=0;

                //if we run out of lives the hearts dissapear and we get 3 lives back again
                if(frog.noOfLives<=0)
                    frog.noOfLives=livesTotal;
                
            }
            glutPostRedisplay();
            glutTimerFunc(TIMER_INTERVAL, on_timer, TIMER_ID);
            break;
    }
}
static void on_display(void)
{
   /* Delete previous contents of the window */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
     
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glEnable(GL_DEPTH_TEST);
    gluPerspective(53, WinRatio, 1, 800); 

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    

    gluLookAt(
        headX, headY, headZ,
        atX, atY, atZ,
        0, 0, 1
    );


    drawPlane();

    //clipping planes used to make objects entering the scene look nicer
    GLdouble plane0[] = {1, 0,0, planeLength/2};
    GLdouble plane1[] = {-1, 0, 0, planeLength/2};

    glEnable(GL_CLIP_PLANE0);
    glEnable(GL_CLIP_PLANE1);

    glClipPlane(GL_CLIP_PLANE0, plane0);
    glClipPlane(GL_CLIP_PLANE1, plane1);


    



    //going through all the cars in Cars list
    for(auto it = Cars.begin(); it != Cars.end(); it++){
        glPushMatrix();
        //Drawing the car depending on factors in Car stuct.
        glTranslatef(it->offset,0,0);
        drawCar(it->lane,it->color);
        glPopMatrix();

        /* Collisions */

        //if the frog is in the same lane as car and the offset is within the margin its a COLLISION!
        if(it->lane == frog.lane && abs(it->offset - frog.offset)<carLenght/2+frogBody/2){
            frog.dead = 1;
            frog.noOfLives--;
        }
    }
    //used to determine if the frog is on log during this cycle
    frog.movingOnLogThisCycle=0;

    for(auto it = Logs.begin(); it != Logs.end(); it++){
        //draw the Log depending on factors from Log struct
        glPushMatrix();
        glTranslatef(it->offset,0,0);
        drawLog(it->lane,it->size);
        glPopMatrix();

        /* Collisions */

        /*if the frog is in the same lane as the Log and the collision condition is 
        fulfilled  we set the frog speed to match the log speed so that the frog moves at the same
        rate as the log
        */
        if(it->lane == frog.lane && (frog.offset-it->offset)< it->size &&(frog.offset-it->offset) >0){
            frog.speed = it->speed;
            frog.movingOnLogThisCycle=1;
        }
    }
  
    //death Var set to 0 at the beggining of every display cycle
    frog.movingOnTurtleThisCycle=0;

    for(auto it = Turtles.begin(); it != Turtles.end(); it++){
        //Draw the Turtles depending on factors from Turtle struct
        glPushMatrix();
        glTranslatef(it->offset,0,0);
        drawTurtles(it->lane,it->size,it->red);
        glPopMatrix();

        /*if the frog is in the same lane as the Turtles and the collision condition is 
        fulfilled  we set the frog speed to match the turtle speed so that the frog moves at the same
        rate as the turtle
        */

        if(it->lane == frog.lane && (it->offset-frog.offset) < it->size*turtleSize && (it->offset-frog.offset) >0){
                frog.speed = it->speed;
                frog.movingOnTurtleThisCycle=1;
        }
    }

    //used to determine if the frog caught any logs during this cycle
    frog.coughtHeartThisCycle =0;
    //heartCounter used to determine if game is over
    int heartCounter=0;
    for(auto it = Hearts.begin(); it != Hearts.end(); it++){
        glPushMatrix();
        glTranslatef(it->offset,0,0);
        drawheart(it->rotParm, it->taken);
        glPopMatrix();
        //colision with the heart

        /* Collisions */

        /* if the frog is in the lane  where hearts are, the heart is not taken and they are close enough */
        if(frog.lane==-6 && abs(it->offset - frog.offset) < frogBody && !it->taken){
            it->taken=1;
            frog.coughtHeartThisCycle = 1;
        }

        
        if(it->taken){
            heartCounter++;
        }
    }
    heartCounter == 6 ? gameOver=1 : gameOver=0;



    /* Checking all the death conditions to see if frog is dead */

    /* if the frog is in the water (thats where frog.lane is less that 0) and not on a log or a turtle
    it dies unless she caught a hart this cycle */
    if(frog.lane < 0 && !(frog.movingOnLogThisCycle || frog.movingOnTurtleThisCycle)){
        if(frog.coughtHeartThisCycle){
            //if frog catches caught a heart this cycle it gets placed at the beggining
            frog.lane=6;
            frog.offset=planeLength/2;
            frog.speed =0;
        }
        //else the frog dies
        else{
            frog.dead=1;
            frog.noOfLives--;

        }
    }

    //death out out bounds
    if(frog.lane > 6 || frog.lane < -6 || frog.offset>planeLength || frog.offset<0){
        frog.dead=1;
        frog.noOfLives--;

    }

    //animate the frog using the frog struct
    glPushMatrix();
        glTranslatef(frog.offset,frog.laneOffset,0);
        drawFrog(frog.lane);
    glPopMatrix();

    
    //draw the text that gets displayed on the screen.
	drawText();


    glDisable(GL_CLIP_PLANE0);
    glDisable(GL_CLIP_PLANE1);

    glColor3f(1, 1, 1);
    glLineWidth(4);

    glutSwapBuffers();
}

/* ALL THE DRAWING FUNCTIONS */



void drawheart(int rotParm, int taken){
    glPushMatrix();
    glTranslatef(-planeLength/2,-planeWidth-marginWidth,1.5);
    glRotatef(90,1,0,0);
    taken ? glRotated(0,0,1,0) : glRotated(rotParm,0,1,0);
    glScalef(1.5,1.5,1.5);
    //code for drawing the heart taken from the internet
    glPointSize(5);
    gluOrtho2D(-2, 2, -2, 2);
    taken ? glColor3ub(0, 255, 0) : glColor3ub(255, 0, 0);  // Color Red
    glBegin(GL_POINTS);
        for (float x = -1.139; x <= 1.139; x += 0.001) 
        {
            float delta = cbrt(x*x) * cbrt(x*x) - 4*x*x + 4;
            float y1 = (cbrt(x*x) + sqrt(delta)) / 2;
            float y2 = (cbrt(x*x) - sqrt(delta)) / 2;
            glVertex2f(x, y1);
            glVertex2f(x, y2);
        }
    glEnd();
    glPopMatrix();
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

void moveFrog(){

    /*
    This is a weird function because the collisions were done before this func and they depend the
    lane of the frog beeing integer(so instead of changing the way frog colides with things we get this func) 
    To move the frog smoothly we change the laneOffset until it reaches the desired lane
    than we revert the offset and change the lane.
    */

    //set the speed of the frog depending on the movVar(direction)

    double laneSpeed;
    frog.laneDirection ? laneSpeed = +0.3 : laneSpeed = -0.3 ;
    //chagne the laneOffset used when drawing the Frog
    frog.laneOffset += laneSpeed;

    //if the lane offset reaches 1 or -1 depengin on the direction
    //we set the lane of the frog to the needed one and reset the offset
    if(frog.laneDirection ? frog.laneOffset > 1 : frog.laneOffset < -1 ){
        frog.laneOffset = 0;
        frog.laneDirection ? frog.lane++ : frog.lane--;
        frog.shouldMove=0;
    }
}


void drawText() {
	char text[50];
    int len;
	
    glPushMatrix();
    glMatrixMode (GL_PROJECTION); 
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glDisable(GL_LIGHTING);
    
    
    //Number of lives
    
    
    glColor3f(7, 0, 0);
    glRasterPos2f(-0.99, 0.9);
    sprintf(text, "Lives left: %d", frog.noOfLives);
    len = strlen(text);
    for (int i = 0; i < len; i++) {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, text[i]);
    }
    //difficulty 
    glColor3f(0, 0, 0);
    glRasterPos2f(-0.99, -0.9);
    
    //number to string;
    char gameDifString [50];
    if(gameDifficulty==0){
        strcpy(gameDifString,"easy"); 
    }
    else if (gameDifficulty==1){
        strcpy(gameDifString,"normal"); 
    }
    else
        strcpy(gameDifString,"hard");



    sprintf(text, "Difficulty: %s",gameDifString);
    len = strlen(text);
    for (int i = 0; i < len; i++) {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, text[i]);
    }




    //game over
    if(gameOver){
        glColor3f(0, 0, 0);
        glRasterPos2f(-0.1, 0.8);
        sprintf(text, "VICTORY!") ;
        len = strlen(text);
        for (int i = 0; i < len; i++) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, text[i]);
        }
        glRasterPos2f(-0.15, 0.7);
        sprintf(text, "press r to play again") ;
        len = strlen(text);
        for (int i = 0; i < len; i++) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, text[i]);
        }
    }
   glEnable(GL_LIGHTING);
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
    glTranslatef(-planeLength/2,poss*planeWidth/5,planeHeight/2);
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
      if(texture){
        glBindTexture(GL_TEXTURE_2D, names[0]);
            double waterUp = planeWidth/2;
            double waterDown = - waterUp;
            double waterRight = planeLength/2;
            double waterLeft = -waterRight;
            glPushMatrix();
                glTranslated(0, -planeWidth/2, planeHeight+0.04);
                glColor3f(0, 0, 0);
                glDisable(GL_LIGHTING);
                glBegin(GL_POLYGON);
                    glNormal3f(0, 0, 1);
                    glTexCoord2f(0, 0);
                    glVertex2f(waterLeft, waterUp);
                    glTexCoord2f(1, 0);
                    glVertex2f(waterRight, waterUp);
                    glTexCoord2f(1, 1);
                    glVertex2f(waterRight, waterDown);
                    glTexCoord2f(0, 1);
                    glVertex2f(waterLeft, waterDown);
                glEnd();
                glEnable(GL_LIGHTING);
            glPopMatrix();
            glBindTexture(GL_TEXTURE_2D, 0);
      }

    glPopMatrix();
}

void drawTurtle(int red){
    glPushMatrix();

    //one turtle
    glPushMatrix();
    //body

    //check if red
    red ? glColor3f(0.7,0.3,0.1) : glColor3f(0.3,0.7,0.1);

    glTranslatef(0,0,planeHeight+turtleSize/10);
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
void drawTurtles(int poss,int num,int red){
    glPushMatrix();
        glTranslatef(-planeLength/2-turtleSize/2,poss*planeWidth/5,0);
        drawTurtle(red);
        glTranslatef(-turtleSize,0,0);
        drawTurtle(red);
        if(num==3){
            glTranslatef(-turtleSize,0,0);
            drawTurtle(red);  
        }
    glPopMatrix();   
    
}

//function taken from course materials
static void initializeTexture(void)
{
    /* Objekat koji predstavlja teskturu ucitanu iz fajla. */
    Image * image;


    /* Ukljucuje se testiranje z-koordinate piksela. */
    glEnable(GL_DEPTH_TEST);

    /* Ukljucuju se teksture. */
    glEnable(GL_TEXTURE_2D);

    glTexEnvf(GL_TEXTURE_ENV,
              GL_TEXTURE_ENV_MODE,
              GL_REPLACE);

    /*
     * Inicijalizuje se objekat koji ce sadrzati teksture ucitane iz
     * fajla.
     */
    image = image_init(0, 0);

    /* Kreira se prva tekstura. */
    image_read(image,(char *)FILENAME0);
    //asd
    /* Generisu se identifikatori tekstura. */
    glGenTextures(2, names);

     glBindTexture(GL_TEXTURE_2D, names[0]);
    glTexParameteri(GL_TEXTURE_2D,
                    GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D,
                    GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
                 image->width, image->height, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, image->pixels);


    glBindTexture(GL_TEXTURE_2D, 0);
    image_done(image);
}
