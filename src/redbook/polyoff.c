/*
 * Copyright (c) 1993-1997, Silicon Graphics, Inc.
 * ALL RIGHTS RESERVED
 * Permission to use, copy, modify, and distribute this software for
 * any purpose and without fee is hereby granted, provided that the above
 * copyright notice appear in all copies and that both the copyright notice
 * and this permission notice appear in supporting documentation, and that
 * the name of Silicon Graphics, Inc. not be used in advertising
 * or publicity pertaining to distribution of the software without specific,
 * written prior permission.
 *
 * THE MATERIAL EMBODIED ON THIS SOFTWARE IS PROVIDED TO YOU "AS-IS"
 * AND WITHOUT WARRANTY OF ANY KIND, EXPRESS, IMPLIED OR OTHERWISE,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF MERCHANTABILITY OR
 * FITNESS FOR A PARTICULAR PURPOSE.  IN NO EVENT SHALL SILICON
 * GRAPHICS, INC.  BE LIABLE TO YOU OR ANYONE ELSE FOR ANY DIRECT,
 * SPECIAL, INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY
 * KIND, OR ANY DAMAGES WHATSOEVER, INCLUDING WITHOUT LIMITATION,
 * LOSS OF PROFIT, LOSS OF USE, SAVINGS OR REVENUE, OR THE CLAIMS OF
 * THIRD PARTIES, WHETHER OR NOT SILICON GRAPHICS, INC.  HAS BEEN
 * ADVISED OF THE POSSIBILITY OF SUCH LOSS, HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, ARISING OUT OF OR IN CONNECTION WITH THE
 * POSSESSION, USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * US Government Users Restricted Rights
 * Use, duplication, or disclosure by the Government is subject to
 * restrictions set forth in FAR 52.227.19(c)(2) or subparagraph
 * (c)(1)(ii) of the Rights in Technical Data and Computer Software
 * clause at DFARS 252.227-7013 and/or in similar or successor
 * clauses in the FAR or the DOD or NASA FAR Supplement.
 * Unpublished-- rights reserved under the copyright laws of the
 * United States.  Contractor/manufacturer is Silicon Graphics,
 * Inc., 2011 N.  Shoreline Blvd., Mountain View, CA 94039-7311.
 *
 * OpenGL(R) is a registered trademark of Silicon Graphics, Inc.
 */

/*
 *  polyoff.c
 *  This program demonstrates polygon offset to draw a shaded
 *  polygon and its wireframe counterpart without ugly visual
 *  artifacts ("stitching").
 */
#include "glut_wrap.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

GLuint list;
GLint fill = 1;
GLfloat spinx = 0;
GLfloat spiny = 0;
GLfloat tdist = 0.0;
GLfloat polyfactor = 1.0;
GLfloat polyunits = 1.0;
GLboolean doubleBuffer;


/*  display() draws two spheres, one with a gray, diffuse material,
 *  the other sphere with a magenta material with a specular highlight.
 */
static void display (void)
{
    GLfloat gray[] = { 0.8, 0.8, 0.8, 1.0 };
    GLfloat black[] = { 0.0, 0.0, 0.0, 1.0 };

    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glPushMatrix ();
    glTranslatef (0.0, 0.0, tdist);
    glRotatef ((GLfloat) spinx, 1.0, 0.0, 0.0);
    glRotatef ((GLfloat) spiny, 0.0, 1.0, 0.0);

    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, gray);
    glMaterialfv(GL_FRONT, GL_SPECULAR, black);
    glMaterialf(GL_FRONT, GL_SHININESS, 0.0);
    if (fill) {
       glEnable(GL_LIGHTING);
       glEnable(GL_LIGHT0);
       glEnable(GL_POLYGON_OFFSET_FILL);
       glPolygonOffset(polyfactor, polyunits);
       glCallList (list);
       glDisable(GL_POLYGON_OFFSET_FILL);
    }

    glDisable(GL_LIGHTING);
    glDisable(GL_LIGHT0);
    glColor3f (1.0, 1.0, 1.0);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glPolygonOffset(-polyfactor, -polyunits);
    if (!fill) glEnable(GL_POLYGON_OFFSET_LINE);
    glCallList (list);
    glDisable(GL_POLYGON_OFFSET_LINE);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    if (!fill) {
       glEnable(GL_LIGHTING);
       glEnable(GL_LIGHT0);
       glCallList (list);
    }

    glPopMatrix ();
    glFlush ();
    if (doubleBuffer) glutSwapBuffers();
}

/*  specify initial properties
 *  create display list with sphere
 *  initialize lighting and depth buffer
 */
