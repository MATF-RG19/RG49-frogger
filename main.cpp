#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>
#include <math.h>
#include <algorithm>
#include <list>
#include<time.h>
#include <iterator>
#include <random>
#include <string.h>


using namespace std;

#define TIMER_INTERVAL 20
#define TIMER_INTERVAL1 1000
#define TIMER_INTERVAL3 500
#define TIMER_INTERVAL4 5500
#define TIMER_ID 0
#define TIMER_ID1 1
#define TIMER_ID3 3
#define TIMER_ID4 4


//temp
int pomerio=0;
int activeHeart;

/* Deklaracije callback funkcija. */
static void on_keyboard(unsigned char key, int x, int y);
static void on_display(void);
static void on_timer(int id);
static void on_reshape(int,int);

void drawText();
void drawCoord();
void drawPlane();
void drawFrog(int);
void drawCar(int,int);
void drawLog(int,int);
void drawTurtle(int);
void drawTurtles(int,int,int);
void drawheart(int,int);

//taken from the internet for randomly selecting an item from collection
template<typename Iter, typename RandomGenerator>
Iter select_randomly(Iter start, Iter end, RandomGenerator& g) {
    std::uniform_int_distribution<> dis(0, std::distance(start, end) - 1);
    std::advance(start, dis(g));
    return start;
}
//taken from the internet
template<typename Iter>
Iter select_randomly(Iter start, Iter end) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    return select_randomly(start, end, gen);
}

int globalRandInt=0;

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
//used for dissapearing
int numberofBlinks = 6;


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

//list for randomising heart
list<double> heartsArray = {planeLength*0.05, planeLength*0.20, planeLength*0.35, planeLength*0.50, planeLength*0.65, planeLength*0.80};

//ranomising turtles
int dissTurtle;


//var for detecting death



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
    if turtle is randmly selected to dissapear(happens every second)
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
    //vars used to smoothly move frog
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

//tmp
list<Turtle>::iterator diss;
list<double>::iterator heartIt;

int WinWidth;
int WinHeight;
double WinRatio;

double tmpSpeed;
int noTimes;


/*
    This is a weird function because the collisions were done before this func and they depend the
    lane of the frog beeing integer(so instead of changing the way frog colides with things we get this func) 
    To move the frog smoothly we change the offset until it reaces the desired lane
    than we revert the offset and change the lane.
*/

