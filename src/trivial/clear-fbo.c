
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "glad/gl.h"
#include "glut_wrap.h"



static int Width = 512, Height = 512;
static GLuint MyFB, MyRB;


#define CheckError() assert(glGetError() == 0)


static void Init(void)
{
   fprintf(stderr, "GL_RENDERER   = %s\n", (char *) glGetString(GL_RENDERER));
   fprintf(stderr, "GL_VERSION    = %s\n", (char *) glGetString(GL_VERSION));
   fprintf(stderr, "GL_VENDOR     = %s\n", (char *) glGetString(GL_VENDOR));
   fflush(stderr);

   if (!glutExtensionSupported("GL_EXT_framebuffer_object")) {
      printf("GL_EXT_framebuffer_object not found!\n");
      exit(0);
   }

   glGenFramebuffersEXT(1, &MyFB);
   glGenRenderbuffersEXT(1, &MyRB);

   glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, MyFB);

   glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, MyRB);

   glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT1_EXT,
                                GL_RENDERBUFFER_EXT, MyRB);

   glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_RGB, Width, Height);

   glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
}


static void
Reshape(int width, int height)
{
   glViewport(0, 0, width, height);
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   glOrtho(-1.0, 1.0, -1.0, 1.0, -0.5, 1000.0);
   glMatrixMode(GL_MODELVIEW);

   Width = width;
   Height = height;
   glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_RGB, Width, Height);
}


static void
Key(unsigned char key, int x, int y)
{
   if (key == 27)
      exit(0);
   glutPostRedisplay();
}


static void
Draw(void)
{
   /* draw to user framebuffer */
   glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, MyFB);
   glDrawBuffer(GL_COLOR_ATTACHMENT1_EXT);
   glReadBuffer(GL_COLOR_ATTACHMENT1_EXT);

   glViewport(0, 0, Width, Height);
   CheckError();

   /* Clear the FBO to gray-blue */
   glClearColor(0.5, 0.5, 1.0, 0.0);
   glClear(GL_COLOR_BUFFER_BIT);
   CheckError();

   if (0) {
      glBegin(GL_TRIANGLES);
      glColor3f(0,0,.7);
      glVertex3f(0.9, -0.9, -30.0);
      glColor3f(.8,0,0);
      glVertex3f(0.9,  0.9, -30.0);
      glColor3f(0,.9,0);
      glVertex3f(-0.9,  0.0, -30.0);
      glEnd();
   }

   {
      GLubyte *buffer = malloc(Width * Height * 4);

      /* read from user framebuffer */
      glReadPixels(0, 0, Width-60, Height-60, GL_RGBA, GL_UNSIGNED_BYTE, buffer);
      CheckError();

      printf("Pixel(0,0) = %d, %d, %d, %d\n",
             buffer[0], buffer[1], buffer[2], buffer[3]);
      fflush(stdout);

      /* draw to window */
      glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
      glViewport(0, 0, Width, Height);

      /* Try to clear the window, but will overwrite:
       */
      glClearColor(0.8, 0.8, 0, 0.0);
      glClear(GL_COLOR_BUFFER_BIT);

      glWindowPos2iARB(30, 30);
      glDrawPixels(Width-60, Height-60, GL_RGBA, GL_UNSIGNED_BYTE, buffer);

      free(buffer);
   }

   /* Bind normal framebuffer */
   glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
   glViewport(0, 0, Width, Height);

   if (0) {
      glBegin(GL_TRIANGLES);
      glColor3f(0,.7,0);
      glVertex3f(0.5, -0.5, -30.0);
      glColor3f(0,0,.8);
      glVertex3f(0.5,  0.5, -30.0);
      glColor3f(.9,0,0);
      glVertex3f(-0.5,  0.0, -30.0);
      glEnd();
   }

   glutSwapBuffers();

   CheckError();
}


int
main(int argc, char *argv[])
{
   glutInit(&argc, argv);
   glutInitWindowPosition(100, 0);
   glutInitWindowSize(Width, Height);
   glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);

   if (glutCreateWindow(argv[0]) == GL_FALSE) {
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
