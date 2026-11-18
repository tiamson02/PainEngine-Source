

#include <stdlib.h>
#include "stdio_multistream.h"
//#include <stdio.h>
#include <math.h>

#include "GL/ps2gl.h"
#include "pads.h"
#include "audio.h"

enum model {cube = 0, frontface = 1, singletri = 2 } ;
enum method { quads = 0, tris = 1, tristrip = 2 } ;

int type = cube;
//int type = frontface;
int method = tristrip;

// Above 32, we get incorrect clamping effects - huh? Note that 2048/64 (our
// texture size) == 32....
#define TEXREPEAT 0.75

// tglogo data
extern "C" {
extern const int tglogo_imgwidth, tglogo_imgheight;
extern const int tglogo_width, tglogo_height;
extern const float tglogo_x0, tglogo_y0, tglogo_x1, tglogo_y1;
extern const unsigned char tglogo_data[];
};

static float points[16][3] = {
	{ -2, 1, -1 },
	{ -2, 1, 1 },
	{ 2, 1, 1 },
	{ 2, 1, -1 },

	{ -2, -1, -1 },
	{ -2, -1, 1 },
	{ 2, -1, 1 },
	{ 2, -1, -1 },

};

GLuint create_boxtex()
{
	GLuint  newTex;
	glGenTextures(1, &newTex);
	glBindTexture(GL_TEXTURE_2D, newTex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_NEAREST);

	GLubyte *tbits = (GLubyte*)malloc(64*64*4);

	int i,j;
	for (i = 0; i < 64; i++)
	{
		for (j=0; j < 64; j++)
		{
			tbits[(i*64 + j)*4 + 0]= tglogo_data[(i*64 + j)*4 + 2] ;
			tbits[(i*64 + j)*4 + 1]= tglogo_data[(i*64 + j)*4 + 1] ;
			tbits[(i*64 + j)*4 + 2]= tglogo_data[(i*64 + j)*4 + 0] ;
			tbits[(i*64 + j)*4 + 3]= 0;
	
	//		tbits[(i*64 + j)*4 + 0]= 2*i + 2*j;
	//		tbits[(i*64 + j)*4 + 1]= i + 3*j;
	//		tbits[(i*64 + j)*4 + 2]= 3*i + j;
	//		tbits[(i*64 + j)*4 + 3]= 255;
		}
	}

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tglogo_imgwidth, tglogo_imgheight, 0, GL_RGBA, GL_UNSIGNED_BYTE, tbits);

	return newTex;
}



void test_drawing();
void perspective( float fov, float aspect, float nearClip, float farClip );
void reshape(int w, int h);
void draw_cube(int model, int method);

void reset_renderers();

