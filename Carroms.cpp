/* 
   NEED TO DO :
   3) Inelastic collision.
   4) Mouse Functions.
   5) Refactor
   6) MultiPlayer If possible;
   7) Sounds;
*/

#include <GL/gl.h>
#include <GL/glut.h>
#include <bits/stdc++.h> 

#define PI 3.1415926535
#define CF 0.0174532925
using namespace std;

//Globals
bool flag=0; //For striker;
bool HelpFlag=0; // Displaying Help;
float Intial_velocity_x=5.0f; // Initial X-Vel of striker; default,min : 5; max:15;
float Intial_velocity_y=5.0f; // Initial Y-Vel of striker; default,min : 5; max:15;
float Dir=0.0f; // Direction of striking;
int score=30; // Score;
int MyCoins; // Either White/Black;
int Timer=0; // count to update score(1s -> score-=1);
int GameEnd=0; // Flag to take care of end;

float Speed(float x, float y)
{
	return sqrt(x*x + y*y);
}

float Distance(float x1, float y1, float x2, float y2)
{
	float x=(x1-x2);
	x*=x;
	float y=(y1-y2);
	y*=y;
	return sqrt(x+y);
}

class Coins
{
	public :
		float x,y,velx,vely,rad,mass;
		bool pocketed;
		void init(float a, float b, float c, float d, float e, float f)
		{
			x = a;
			y = b;
			velx = c;
			vely = d;
			rad = e;
			mass = f;
			pocketed=0;
		}
	    void Intial_up()
		{
			if(x + rad > 968 || x - rad < 342)
			{
				velx *= -1;
			}
			if(y + rad > 668 || y - rad < 42)
			{
				vely *= -1;
			}
			x += velx;
			y += vely;
			if(abs(velx)>0)
			{
				velx-=0.005*(velx);
			}
			if(abs(vely)>0)
			{
				vely-=0.005*(vely);
			}
			if(Speed(velx,vely)<=0.01)
			{
				velx=0;
				vely=0;
			}
			/* BE Careful, the below statement is only for striker !!!!! */
			if(abs(velx)==0 && abs(vely)==0 && mass==2 && flag==1)
			{
				x=655.0f;
				y=118.0f;
				flag=0;
			}
		}
};

Coins C[10]; // 1 Striker + 8 coins(4w + 4b) + 1 Red

void drawCircle(float cx,float cy,float r,char type)
{
	if(type=='P')
	{
		glBegin(GL_POLYGON);
	}
	else if(type=='L')
	{
		glBegin(GL_POINTS);
	}
	for(int ii=0;ii<1000;ii++) 
	{
		float theta = 2.0f * PI * float(ii) / float(1000);
		float x = r * cosf(theta);
		float y = r * sinf(theta);
		glVertex2f(x + cx, y + cy);
	}
	glEnd();
}

void drawArrow()
{
	float CenX=C[0].x;
	float CenY=C[0].y;
	float theta=CF*Dir;
	float EndX=CenX+60*(cosf(theta));
	float EndY=CenY+60*(sinf(theta));
	float x1=10*(cosf(165*CF + theta));
	float y1=10*(sinf(165*CF + theta));
	float x2=10*(cosf(195*CF + theta));
	float y2=10*(sinf(195*CF + theta));
	glColor3f(0,0,0);
	glBegin(GL_LINES);
	glVertex2f(CenX,CenY);
	glVertex2f(EndX,EndY);
	glEnd();
	glColor3f(0,0,0);
	glBegin(GL_LINES);
	glVertex2f(EndX,EndY);
	glVertex2f(EndX+x1,EndY+y1);
	glEnd();
	glColor3f(0,0,0);
	glBegin(GL_LINES);
	glVertex2f(EndX,EndY);
	glVertex2f(EndX+x2,EndY+y2);
	glEnd();
}

void drawBitmapText(char* string, float x, float y, float z)
{
	char* tx;
	glRasterPos3f(x,y,z);
	for (tx = string; *tx != '\0'; tx++)
	{
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *tx);
	}
}