static void gfxinit (void)
{
    GLfloat light_ambient[] = { 0.0, 0.0, 0.0, 1.0 };
    GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat light_position[] = { 1.0, 1.0, 1.0, 0.0 };

    GLfloat global_ambient[] = { 0.2, 0.2, 0.2, 1.0 };

    glClearColor (0.0, 0.0, 0.0, 1.0);

    list = glGenLists(1);
    glNewList (list, GL_COMPILE);
       glutSolidSphere(1.0, 20, 12);
    glEndList ();

    glEnable(GL_DEPTH_TEST);

    glLightfv (GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv (GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv (GL_LIGHT0, GL_SPECULAR, light_specular);
    glLightfv (GL_LIGHT0, GL_POSITION, light_position);
    glLightModelfv (GL_LIGHT_MODEL_AMBIENT, global_ambient);
}

/*  call when window is resized  */
static void reshape(int width, int height)
{
    glViewport (0, 0, width, height);
    glMatrixMode (GL_PROJECTION);
    glLoadIdentity ();
    gluPerspective(45.0, (GLdouble)width/(GLdouble)height,
	    1.0, 10.0);
    glMatrixMode (GL_MODELVIEW);
    glLoadIdentity ();
    gluLookAt (0.0, 0.0, 5.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
}

static void Benchmark( float xdiff, float ydiff )
{
   int startTime, endTime;
   int draws;
   double seconds, fps;

   printf("Benchmarking...\n");
   fflush(stdout);

   draws = 0;
   startTime = glutGet(GLUT_ELAPSED_TIME);
   spinx = spiny = 0.0;
   do {
      spinx += xdiff;
      spiny += ydiff;
      display();
      draws++;
      endTime = glutGet(GLUT_ELAPSED_TIME);
   } while (endTime - startTime < 5000);   /* 5 seconds */

   /* Results */
   seconds = (double) (endTime - startTime) / 1000.0;
   fps = draws / seconds;
   printf("Result:  fps: %g\n", fps);
   fflush(stdout);
}


/*  call when mouse button is pressed  */
/* ARGSUSED2 */
static void mouse(int button, int state, int x, int y) {
    switch (button) {
	case GLUT_LEFT_BUTTON:
	    switch (state) {
		case GLUT_DOWN:
		    spinx += 5;
                    glutPostRedisplay();
		    break;
		default:
		    break;
            }
            break;
	case GLUT_MIDDLE_BUTTON:
	    switch (state) {
		case GLUT_DOWN:
		    spiny += 5;
                    glutPostRedisplay();
		    break;
		default:
		    break;
            }
            break;
	case GLUT_RIGHT_BUTTON:
	    switch (state) {
		case GLUT_UP:
		    exit(0);
		    break;
		default:
		    break;
            }
            break;
        default:
            break;
    }
}

/* ARGSUSED1 */
static void keyboard (unsigned char key, int x, int y)
{
   switch (key) {
      case 't':
         if (tdist < 4.0) {
            tdist = (tdist + 0.5);
            glutPostRedisplay();
         }
         break;
      case 'T':
         if (tdist > -5.0) {
            tdist = (tdist - 0.5);
            glutPostRedisplay();
         }
         break;
      case 'F':
         polyfactor = polyfactor + 0.1;
	 printf ("polyfactor is %f\n", polyfactor);
         glutPostRedisplay();
         break;
      case 'f':
         polyfactor = polyfactor - 0.1;
	 printf ("polyfactor is %f\n", polyfactor);
         glutPostRedisplay();
         break;
      case 'U':
         polyunits = polyunits + 1.0;
	 printf ("polyunits is %f\n", polyunits);
         glutPostRedisplay();
         break;
      case 'u':
         polyunits = polyunits - 1.0;
	 printf ("polyunits is %f\n", polyunits);
         glutPostRedisplay();
         break;
      case 'b':
	 Benchmark(5.0, 0);
	 break;
      case 'B':
	 Benchmark(0, 5.0);
	 break;
      case ' ':
	 fill = !fill;
	 printf ("fill/line: %d\n", fill);
         glutPostRedisplay();
         break;
      case 27:  /* Escape */
         exit(0);
         break;
      default:
         break;
   }
   fflush(stdout);
}

static GLenum Args(int argc, char **argv)
{
    GLint i;

    doubleBuffer = GL_FALSE;

    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-sb") == 0) {
	    doubleBuffer = GL_FALSE;
	} else if (strcmp(argv[i], "-db") == 0) {
	    doubleBuffer = GL_TRUE;
	} else {
	    printf("%s (Bad option).\n", argv[i]);
	    return GL_FALSE;
	}
    }
    return GL_TRUE;
}


/*  Main Loop
 *  Open window with initial window size, title bar,
 *  RGBA display mode, and handle input events.
 */
int main(int argc, char** argv)
{
    GLuint type;
    glutInit(&argc, argv);

    Args(argc, argv);

    type = GLUT_DEPTH | GLUT_RGB;
     type |= (doubleBuffer) ? GLUT_DOUBLE : GLUT_SINGLE;

    glutInitDisplayMode(type);
    glutCreateWindow("polyoff");
    glutReshapeFunc(reshape);
    glutDisplayFunc(display);
    glutMouseFunc(mouse);
    glutKeyboardFunc(keyboard);
    gfxinit();
    glutMainLoop();
    return 0;
}