void
draw_cube(int model, int method)
{

	static GLuint boxtex = create_boxtex();
	//glEnable(GL_BLEND);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_ALPHA_TEST);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glBindTexture(GL_TEXTURE_2D, boxtex);

	switch (method) 
	{
	case quads:
		glBegin( GL_QUADS);
		switch (model)
		{
		case cube: // draw a cube
			// top
			glNormal3f( 0, 1, 0 );
			glColor3f( 1, 0, 0 );
			glVertex3fv( points[0] );
			glVertex3fv( points[1] );
			glVertex3fv( points[2] );
			glVertex3fv( points[3] );

			// bottom
			glNormal3f( 0, -1, 0 );
			glColor3f( 1, 1, 1 );
			glVertex3fv( points[7] );
			glVertex3fv( points[6] );
			glVertex3fv( points[5] );
			glVertex3fv( points[4] );

			// left
			glNormal3f( -1, 0, 0 );
			glColor3f( 0, 0, 1 );
			glVertex3fv( points[0] );
			glVertex3fv( points[4] );
			glVertex3fv( points[5] );
			glVertex3fv( points[1] );

			// right
			glNormal3f( 1, 0, 0 );
			glColor3f( 1, 1, 0 );
			glVertex3fv( points[2] );
			glVertex3fv( points[6] );
			glVertex3fv( points[7] );
			glVertex3fv( points[3] );

			// back
			glNormal3f( 0, 0, -1 );
			glColor3f( 1, 0, 1 );
			glVertex3fv( points[3] );
			glVertex3fv( points[7] );
			glVertex3fv( points[4] );
			glVertex3fv( points[0] );
			// no break here, let it flow to draw the front	
		case frontface:
			// front
			glNormal3f( 0, 0, 1 );
			glColor3f( 0, 1, 1 );
			glVertex3fv( points[1] );
			glVertex3fv( points[5] );
			glVertex3fv( points[6] );
			glVertex3fv( points[2] );
			break;
		case singletri:
			// can't draw single triangle with a quad
			break;
		}
		glEnd();
		break;
	
	case tris: 	// draw front face only
		glBegin( GL_TRIANGLES);
		switch (model)
		{
		case cube: // draw a cube
			// top
			glNormal3f( 0, 1, 0 );
			glColor3f( 1, 0, 0 );
			glVertex3fv( points[0] );
			glVertex3fv( points[1] );
			glVertex3fv( points[2] );
			glVertex3fv( points[2] );
			glVertex3fv( points[3] );
			glVertex3fv( points[0] );

			// bottom
			glNormal3f( 0, -1, 0 );
			glColor3f( 1, 1, 1 );
			glVertex3fv( points[7] );
			glVertex3fv( points[6] );
			glVertex3fv( points[5] );
			glVertex3fv( points[5] );
			glVertex3fv( points[4] );
			glVertex3fv( points[7] );

			// left
			glNormal3f( -1, 0, 0 );
			glColor3f( 0, 0, 1 );
			glVertex3fv( points[0] );
			glVertex3fv( points[4] );
			glVertex3fv( points[5] );
			glVertex3fv( points[5] );
			glVertex3fv( points[1] );
			glVertex3fv( points[0] );

			// right
			glNormal3f( 1, 0, 0 );
			glColor3f( 1, 1, 0 );
			glVertex3fv( points[2] );
			glVertex3fv( points[6] );
			glVertex3fv( points[7] );
			glVertex3fv( points[7] );
			glVertex3fv( points[3] );
			glVertex3fv( points[2] );

			// back
			glNormal3f( 0, 0, -1 );
			glColor3f( 1, 0, 1 );
			glVertex3fv( points[3] );
			glVertex3fv( points[7] );
			glVertex3fv( points[4] );
			glVertex3fv( points[4] );
			glVertex3fv( points[0] );
			glVertex3fv( points[3] );
			// no break here, let it flow to draw the front	
		case frontface:
			// front
			glNormal3f( 0, 0, 1 );
			glColor3f( 0, 1, 1 );
			glVertex3fv( points[1] );
			glVertex3fv( points[5] );
			glVertex3fv( points[6] );
			// no break here, let it flow to draw the last tri
		case singletri:
			glVertex3fv( points[6] );
			glVertex3fv( points[2] );
			glVertex3fv( points[1] );
			break;
		}
		glEnd();
		break;
        
	case tristrip:
		switch (model)
		{
		case cube: // draw a cube
#if 1			// top
			glBegin( GL_TRIANGLE_STRIP);
			glNormal3f( 0, 1, 0 );
			glColor3f( 1, 0, 0 );
      glTexCoord2f(0.0, 0.0);
			glVertex3fv( points[1] );
      glTexCoord2f(TEXREPEAT, 0.0);
			glVertex3fv( points[2] );
      glTexCoord2f(0.0, TEXREPEAT);
			glVertex3fv( points[0] );
      glTexCoord2f(TEXREPEAT, TEXREPEAT);
			glVertex3fv( points[3] );
			glEnd();

			// bottom
			glBegin( GL_TRIANGLE_STRIP);
			glNormal3f( 0, -1, 0 );
			glColor3f( 1, 1, 1 );
      glTexCoord2f(0.0, 0.0);
			glVertex3fv( points[6] );
      glTexCoord2f(TEXREPEAT, 0.0);
			glVertex3fv( points[5] );
      glTexCoord2f(0.0, TEXREPEAT);
			glVertex3fv( points[7] );
      glTexCoord2f(TEXREPEAT, TEXREPEAT);
			glVertex3fv( points[4] );
			glEnd();
#endif
#if 1
			// left
			glBegin( GL_TRIANGLE_STRIP);
			glNormal3f( -1, 0, 0 );
			glColor3f( 0, 0, 1 );
      glTexCoord2f(0.0, 0.0);
			glVertex3fv( points[4] );
      glTexCoord2f(TEXREPEAT, 0.0);
			glVertex3fv( points[5] );
      glTexCoord2f(0.0, TEXREPEAT);
			glVertex3fv( points[0] );
      glTexCoord2f(TEXREPEAT, TEXREPEAT);
			glVertex3fv( points[1] );
			glEnd();

			// right
			glBegin( GL_TRIANGLE_STRIP);
			glNormal3f( 1, 0, 0 );
			glColor3f( 1, 1, 0 );
      glTexCoord2f(0.0, 0.0);
			glVertex3fv( points[6] );
      glTexCoord2f(TEXREPEAT, 0.0);
			glVertex3fv( points[7] );
      glTexCoord2f(0.0, TEXREPEAT);
			glVertex3fv( points[2] );
      glTexCoord2f(TEXREPEAT, TEXREPEAT);
			glVertex3fv( points[3] );
			glEnd();

#endif
#if 1
			// back
			glBegin( GL_TRIANGLE_STRIP);
			glNormal3f( 0, 0, -1 );
			glColor3f( 1, 0, 1 );
      glTexCoord2f(0.0, 0.0);
			glVertex3fv( points[7] );
      glTexCoord2f(TEXREPEAT, 0.0);
			glVertex3fv( points[4] );
      glTexCoord2f(0.0, TEXREPEAT);
			glVertex3fv( points[3] );
      glTexCoord2f(TEXREPEAT, TEXREPEAT);
			glVertex3fv( points[0] );
			glEnd();
#endif
		// no break here, let it flow to draw the front	
		// we draw the single triangle first, then complete the front 
		case frontface:
#if 1
			// front
			glBegin( GL_TRIANGLE_STRIP);
			glNormal3f( 0, 0, 1 );
			glColor3f( 0, 1, 1 );
      glTexCoord2f(0.0, 0.0);
                        glVertex3fv( points[5] );
      glTexCoord2f(TEXREPEAT, 0.0);
                        glVertex3fv( points[6] );
      glTexCoord2f(0.0, TEXREPEAT);
                        glVertex3fv( points[1] );
      glTexCoord2f(TEXREPEAT, TEXREPEAT);
                        glVertex3fv( points[2] );
			glEnd();
#endif
			break;
		case singletri:
			glBegin( GL_TRIANGLE_STRIP);
			glNormal3f( 0, 0, 1 );
			glColor3f( 0, 1, 1 );
      glTexCoord2f(0.0, 0.0);
			glVertex3fv( points[5] );
      glTexCoord2f(TEXREPEAT, 0.0);
			glVertex3fv( points[6] );
      glTexCoord2f(0.0, TEXREPEAT);
			glVertex3fv( points[1] );
			glEnd();
			break;
		}
		break;
		
	}
}