void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	if(GameEnd==1)
	{
		glPushMatrix();
		char scoreStr[100000]={'\0'};
		glColor3f(1,0,0);
		sprintf(scoreStr,"GAME OVER");
		drawBitmapText(scoreStr,600,650,0.0f);
		sprintf(scoreStr,"Your Score : %d",(int)score);
		drawBitmapText(scoreStr,600,600,0.0f);
		//sprintf(scoreStr,"Your Score : %d",(int)score);
		//drawBitmapText(scoreStr,600,600,0.0f);
	}
	else
	{
		if(HelpFlag==0)
		{
			glPushMatrix();
			char scoreStr[100000]={'\0'};
			glColor3f(1,0,1);
			sprintf(scoreStr,"INSTRUCTIONS");
			drawBitmapText(scoreStr,600,650,0.0f);
			sprintf(scoreStr,"1) Score and Speed of striker is shown at right side of board.");
			drawBitmapText(scoreStr,100,620,0.0f);

			sprintf(scoreStr,"2) You can adjust speed using Up arrow and Down arrow keys.");
			drawBitmapText(scoreStr,100,590,0.0f);

			sprintf(scoreStr,"3) Rotate striker using 'a' and 'c' keys.");
			drawBitmapText(scoreStr,100,560,0.0f);

			sprintf(scoreStr,"4) Use left and right arrow keys to move the striker along the line.");
			drawBitmapText(scoreStr,100,530,0.0f);

			sprintf(scoreStr,"5) Hit Space bar to strike.");
			drawBitmapText(scoreStr,100,500,0.0f);

			sprintf(scoreStr,"6) Red: 50 points, your coin : 10 points, other coin : -5 points");
			drawBitmapText(scoreStr,100,470,0.0f);

			sprintf(scoreStr,"7) Each second costs you 1 point, Initial Score is 30 points");
			drawBitmapText(scoreStr,100,440,0.0f);

			sprintf(scoreStr,"Press 'w' to choose white coins, other wise press 'b' ");
			drawBitmapText(scoreStr,400,400,0.0f);
			glPopMatrix();

		}
		else
		{
			glPushMatrix();
			/*Outer Border starts*/
			glColor3f(0.647059,0.164706,0.164706);
			glBegin(GL_POLYGON);
			glVertex2f(300,0);
			glVertex2f(300,710);
			glVertex2f(1010,710);
			glVertex2f(1010,0);
			glEnd();
			/*Outer Border Ends*/
			/*Inner Border starts*/
			glColor3f(0.937f, 0.780f, 0.619f);
			glBegin(GL_POLYGON);
			glVertex2f(342,62);
			glVertex2f(342,648);
			glVertex2f(362,668);
			glVertex2f(948,668);
			glVertex2f(968,648);
			glVertex2f(968,62);
			glVertex2f(948,42);
			glVertex2f(362,42);
			glEnd();
			glColor3f(1.0f,1.0f,1.0f);
			drawCircle(362,62,20,'P');
			drawCircle(362,648,20,'P');
			drawCircle(948,648,20,'P');
			drawCircle(948,62,20,'P');
			/*Inner Border(including holes) Ends*/
			/*Central Lines starts*/
			glBegin(GL_LINES);
			glColor3f(0.647059,0.164706,0.164706);
			glVertex2f(400,114);
			glVertex2f(400,596);
			glVertex2f(414,610);
			glVertex2f(896,610);
			glVertex2f(910,596);
			glVertex2f(910,114);
			glVertex2f(896,100);
			glVertex2f(414,100);
			glEnd(); 

			glBegin(GL_LINES);
			glColor3f(0.647059,0.164706,0.164706);
			glVertex2f(422,125);
            glVertex2f(422,584);
			glVertex2f(427,592);
			glVertex2f(883,592);
			glVertex2f(887,584);
		    glVertex2f(887,125);
			glVertex2f(883,119);
		    glVertex2f(427,119);
			glEnd();

			drawCircle(414,114,14,'L');
			drawCircle(414,596,14,'L');
			drawCircle(896,596,14,'L');
			drawCircle(896,114,14,'L');
			/*Central Lines ends*/
			/*Central Rings starts*/
			drawCircle(657,374,100,'L');
			drawCircle(657,374,91,'L');
			drawCircle(657,374,14,'L');
			/*Central Rings ends*/
			/*Printing Vel start*/
			glColor3f(1.0f, 0.0f, 1.0f);
			char scoreStr[100000]={'\0'};
			char My[12]={'\0'};
			if(MyCoins%2==1)
			{
				strcpy(My,"Whites");
			}
			else
			{
				strcpy(My,"Blacks");
			}
			sprintf(scoreStr,"Your Coins : %s",My);
			drawBitmapText(scoreStr,1058,630,0.0f);
			sprintf(scoreStr,"Speed : %d",(int)Speed(Intial_velocity_x,Intial_velocity_y));
			drawBitmapText(scoreStr,1058,593,0.0f);
			sprintf(scoreStr,"Score : %d",(int)score);
			drawBitmapText(scoreStr,1058,550,0.0f);
			/*Vel end*/
			if(flag==0)
			{
				drawArrow();
			}
			/*Striker and Coins starts*/
			glPushMatrix();
			glColor3f(0.0f, 1.0f, 0.0f);
			drawCircle(C[0].x,C[0].y, C[0].rad,'P'); // Striker
			if(C[1].pocketed==0)
			{
				glColor3f(1.0f, 1.0f, 1.0f);
				drawCircle(C[1].x,C[1].y, C[1].rad,'P'); // White
			}
			if(C[2].pocketed==0)
			{
				glColor3f(0.0f, 0.0f, 0.0f);
				drawCircle(C[2].x,C[2].y, C[2].rad,'P'); // Black
			}
			if(C[3].pocketed==0)
			{
				glColor3f(1.0f, 1.0f, 1.0f);
				drawCircle(C[3].x,C[3].y, C[3].rad,'P'); // White
			}
			if(C[4].pocketed==0)
			{
				glColor3f(0.0f, 0.0f, 0.0f);
				drawCircle(C[4].x,C[4].y, C[4].rad,'P'); // Black
			}
			if(C[5].pocketed==0)
			{
				glColor3f(1.0f, 1.0f, 1.0f);
				drawCircle(C[5].x,C[5].y, C[5].rad,'P'); // White
			}
			if(C[6].pocketed==0)
			{
				glColor3f(0.0f, 0.0f, 0.0f);
				drawCircle(C[6].x,C[6].y, C[6].rad,'P'); // Black
			}
			if(C[7].pocketed==0)
			{
				glColor3f(1.0f,1.0f,1.0f);
				drawCircle(C[7].x,C[7].y, C[7].rad,'P'); // White
			}
			if(C[8].pocketed==0)
			{
				glColor3f(0.0f,0.0f,0.0f);
				drawCircle(C[8].x,C[8].y, C[8].rad,'P'); // White
			}
			if(C[9].pocketed==0)
			{
				glColor3f(1.0f,0.0f,0.0f);
				drawCircle(C[9].x,C[9].y, C[9].rad,'P'); // Red
			}
			glPopMatrix();
			/*Striker and Coins ends*/
			glPopMatrix();
		}
	}
	glFlush();
	glutSwapBuffers();
}

