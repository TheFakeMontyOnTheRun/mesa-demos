/*
 * Copyright (c) 1991, 1992, 1993 Silicon Graphics, Inc.
 *
 * Permission to use, copy, modify, distribute, and sell this software and
 * its documentation for any purpose is hereby granted without fee, provided
 * that (i) the above copyright notices and this permission notice appear in
 * all copies of the software and related documentation, and (ii) the name of
 * Silicon Graphics may not be used in any advertising or
 * publicity relating to the software without the specific, prior written
 * permission of Silicon Graphics.
 *
 * THE SOFTWARE IS PROVIDED "AS-IS" AND WITHOUT WARRANTY OF
 * ANY KIND,
 * EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY
 * WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * IN NO EVENT SHALL SILICON GRAPHICS BE LIABLE FOR
 * ANY SPECIAL, INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND,
 * OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER OR NOT ADVISED OF THE POSSIBILITY OF DAMAGE, AND ON ANY THEORY OF
 * LIABILITY, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE
 * OF THIS SOFTWARE.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "glad/gl.h"
#include "glut_wrap.h"


#define CI_OFFSET_1 16
#define CI_OFFSET_2 32

#define SIZE 16
GLenum doubleBuffer;
GLint line = 0;

static void MakeImage(void)
{
   GLubyte tex2d[SIZE][SIZE][3];
   GLint s, t;

   for (s = 0; s < SIZE; s++) {
      for (t = 0; t < SIZE; t++) {
         tex2d[t][s][0] = s*255/(SIZE-1);
         tex2d[t][s][1] = t*255/(SIZE-1);
         tex2d[t][s][2] = 0*255/(SIZE-1);
      }
   }

   glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
                SIZE, SIZE,
                0,
                GL_RGB, GL_UNSIGNED_BYTE, tex2d);
   glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
   glEnable(GL_TEXTURE_2D);
}

static void UpdateLine(void)
{
   GLubyte tex[SIZE][3];
   GLubyte b = 0;
   GLint s, t;

   t = line % SIZE;
   if (line % (SIZE * 2) < SIZE)
      b = 255;
   else
      b = 0;

   for (s = 0; s < SIZE; s++) {
      tex[s][0] = s*255/(SIZE-1);
      tex[s][1] = t*255/(SIZE-1);
      tex[s][2] = b;
   }

   glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
   glTexSubImage2D(GL_TEXTURE_2D, 0,
                   0, t,
                   SIZE, 1,
                   GL_RGB, GL_UNSIGNED_BYTE, tex);
   glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
}

static void Init(void)
{
   fprintf(stderr, "GL_RENDERER   = %s\n", (char *) glGetString(GL_RENDERER));
   fprintf(stderr, "GL_VERSION    = %s\n", (char *) glGetString(GL_VERSION));
   fprintf(stderr, "GL_VENDOR     = %s\n", (char *) glGetString(GL_VENDOR));
   fflush(stderr);

   glClearColor(0.0, 0.0, 1.0, 0.0);

   glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

   MakeImage();
   UpdateLine();
   line++;
}

static void Reshape(int width, int height)
{

   glViewport(0, 0, (GLint)width, (GLint)height);

   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   glOrtho(-1.0, 1.0, -1.0, 1.0, -0.5, 1000.0);
   glMatrixMode(GL_MODELVIEW);
}

static void Key(unsigned char key, int x, int y)
{

   switch (key) {
   case 27:
      exit(1);
   default:
      UpdateLine();
      line++;
      break;
   }

   glutPostRedisplay();
}

static void Draw(void)
{
   glClear(GL_COLOR_BUFFER_BIT);

   glBegin(GL_QUADS);
   glTexCoord2f(1,0);
   glVertex3f( 0.9, -0.9, -30.0);
   glTexCoord2f(1,1);
   glVertex3f( 0.9,  0.9, -30.0);
   glTexCoord2f(0,1);
   glVertex3f(-0.9,  0.9, -30.0);
   glTexCoord2f(0,0);
   glVertex3f(-0.9,  -0.9, -30.0);
   glEnd();

   glFlush();

   if (doubleBuffer) {
      glutSwapBuffers();
   }
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
         fprintf(stderr, "%s (Bad option).\n", argv[i]);
         return GL_FALSE;
      }
   }
   return GL_TRUE;
}

int main(int argc, char **argv)
{
   GLenum type;

   glutInit(&argc, argv);

   if (Args(argc, argv) == GL_FALSE) {
      exit(1);
   }

   glutInitWindowPosition(0, 0);
   glutInitWindowSize(250, 250);

   type = GLUT_RGB | GLUT_ALPHA;
   type |= (doubleBuffer) ? GLUT_DOUBLE : GLUT_SINGLE;
   glutInitDisplayMode(type);

   if (glutCreateWindow(*argv) == GL_FALSE) {
      exit(1);
   }

   gladLoaderLoadGL();

   Init();

   glutReshapeFunc(Reshape);
   glutKeyboardFunc(Key);
   glutDisplayFunc(Draw);
   glutMainLoop();
   gladLoaderUnloadGL();
   return 0;
}