void reset_renderers()
{
	glBegin( GL_TRIANGLE_STRIP);
	glVertex3fv( points[0] );
	glVertex3fv( points[1] );
	glVertex3fv( points[2] );
	glEnd();
	
	glBegin( GL_QUADS);
	glNormal3f( 0, 1, 0 );
	glColor3f( 1, 0, 0 );
	glVertex3fv( points[0] );
	glVertex3fv( points[1] );
	glVertex3fv( points[2] );
	glVertex3fv( points[3] );
	glEnd();
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

static GLfloat l0_position[] = {-1, 0.9f, 0.9f, 0};
static GLfloat l1_position[] = {-2, -1, 1, 0.0};

void
init_lights(void)
{
   GLfloat ambient[] = {0.2, 0.2, 0.2, 1.0};

   GLfloat l0_diffuse[] = {1.0f, 1.0f, 1.0f, 0 };

   GLfloat l1_diffuse[] = {.6f, .2f, .2f, 0.0f };

   GLfloat black[] = { 0, 0, 0, 0 };

   glEnable(GL_LIGHTING);
   glEnable(GL_LIGHT0);
   glEnable(GL_LIGHT1);

   // a directional light

   glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
   glLightfv(GL_LIGHT0, GL_DIFFUSE, l0_diffuse);
   glLightfv(GL_LIGHT0, GL_SPECULAR, l0_diffuse);

   // a point light

   glLightfv(GL_LIGHT1, GL_AMBIENT, black);
   glLightfv(GL_LIGHT1, GL_DIFFUSE, l1_diffuse);
   glLightfv(GL_LIGHT1, GL_SPECULAR, black);

   glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, 0.0f);
   glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, 0.005f);
   glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, 0.0f);
}