void init (void)
{
	glClearColor (1.0,1.0,1.0,1.0);
	glShadeModel (GL_FLAT);
	C[0].init(655.0f,118.0f,0.0f,0.0f,18.0f,2); // Striker
	C[1].init(586.0f,325.471f,0.0f,0.0f,14.0f,1.3f); // White
	C[2].init(740.0f,375.0f,0.0f,0.0f,14.0f,1.2f); // Black
	C[3].init(728.0f,422.528f,0.0f,0.0f,14.0f,1.3f); // White
	C[4].init(657.0f,460.0f,0.0f,0.0f,14.0f,1.3f); // Black
	C[5].init(586.0f,422.528f,0.0f,0.0f,14.0f,1.3f); // White
	C[6].init(657.0f,288.0f,0.0f,0.0f,14.0f,1.2f); // Black
	C[7].init(728.0f,325.471f,0.0f,0.0f,14.0f,1.3f); // White
	C[8].init(577.0f,375.0f,0.0f,0.0f,14.0f,1.4f); // Black
	C[9].init(657.0f,375.0f,0.0f,0.0f,14.0f,1.4f); // Red
	
}

void reshape (int w, int h)
{
	glViewport (0, 0, (GLsizei) w, (GLsizei) h);
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity ();
	gluOrtho2D (0.0, (GLdouble) w, 0.0, (GLdouble) h);
	glMatrixMode(GL_MODELVIEW); 
	glLoadIdentity();
}

