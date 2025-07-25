
/*
 * Textured cylinder demo: lighting, texturing, reflection mapping.
 *
 * Command line options:
 *    -info      print GL implementation information
 *
 *
 * Brian Paul  May 1997  This program is in the public domain.
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "glut_wrap.h"

#include "readtex.h"

#define TEXTURE_FILE DEMOS_DATA_DIR "reflect.png"

#define LIT 1
#define TEXTURED 2
#define REFLECT 3
#define ANIMATE 10
#define POINT_FILTER 20
#define LINEAR_FILTER 21
#define QUIT 100

static GLint Win = -1;

static GLuint CylinderObj = 0;
static GLboolean Animate = GL_TRUE;

static GLfloat Xrot = 0.0, Yrot = 0.0, Zrot = 0.0;
static GLfloat DXrot = 50.0, DYrot = 125.0;

/* performance info */
static GLint T0 = 0;
static GLint Frames = 0;


static void Idle( void )
{
   static double t0 = -1.;
   double dt, t = glutGet(GLUT_ELAPSED_TIME) / 1000.0;
   if (t0 < 0.0)
      t0 = t;
   dt = t - t0;
   t0 = t;

   if (Animate) {
      Xrot += DXrot * dt;
      Yrot += DYrot * dt;
      glutPostRedisplay();
   }
}


static void Display( void )
{
   glClear( GL_COLOR_BUFFER_BIT );

   glPushMatrix();
   glRotatef(Xrot, 1.0, 0.0, 0.0);
   glRotatef(Yrot, 0.0, 1.0, 0.0);
   glRotatef(Zrot, 0.0, 0.0, 1.0);
   glScalef(5.0, 5.0, 5.0);
   glCallList(CylinderObj);

   glPopMatrix();

   glutSwapBuffers();

   if (Animate) {
      GLint t = glutGet(GLUT_ELAPSED_TIME);
      Frames++;
      if (t - T0 >= 5000) {
         GLfloat seconds = (t - T0) / 1000.0;
         GLfloat fps = Frames / seconds;
         printf("%d frames in %g seconds = %g FPS\n", Frames, seconds, fps);
         fflush(stdout);
         T0 = t;
         Frames = 0;
      }
   }
}


static void Reshape( int width, int height )
{
   glViewport( 0, 0, width, height );
   glMatrixMode( GL_PROJECTION );
   glLoadIdentity();
   glFrustum( -1.0, 1.0, -1.0, 1.0, 10.0, 100.0 );
   glMatrixMode( GL_MODELVIEW );
   glLoadIdentity();
   glTranslatef( 0.0, 0.0, -70.0 );
}


static void SetMode(GLuint m)
{
   /* disable everything */
   glDisable(GL_LIGHTING);
   glDisable(GL_TEXTURE_2D);
   glDisable(GL_TEXTURE_GEN_S);
   glDisable(GL_TEXTURE_GEN_T);

   /* enable what's needed */
   if (m==LIT) {
      glEnable(GL_LIGHTING);
   }
   else if (m==TEXTURED) {
      glEnable(GL_TEXTURE_2D);
   }
   else if (m==REFLECT) {
      glEnable(GL_TEXTURE_2D);
      glEnable(GL_TEXTURE_GEN_S);
      glEnable(GL_TEXTURE_GEN_T);
   }
}


static void ModeMenu(int entry)
{
   if (entry==ANIMATE) {
      Animate = !Animate;
      if (Animate)
         glutIdleFunc(Idle);
      else
         glutIdleFunc(NULL);
   }
   else if (entry==POINT_FILTER) {
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
   }
   else if (entry==LINEAR_FILTER) {
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
   }
   else if (entry==QUIT) {
      exit(0);
   }
   else {
      SetMode(entry);
   }
   glutPostRedisplay();
}


static void Key( unsigned char key, int x, int y )
{
   (void) x;
   (void) y;
   switch (key) {
      case ' ':
      Animate = !Animate;
      if (Animate)
         glutIdleFunc(Idle);
      else
         glutIdleFunc(NULL);
      break;
      case 27:
         glutDestroyWindow(Win);
         exit(0);
         break;
   }
   glutPostRedisplay();
}