void
set_light_positions()
{
   glLightfv(GL_LIGHT0, GL_POSITION, l0_position);
   glLightfv(GL_LIGHT1, GL_POSITION, l1_position);
}
#define KEYBOARD_MOVES
void test_drawing()
 {

static float camera_z = 5.0f, camera_alt = 0.0f, camera_azi = 0.0f;
static float spin_angle = 0.0f;
static float spin_angle2 = 0.0f;
static float color_red = 0.0f;

static float xtrans = -2.5f;
static float ytrans = 0.0f;
static float ztrans = 0.0f;
static int   cullState = 0;
static int Wait = 1;
int skipchar =0;
   reshape (640, 480);

   glClearColor(0.0f, 0.0f, 0.1f, 0.0f);
   glEnable(GL_DEPTH_TEST);
   glEnable( GL_RESCALE_NORMAL );
   glShadeModel(GL_SMOOTH);

   init_lights();
   while (1) {
#if defined (KEYBOARD_MOVES)

	char input = 't';
//	if (skipchar == 0)
//		input = getchar();
//	else
//		skipchar--;
	if (input == '=')
		break;
	switch (input)
	{
	case 'f':
		skipchar += 50;
		break;
	case 'l':
		xtrans-=0.2;
		break;
	case 'r':
		xtrans+=0.2;
		break;
	case 'u':
		ytrans+=0.2;
		break;
	case 'd':
		ytrans-=0.2;
		break;
	case 'z':
		ztrans-=0.2;
		break;
	case 'Z':
		ztrans+=0.2;
		break;
	case 'b':
		cullState += 1;
		if (cullState >= 3) 
			cullState = 0;
		if (cullState)
		{
			glEnable(GL_CULL_FACE);
			if (cullState == 1)
				glCullFace(GL_FRONT);
		        else
				glCullFace(GL_BACK);
		} else
			glDisable(GL_CULL_FACE);
		break;
	case 'B':
		glCullFace(GL_BACK);
		break;
	case 'c':
		pglEnableCustom(1<<20);
		break;
	case 'C':
		pglDisableCustom(1<<20);
		break;
	case 'q':
		method ++; 
		if (method > tristrip) method = 0;
		break;
	case 'Q':
		type ++; 
		if (type > singletri) type = 0;
		break;
	case 't':
		spin_angle += 0.35f;
		break;
	case 'T':
		spin_angle -= 3.0f;
		break;
	case 'W':
		Wait = !Wait;
		break;
	case 'R':
		{
			const char* name = pglGetCurRendererName();
			if (name) 
				printf("rend name = %s \n", name);
		}
		break;
	case 'h':
		
		printf("l/r/u/d/z/Z translate the world\n");
		printf("Q toggles the model being drawn (SIDE/CUBE)\n");
		printf("q toggles the drawing method (TRIANGLE/QUAD)\n");
		printf("c/C sets on and off the custom renderer flag \n");
		printf("b cycles the culling from FRONT/BACK/NONE \n");
		
		printf("R prints out the current VU renderer\n");
		printf("t/T spins the world around the Z axis\n");
		printf("W toggles syncing to the display (ie, fast mode)\n");
		break;
	default:
		break;

   }
#endif //defined (KEYBOARD_MOVES)

#if defined( PS2_LINUX )
   // Use the pad for input, not the damned keyboard!
   Pad[PAD1].Update();

   if (Pad[PAD1].UpState())
	ytrans+=0.2;

   if (Pad[PAD1].DownState())
	ytrans-=0.2;

   if (Pad[PAD1].LeftState())
	xtrans-=0.2;

   if (Pad[PAD1].RightState())
	xtrans+=0.2;

   if (Pad[PAD1].CrossState())
	ztrans+=0.2;

   if (Pad[PAD1].TriangleState())
	ztrans-=0.2;

   if (Pad[PAD1].R1State())
	spin_angle += 3.0f;

   if (Pad[PAD1].L1State())
	spin_angle -= 3.0f;

   if (Pad[PAD1].SquareState())
   {
	method ++; 
	if (method > 1) method = 0;
   }

   if (Pad[PAD1].CircleState())
   {
	type++; 
	if (type > 1) type = 0;
   }

#elif defined ( __ee__ )
   // Use the pad for input, not the damned keyboard!
   Pads::Read();
   if (Pad0.IsDown(Pads::kLeftUp))
      ytrans+=0.2;

   if (Pad0.IsDown(Pads::kLeftDown))
	ytrans-=0.2;

   if (Pad0.IsDown(Pads::kLeftLeft))
	xtrans-=0.2;

   if (Pad0.IsDown(Pads::kLeftRight))
	xtrans+=0.2;

   if (Pad0.IsDown(Pads::kRightDown))
	ztrans+=0.2;

   if (Pad0.IsDown(Pads::kRightUp))
	ztrans-=0.2;

   if (Pad0.IsDown(Pads::kR1))
	spin_angle += 3.0f;

   if (Pad0.IsDown(Pads::kL1))
	spin_angle -= 3.0f;

   if (Pad0.IsDown(Pads::kRightLeft))
   {
	method ++; 
	if (method > 1) method = 0;
   }

   if (Pad0.IsDown(Pads::kRightRight))
   {
	type++; 
	if (type > 1) type = 0;
   }
#endif
   pglBeginGeometry();
	static int initial = 0; 
	if (initial == 0 )
	{
		reset_renderers();
		initial = 1;
	}
   //glClearColor(color_red, 0.0f, 0.4f, 0.0f);
   glClearColor(0.0, 0.0f, 0.0f, 0.0f);
//   color_red += 0.01;
   if (color_red > 1.0)
     color_red = 0.0;
	
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   glMatrixMode( GL_MODELVIEW );
   glLoadIdentity();
   // camera transform

   glTranslatef( 0, 0, -camera_z );
   glRotatef( camera_azi, 0, 1, 0 );
   glRotatef( camera_alt, -1, 0, 0 );

   set_light_positions();

   // object space transforms
   //glLoadIdentity();
//   spin_angle += 0.25f;
//   spin_angle2 += 0.15f;
	glRotatef( spin_angle, 0, 1, 0 );
	glRotatef( spin_angle2, 1, 0, 0 );
	glTranslatef(xtrans, ytrans, ztrans);

	draw_cube(type, method);

	glLoadIdentity();

   glDisable( GL_TEXTURE_2D );

	glFlush();
	pglEndGeometry();
	static int first = 1;
	if (first)
		first = 0;
	else
		pglFinishRenderingGeometry( PGL_DONT_FORCE_IMMEDIATE_STOP );	
	if (Wait) pglWaitForVSync();
	pglSwapBuffers();
	pglRenderGeometry();

   }
}

void perspective( float fov, float aspect, float nearClip, float farClip )
{
   float w, h;
   fov *= 3.141592654f/180.0f;
   h = 2.0f * nearClip * (float)tanf( fov/2.0f );
   w = h * aspect;

   glFrustum( -w/2.0f, w/2.0f, -h/2.0f, h/2.0f, nearClip, farClip );
}

void reshape(int w, int h)
{
   glViewport(0, 0, w, h);

   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   perspective( 40.0f, (float)w/(float)h, 1.0f, 4000.0f );

   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
}