bool check(int i, int j) //Check for collision between coins.
{
	float x=(C[i].x-C[j].x);
	x*=x;
	float y=(C[i].y-C[j].y);
	y*=y;
	float r=(C[i].rad+C[j].rad);
	r*=r;
	if(x+y <= r)
	{
		return 1;
	}
	return 0;
}

void Check(int direction)
{
	if(flag==0)
	{
		bool clearflag=0;
		while(clearflag==0)
		{
			clearflag=1;
			float x1=C[0].x;
			float y1=C[0].y;
			if(x1<=414)
			{
				C[0].x=414;
				x1=414;
			}
			if(x1>=896)
			{
				C[0].x=896;
				x1=896;
			}
			float r1=C[0].rad;
			for(int i=1;i<=7;i=i+1)
			{
				float x2=C[i].x;
				float y2=C[i].y;
				float r2=C[i].rad;
				float D=Distance(x1,y1,x2,y2);
				if(D<=(r1+r2))
				{
					clearflag=0;
					float T=sqrt((r1+r2)*(r1+r2) - (y2-y1)*(y2-y1));
					if(direction==0)
					{
						C[0].x-=T;
					}
					else
					{
						C[0].x+=T;
					}
				}
			}
		}
	}
}

void GameCheck()
{
	bool f=0;
	for(int i=1;i<=9;i=i+1)
	{
		if(i==9 || i%2==MyCoins)
		{
			if(C[i].pocketed==0)
			{
				f=1;
				break;
			}
		}
	}
	if(f==0)
	{
		GameEnd=1;
	}
}

void update(int val)
{
	for(int i=0;i<10;i=i+1)
	{
		C[i].Intial_up();
		//int extra = 0;
		float Dis1=Distance(C[i].x, C[i].y,362,62);
		float Dis2=Distance(C[i].x, C[i].y,362,648);
		float Dis3=Distance(C[i].x, C[i].y,948,648);
		float Dis4=Distance(C[i].x, C[i].y,948,62);
		if(C[i].pocketed == 0 && (Dis1 <= 20 || Dis2 <= 20 || Dis3 <= 20 || Dis4 <= 20))
		{
			if(i!=0)
			{
				C[i].pocketed=1;
				if(i>0 && i<9)
				{
					if(i%2==MyCoins)
					{
						score+=10;
					}
					else
					{
						score-=5;
					}
				}
				else if(i==9)
				{
					score+=50;
				}
				C[i].x=0;
				C[i].y=0;
			}
			else
			{
				C[i].x=655;
				C[i].y=118;
				C[i].velx=0;
				C[i].vely=0;
				score-=5;
			}
		}
	}
	for(int i=0;i<10;i=i+1)
	{
		for(int j=0;j<10;j=j+1)
		{
			if(i!=j)
			{
				bool res=check(i,j);
				if(res==1)
				{
					float FX=((C[i].velx * (C[i].mass - C[j].mass))+(2*C[j].mass * C[j].velx))/(C[i].mass + C[j].mass);
					float FY=((C[i].vely * (C[i].mass - C[j].mass))+(2*C[j].mass * C[j].vely))/(C[i].mass + C[j].mass);
					float SX=((C[j].velx * (C[j].mass - C[i].mass))+(2*C[i].mass * C[i].velx))/(C[i].mass + C[j].mass);
					float SY=((C[j].vely * (C[j].mass - C[i].mass))+(2*C[i].mass * C[i].vely))/(C[i].mass + C[j].mass);
					C[i].velx = FX;
					C[i].vely = FY;
					C[j].velx = SX;
					C[j].vely = SY;
					C[i].Intial_up();
					C[j].Intial_up();
				}
			}
		}
	}
	Check(0);
	GameCheck();
	Timer+=10;
	if(Timer%1000==0 && GameEnd==0 && HelpFlag==1)
	{
		score-=1;
	}
	glutTimerFunc(10, update, 0);
}