static void SpecialKey( int key, int x, int y )
{
   float step = 3.0;
   (void) x;
   (void) y;

   switch (key) {
      case GLUT_KEY_UP:
         Xrot += step;
         break;
      case GLUT_KEY_DOWN:
         Xrot -= step;
         break;
      case GLUT_KEY_LEFT:
         Yrot += step;
         break;
      case GLUT_KEY_RIGHT:
         Yrot -= step;
         break;
   }
   glutPostRedisplay();
}


static void Init( int argc, char *argv[] )
{
   GLUquadricObj *q = gluNewQuadric();
   CylinderObj = glGenLists(1);
   glNewList(CylinderObj, GL_COMPILE);

   glTranslatef(0.0, 0.0, -1.0);

   /* cylinder */
   gluQuadricNormals(q, GL_SMOOTH);
   gluQuadricTexture(q, GL_TRUE);
   gluCylinder(q, 0.6, 0.6, 2.0, 24, 1);

   /* end cap */
   glTranslatef(0.0, 0.0, 2.0);
   gluDisk(q, 0.0, 0.6, 24, 1);

   /* other end cap */
   glTranslatef(0.0, 0.0, -2.0);
   gluQuadricOrientation(q, GLU_INSIDE);
   gluDisk(q, 0.0, 0.6, 24, 1);

   glEndList();
   gluDeleteQuadric(q);

   /* lighting */
   glEnable(GL_LIGHTING);
   {
      GLfloat gray[4] = {0.2, 0.2, 0.2, 1.0};
      GLfloat white[4] = {1.0, 1.0, 1.0, 1.0};
      GLfloat teal[4] = { 0.0, 1.0, 0.8, 1.0 };
      glMaterialfv(GL_FRONT, GL_DIFFUSE, teal);
      glLightfv(GL_LIGHT0, GL_AMBIENT, gray);
      glLightfv(GL_LIGHT0, GL_DIFFUSE, white);
      glEnable(GL_LIGHT0);
   }

   /* fitering = nearest, initially */
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

   glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
   glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);

   glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
   glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);

   if (!LoadRGBMipmaps(TEXTURE_FILE, GL_RGB)) {
      printf("Error: couldn't load texture image\n");
      exit(1);
   }

   glEnable(GL_CULL_FACE);  /* don't need Z testing for convex objects */

   SetMode(LIT);

   if (argc > 1 && strcmp(argv[1], "-info")==0) {
      printf("GL_RENDERER   = %s\n", (char *) glGetString(GL_RENDERER));
      printf("GL_VERSION    = %s\n", (char *) glGetString(GL_VERSION));
      printf("GL_VENDOR     = %s\n", (char *) glGetString(GL_VENDOR));
      printf("GL_EXTENSIONS = %s\n", (char *) glGetString(GL_EXTENSIONS));
   }
}


int main( int argc, char *argv[] )
{
   glutInitWindowSize( 400, 400 );
   glutInit( &argc, argv );
   glutInitDisplayMode( GLUT_RGB | GLUT_DOUBLE );

   Win = glutCreateWindow(argv[0] );

   Init(argc, argv);

   glutReshapeFunc( Reshape );
   glutKeyboardFunc( Key );
   glutSpecialFunc( SpecialKey );
   glutDisplayFunc( Display );
   glutIdleFunc( Idle );

   glutCreateMenu(ModeMenu);
   glutAddMenuEntry("Lit", LIT);
   glutAddMenuEntry("Textured", TEXTURED);
   glutAddMenuEntry("Reflect", REFLECT);
   glutAddMenuEntry("Point Filtered", POINT_FILTER);
   glutAddMenuEntry("Linear Filtered", LINEAR_FILTER);
   glutAddMenuEntry("Toggle Animation", ANIMATE);
   glutAddMenuEntry("Quit", QUIT);
   glutAttachMenu(GLUT_RIGHT_BUTTON);

   glutMainLoop();
   return 0;
}
