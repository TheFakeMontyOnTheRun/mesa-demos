/**
 * Another test for noise() functions (noise1 to noise4 tested independently).
 * 13 Dec 2008
 */

#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "glad/gl.h"
#include "glut_wrap.h"

static const char *VertShaderText =
   "void main() {\n"
   "   gl_TexCoord[0].xyz = gl_Vertex.xyz;\n"
   "   gl_TexCoord[0].w = gl_MultiTexCoord1.x;\n"
   "   gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;\n"
   "}\n";

static const char *FragShaderText[ 4 ] = {
   "void main()\n"
   "{\n"
   "   gl_FragColor.rgb = noise3( gl_TexCoord[ 0 ].w ) * 0.5 + 0.5;\n"
   "   gl_FragColor.a = 1.0;\n"
   "}\n",
   "void main()\n"
   "{\n"
   "   gl_FragColor.rgb = noise3( gl_TexCoord[ 0 ].xw ) * 0.5 + 0.5;\n"
   "   gl_FragColor.a = 1.0;\n"
   "}\n",
   "void main()\n"
   "{\n"
   "   gl_FragColor.rgb = noise3( gl_TexCoord[ 0 ].xyw ) * 0.5 + 0.5;\n"
   "   gl_FragColor.a = 1.0;\n"
   "}\n",
   "void main()\n"
   "{\n"
   "   gl_FragColor.rgb = noise3( gl_TexCoord[ 0 ].xyzw ) * 0.5 + 0.5;\n"
   "   gl_FragColor.a = 1.0;\n"
   "}\n"
};

struct uniform_info {
   const char *name;
   GLuint size;
   GLint location;
   GLfloat value[4];
};

/* program/shader objects */
static GLuint fragShader[ 4 ];
static GLuint vertShader;
static GLuint program[ 4 ];

static GLint win = 0;
static GLfloat xRot = 0.0f, yRot = 0.0f, zRot = 0.0f;
static GLfloat Slice = 0.0;
static GLboolean Anim = GL_FALSE;


static void
Idle(void)
{
   Slice += 0.01;
   glutPostRedisplay();
}


static void
Redisplay(void)
{
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   glMultiTexCoord1f( GL_TEXTURE1, Slice );

   glPushMatrix();
   glRotatef(xRot, 1.0f, 0.0f, 0.0f);
   glRotatef(yRot, 0.0f, 1.0f, 0.0f);
   glRotatef(zRot, 0.0f, 0.0f, 1.0f);

   glutSolidTeapot( 1.0 );

   glPopMatrix();

   glutSwapBuffers();
}


static void
Reshape(int width, int height)
{
   glViewport(0, 0, width, height);
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   glFrustum(-1.0, 1.0, -1.0, 1.0, 5.0, 25.0);
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
   glTranslatef(0.0f, 0.0f, -15.0f);
}


static void
CleanUp(void)
{
   GLint i;

   glDeleteShader(vertShader);
   for( i = 0; i < 4; i++ ) {
      glDeleteShader(fragShader[ i ]);
      glDeleteProgram(program[ i ]);
   }
   glutDestroyWindow(win);
}


static void
Key(unsigned char key, int x, int y)
{
   const GLfloat step = 0.01;
  (void) x;
  (void) y;

   switch(key) {
   case 'a':
      Anim = !Anim;
      glutIdleFunc(Anim ? Idle : NULL);
      break;
   case 's':
      Slice -= step;
      break;
   case 'S':
      Slice += step;
      break;
   case 'z':
      zRot -= 1.0;
      break;
   case 'Z':
      zRot += 1.0;
      break;
   case '1':
   case '2':
   case '3':
   case '4':
      glUseProgram(program[ key - '1' ]);
      break;
   case 27:
      CleanUp();
      exit(0);
      break;
   }
   glutPostRedisplay();
}


static void
SpecialKey(int key, int x, int y)
{
   const GLfloat step = 3.0f;

  (void) x;
  (void) y;

   switch(key) {
   case GLUT_KEY_UP:
      xRot -= step;
      break;
   case GLUT_KEY_DOWN:
      xRot += step;
      break;
   case GLUT_KEY_LEFT:
      yRot -= step;
      break;
   case GLUT_KEY_RIGHT:
      yRot += step;
      break;
   }
   glutPostRedisplay();
}



static void
LoadAndCompileShader(GLuint shader, const char *text)
{
   GLint stat;

   glShaderSource(shader, 1, (const GLchar **) &text, NULL);

   glCompileShader(shader);

   glGetShaderiv(shader, GL_COMPILE_STATUS, &stat);
   if (!stat) {
      GLchar log[1000];
      GLsizei len;
      glGetShaderInfoLog(shader, 1000, &len, log);
      fprintf(stderr, "multinoise: problem compiling shader: %s\n", log);
      exit(1);
   }
   else {
      printf("Shader compiled OK\n");
   }
}


static void
CheckLink(GLuint prog)
{
   GLint stat;
   glGetProgramiv(prog, GL_LINK_STATUS, &stat);
   if (!stat) {
      GLchar log[1000];
      GLsizei len;
      glGetProgramInfoLog(prog, 1000, &len, log);
      fprintf(stderr, "Linker error:\n%s\n", log);
   }
   else {
      fprintf(stderr, "Link success!\n");
   }
}


static void
Init(void)
{
   GLint i;

   if (!GLAD_GL_VERSION_2_0) {
      printf("Warning: this program expects OpenGL 2.0\n");
      /*exit(1);*/
   }

   vertShader = glCreateShader(GL_VERTEX_SHADER);
   LoadAndCompileShader(vertShader, VertShaderText);

   for( i = 0; i < 4; i++ ) {
      fragShader[ i ] = glCreateShader(GL_FRAGMENT_SHADER);
      LoadAndCompileShader(fragShader[ i ], FragShaderText[ i ]);
      program[ i ] = glCreateProgram();
      glAttachShader(program[ i ], fragShader[ i ]);
      glAttachShader(program[ i ], vertShader);
      glLinkProgram(program[ i ]);
      CheckLink(program[ i ]);
   }

   glUseProgram(program[ 0 ]);

   assert(glGetError() == 0);

   glClearColor(0.4f, 0.4f, 0.8f, 0.0f);

   glColor3f(1, 0, 0);

   glFrontFace( GL_CW );
   glEnable( GL_CULL_FACE );
   glEnable( GL_DEPTH_TEST );
}


int
main(int argc, char *argv[])
{
   glutInit(&argc, argv);
   glutInitWindowSize(400, 400);
   glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
   win = glutCreateWindow(argv[0]);
   gladLoaderLoadGL();
   glutReshapeFunc(Reshape);
   glutKeyboardFunc(Key);
   glutSpecialFunc(SpecialKey);
   glutDisplayFunc(Redisplay);
   Init();
   glutMainLoop();
   gladLoaderUnloadGL();
   return 0;
}