void NormalKeys(unsigned char c, int x, int y)
{
	if(c==' ' && flag==0)
	{
		flag=1;
		//float S=Speed(Intial_velocity_x,Intial_velocity_y);
		float theta=CF*Dir;
		C[0].velx=Intial_velocity_x*cosf(theta);
		C[0].vely=Intial_velocity_y*sinf(theta);
	}
	else if(c=='a' && flag==0)
	{
		Dir+=2;
	}
	else if(c=='c' && flag==0)
	{
		Dir-=3;
	}
	else if(c=='w'&& HelpFlag==0)
	{
		MyCoins=1;
		HelpFlag=1;
	}
	else if(c=='b' && HelpFlag==0)
	{
		MyCoins=0;
		HelpFlag=1;
	}
	else if(c=='e')
	{
		exit(0);
	}
}

void SpecialKeys(int c, int x, int y)
{
	if(c==GLUT_KEY_LEFT)
	{
		if(flag==0)
		{
			C[0].x -= 12;
			Check(0);
		}
	}
	if(c==GLUT_KEY_RIGHT)
	{
		if(flag==0)
		{
			C[0].x += 12;
			Check(1);
		}
	}
	if(c==GLUT_KEY_UP)
	{
		if(flag==0)
		{
			Intial_velocity_x+=0.5;
			Intial_velocity_y+=0.5;
			if(Intial_velocity_x>=15)
			{
				Intial_velocity_x=15;
			}
			if(Intial_velocity_y>=15)
			{
				Intial_velocity_y=15;
			}
		}
	}
	if(c==GLUT_KEY_DOWN)
	{
		Intial_velocity_x-=0.5;
		Intial_velocity_y-=0.5;
		if(Intial_velocity_x<=5)
		{
			Intial_velocity_x=5;
		}
		if(Intial_velocity_y<=5)
		{
			Intial_velocity_y=5;
		}
	}
}
/*
void mouse(int button, int state, int x, int y)
{
	GLint viewport[4];
	GLdouble mvmatrix[16], projmatrix[16];
	GLint realy;
	GLdouble wx, wy, wz;
	switch (button) {
		case GLUT_LEFT_BUTTON:
			if (state == GLUT_DOWN) {
				glGetIntegerv (GL_VIEWPORT, viewport);
				glGetDoublev (GL_MODELVIEW_MATRIX, mvmatrix);
				glGetDoublev (GL_PROJECTION_MATRIX, projmatrix);
				realy = viewport[3] - (GLint) y - 1;
				printf ("Coordinates at cursor are (%4d, %4d)\n",
						x, realy);
				gluUnProject ((GLdouble) x, (GLdouble) realy, 0.0,
						mvmatrix, projmatrix, viewport, &wx, &wy, &wz);
				printf ("World coords at z=0.0 are (%f, %f, %f)\n",
						wx, wy, wz);
				gluUnProject ((GLdouble) x, (GLdouble) realy, 1.0,
						mvmatrix, projmatrix, viewport, &wx, &wy, &wz);
				printf ("World coords at z=1.0 are (%f, %f, %f)\n",
						wx, wy, wz);
			}
			break;
		case GLUT_RIGHT_BUTTON:
			if (state == GLUT_DOWN)
				exit(0);
			break;
		default:
			break;
	}
}
*/
int main(int argc, char** argv)
{
	//Initialize GLUT
	glutInit(&argc, argv);
	glutInitDisplayMode (GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(2000,2000);
	glutInitWindowPosition(100,100);

	//Create the window
	glutCreateWindow("My Carrom Board");
	init ();

	//Set handler functions
	glutDisplayFunc(display); 
	glutIdleFunc(display);
	glutReshapeFunc(reshape);
	//glutMouseFunc(mouse);
	glutKeyboardFunc(NormalKeys);  
	glutSpecialFunc(SpecialKeys);
	glutTimerFunc(10, update, 0);
	glutMainLoop();
	return 0;
}
