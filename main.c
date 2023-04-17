#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>
#include <math.h>
#define PI 3.1415926535
#define P2 PI/2
#define P3 3*PI/2
#define DR 0.0174533

int mapX=8,mapY=8,mapS=64;

int map[]=
{
	1,1,1,1,1,1,1,1,
	1,0,0,0,0,0,0,1,
	1,0,0,0,0,0,0,1,
	1,0,0,1,1,0,0,1,
	1,0,0,0,0,0,0,1,
	1,0,0,0,0,0,0,1,
	1,0,0,0,0,0,1,1,
	1,1,1,1,1,1,1,1,
};

void drawMap()
{
	int x,y,xo,yo;
	for(y=0;y<mapY;y++)
	{
		for(x=0;x<mapX;x++)
		{
			if(map[y*mapX+x]==1) { glColor3f(1,1,1); } else { glColor3f(0,0,0); }
			xo=x*mapS; yo=y*mapS;
			glBegin(GL_QUADS);
			glVertex2i(xo+1, yo+1);
			glVertex2i(xo+1, yo+mapS-1);
			glVertex2i(xo+mapS-1, yo+mapS-1);
			glVertex2i(xo+mapS-1, yo+1);
			glEnd();
		}
	}
}


float px,py,pdx,pdy,pa; //player coords
void buttons(unsigned char key,int x,int y)
{
	if(key=='a'){ pa -= 0.1; if (pa<0) { pa+=2*PI; } pdx=cos(pa)*5; pdy=sin(pa)*5; }
	if(key=='d'){ pa += 0.1; if (pa>2*PI) { pa-=2*PI; } pdx=cos(pa)*5; pdy=sin(pa)*5; }
	if(key=='w'){ px+=pdx; py+=pdy; }
	if(key=='s'){ px-=pdx; py-=pdy; }
	glutPostRedisplay();
}


void drawPlayer()
{
	glColor3f(1,1,0);
	glPointSize(8);
	glBegin(GL_POINTS);
	glVertex2i(px,py);
	glEnd();
}

float dist(float ax, float ay, float bx, float by, float ang)
{
	return ( sqrt((bx-ax)*(bx-ax) + (by-ay)*(by-ay)) );	
}

void drawRays2D()
{
	int r,mx,my,mp,dof; float rx,ry,ra,xo,yo,finDist;
	ra=pa-DR*30; if (ra<0) { ra+=2*PI; } if (ra>2*PI) { ra-=2*PI; }
	for(r=0;r<60;r++)
	{
		//Horizontal hit
		float distH=10000000, hx=px, hy=py;
		dof=0;
		float aTan=-1/tan(ra);
		if(ra>PI) { ry=(((int)py>>6)<<6)-0.0001; rx=(py-ry)*aTan+px; yo=-64; xo=-yo*aTan; } //down
		if(ra<PI) { ry=(((int)py>>6)<<6)+64; rx=(py-ry)*aTan+px; yo=64; xo=-yo*aTan; } //up
		if (ra==0 || ra==PI) { rx=px; ry=py; dof=8; } //straight left || right
		while(dof<8)
		{
			mx=(int)(rx)>>6; my=(int)(ry)>>6; mp=my*mapX+mx;
			if(mp>0 && mp<mapX*mapY && map[mp]==1) { hx=rx; hy=ry; distH=dist(px, py, hx, hy, ra); dof=8; }
			else { rx+=xo; ry+=yo; dof++; }
		}
		//Vertical hit
		float distV=10000000, vx=px, vy=py;
		dof=0;
		float nTan=-tan(ra);
		if(ra>P2 && ra<P3) { rx=(((int)px>>6)<<6)-0.0001; ry=(px-rx)*nTan+py; xo=-64; yo=-xo*nTan; } //left
		if(ra<P2 || ra>P3) { rx=(((int)px>>6)<<6)+64; ry=(px-rx)*nTan+py; xo=64; yo=-xo*nTan; } //right
		if (ra==P2 || ra==P3) { rx=px; ry=py; dof=8; } //straight up || down
		while(dof<8)
		{
			mx=(int)(rx)>>6; my=(int)(ry)>>6; mp=my*mapX+mx;
			if(mp>0 && mp<mapX*mapY && map[mp]==1) { vx=rx; vy=ry; distV=dist(px, py, vx, vy, ra); dof=8; }
			else { rx+=xo; ry+=yo; dof++; }
		}
		finDist=distV;
		glColor3f(0,0,1);
		if(distV>distH) { rx=hx; ry=hy; finDist=distH; glColor3f(0,0,0.8); }
		
		glLineWidth(1);glBegin(GL_LINES);glVertex2i(px,py);glVertex2i(rx,ry);glEnd();
		int lineHeight = (mapS*320)/(finDist); if(lineHeight>320){ lineHeight=320;}                     //line height and limit
  		int lineOffset = 160 - (lineHeight>>1);      
		glLineWidth(8); glBegin(GL_LINES); glVertex2i(r*8+530,lineOffset); glVertex2i(r*8+530,lineHeight+lineOffset); glEnd();
		ra+=DR; if (ra<0) { ra+=2*PI; } if (ra>2*PI) { ra-=2*PI; }
	}
}


void display()
{
 glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
 drawMap();
 drawPlayer();
 drawRays2D();
 glutSwapBuffers();
}

void init()
{
	glClearColor(0.3,0.3,0.3,0);
	gluOrtho2D(0,1024,512,0);
	px=300; py=300;
}

int main(int argc, char** argv)
{ 
 glutInit(&argc, argv);
 glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
 glutInitWindowSize(1024, 512);
 glutCreateWindow("Raycaster");
 init();
 glutDisplayFunc(display);
 glutKeyboardFunc(buttons);
 glutMainLoop();
}