void moveFrog(){
    //set the speed depengin on the movVar(direction)

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
    glutReshapeFunc(on_reshape);

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
    frog.speed =0;
    frog.dead=0;
    frog.noOfLives=3;


    

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
            headZ = planeLength;
            glutPostRedisplay();
            break;
        case 'r':
            headX = 0;
            headY = planeLength;
            headZ = planeLength/2+planeWidth;
            pomerio=0;
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
            glutTimerFunc(TIMER_INTERVAL3, on_timer, TIMER_ID3);
            glutTimerFunc(TIMER_INTERVAL4, on_timer, TIMER_ID4);

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
        case TIMER_ID4:
            //if we have 6 hears we are done
            if(Hearts.size()!=6){
                //randomly select from a list of possible possitions
                heartIt = select_randomly(heartsArray.begin(), heartsArray.end());
               
                //fill the Heart stuct
                Heart h;
                h.offset=*heartIt;
                h.taken=0;
                h.rotParm=0;
                
                //remove the current possition of the heart from the list
                //so that the next heart doesnt spawing in that place
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
            
        
            //if statement to make the appearance more random
            if(rand()%2){
                //adding logs to the list

                Log l1;
                l1.offset=planeLength;
                l1.lane=-2;
                l1.size=3;
                l1.speed = -0.1 ;
                Logs.push_back(l1);

                Log l2;
                l2.offset=planeLength;
                l2.lane=-3;
                l2.size=7;
                l2.speed = -0.20;
                Logs.push_back(l2);

                Log l3;
                l3.offset=planeLength;
                l3.lane=-5;
                l3.size=5;
                l3.speed = -0.15;
                Logs.push_back(l3); 

                //adding turtles to the list
                Turtle t1;
                t1.offset=0;
                t1.size=3;
                t1.lane=-1;
                t1.speed=0.15;
                t1.red=0;
                t1.state=0;
                Turtles.push_back(t1);

                Turtle t2;
                t2.offset=0;
                t2.size=2;
                t2.lane=-4;
                t2.speed=0.20;
                t2.red=0;
                t2.state=0;
                Turtles.push_back(t2);

                //every second a radnom turtle gets chosen to dissapear
                diss = select_randomly(Turtles.begin(),Turtles.end());
                //if it isnt dissapearing already make it dissapear
                if(diss->state==0)
                    diss->state=1;
            }
            glutTimerFunc(TIMER_INTERVAL1,on_timer,TIMER_ID1);
            break;
        case TIMER_ID3:
            /*special timer for dissapearing turtles, timer interval is 500ms
            so that every 500ms the chosen(to dissapear) turtle changes color and eventualy dissapears
            */
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
        case TIMER_ID:

            //the quickest timer used to animate and delete objects that are no longer needed
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
            
            //frog only moves if it is on the upper part of the map(Thats where the lane number is negative)
                frog.offset += frog.speed;
            

            //if frog runs out of lives remove all hearts that are not taken, or just update the rotPar
            for (auto it = Hearts.begin(); it != Hearts.end(); ) {
                if (frog.noOfLives==0 && it->taken && !frog.coughtHeartThisCycle) {
                    heartsArray.push_back(it->offset);
                    it = Hearts.erase(it);
                }
                else {
                    it->rotParm = (it->rotParm + 1)%180;
                    ++it;
                }
            }

            if(frog.shouldMove){
                moveFrog();
            }

            //frog.speed is set to 0 after changing the offset in case that in the next itteration its not on the object anymore
            frog.speed=0;
            //checking if frog is dead RIP
            if(frog.dead){
                frog.lane=6;
                frog.offset=planeLength/2;
                frog.speed =0;
                if(frog.noOfLives==0)
                    frog.noOfLives=3;
                
            }
                frog.dead=0;
            glutPostRedisplay();
            glutTimerFunc(TIMER_INTERVAL, on_timer, TIMER_ID);
            break;
    }
}
/*TODO*/ //NE BRISE LEPO SVE OBJEKTE IZ LISTE
static void on_display(void)
{
   /* Brise se prethodni sadrzaj prozora. */
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
    //drawCoord();
    drawPlane();

    //clipping planes for better looking entereing and dissapearing of objects
    GLdouble plane0[] = {1, 0,0, planeLength/2};
    GLdouble plane1[] = {-1, 0, 0, planeLength/2};

    glEnable(GL_CLIP_PLANE0);
    glEnable(GL_CLIP_PLANE1);

    glClipPlane(GL_CLIP_PLANE0, plane0);
    glClipPlane(GL_CLIP_PLANE1, plane1);


    



    //going through all the cars in Cars list
    for(auto it = Cars.begin(); it != Cars.end(); it++){
        glPushMatrix();
        //changing the possition of car depengin on the offset and drawing said car depending on factors in Cars stuct.
        glTranslatef(it->offset,0,0);
        drawCar(it->lane,it->color);
        glPopMatrix();


        //collisions
        //if the frog is in the same lane as car and the offset is within the margin its a COLLISION!
        if(it->lane == frog.lane && abs(it->offset - frog.offset)<carLenght/2+frogBody/2){
            frog.dead = 1;
            frog.noOfLives--;
        }
    }
    //used to determine if the frog is on any object during this cycle
    frog.movingOnLogThisCycle=0;

    for(auto it = Logs.begin(); it != Logs.end(); it++){
        glPushMatrix();
        glTranslatef(it->offset,0,0);
        drawLog(it->lane,it->size);
        glPopMatrix();
        /*if the frog is in the same lane as the Log and the collision condition is 
        fulfilled  we set the frog speed to match the log speed
        */
        if(it->lane == frog.lane && (frog.offset-it->offset)< it->size &&(frog.offset-it->offset) >0){
            frog.speed = it->speed;
            frog.movingOnLogThisCycle=1;
        }
    }
  
    //death Var set to 0 at the beggining of every display cycle
    frog.movingOnTurtleThisCycle=0;

    for(auto it = Turtles.begin(); it != Turtles.end(); it++){
        //animation
        glPushMatrix();
        glTranslatef(it->offset,0,0);
        drawTurtles(it->lane,it->size,it->red);
        glPopMatrix();

        if(it->lane == frog.lane && (it->offset-frog.offset) < it->size*turtleSize && (it->offset-frog.offset) >0){
                frog.speed = it->speed;
                frog.movingOnTurtleThisCycle=1;
        }
    }

    frog.coughtHeartThisCycle =0;

    for(auto it = Hearts.begin(); it != Hearts.end(); it++){
        glPushMatrix();
        glTranslatef(it->offset,0,0);
        drawheart(it->rotParm, it->taken);
        glPopMatrix();

        if(frog.lane==-6 && abs(it->offset - frog.offset) < frogBody){
            //the condition is here so that you cant catch taken hearts more than once
            if(!it->taken){
                it->taken=1;
                frog.coughtHeartThisCycle = 1;
            }
        }

    }
    //checking the death cond 
    if(frog.lane < 0 && !(frog.movingOnLogThisCycle || frog.movingOnTurtleThisCycle)){
        if(frog.coughtHeartThisCycle){
            //if frog cought a car it doesnt die it just get reset to the begginging
            frog.lane=6;
            frog.offset=planeLength/2;
            frog.speed =0;
        }
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

    //animate the frog
    glPushMatrix();
        glTranslatef(frog.offset,frog.laneOffset,0);
        drawFrog(frog.lane);
    glPopMatrix();


    glDisable(GL_LIGHTING);
		drawText();
	glEnable(GL_LIGHTING);   


    glDisable(GL_CLIP_PLANE0);
    glDisable(GL_CLIP_PLANE1);

    glColor3f(1, 1, 1);
    glLineWidth(4);

    glutSwapBuffers();
}

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


void drawText() {
	char text[50];
	glColor3f(0,0,0);
    int len;
	
	glPushMatrix();
		glRasterPos3f(planeLength/2, -planeWidth-marginWidth*3, 0);
		sprintf(text, "Lives left: %d", frog.noOfLives);
		len = strlen(text);
		for (int i = 0; i < len; i++) {
			glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, text[i]);
		}
	glPopMatrix();
    if(Hearts.size()==6){
        glPushMatrix();
            glRasterPos3f(0, -planeWidth-marginWidth*2,0 );
            sprintf(text, "VICTORY!", frog.speed);
            len = strlen(text);
            for (int i = 0; i < len; i++) {
                glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, text[i]);
            }
        glPopMatrix();
    }
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
//this needs refactoring
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