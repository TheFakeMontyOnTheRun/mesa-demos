/**
 * Test using a geometry shader to implement polygon outlining.
 * Using GLSL 1.50 (or any later version)
 *
 * Based on the technique "Single-Pass Wireframe Rendering" by Andreas
 * Bærentzen, Steen Lund Nielsen, Mikkel Gjael, Bent D. Larsen & Niels
 * Jaergen Christensen, SIGGRAPH 2006
 *
 * Brian Paul
 * May 2012
 */

#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "glad/gl.h"
#include "glut_wrap.h"
#include "shaderutil.h"
#include "trackball.h"
#include "matrix.h"

static GLint WinWidth = 500, WinHeight = 500;
static GLint Win = 0;
static GLuint VertShader, GeomShader, FragShader, Program;
static GLuint vao, vbo;
static GLboolean Anim = GL_TRUE;
static int uViewportSize = -1, uModelViewProj = -1, uColor = -1;

static const GLfloat Orange[4] = {1.0, 0.6, 0.0, 1};

static float CurQuat[4] = { 0, 0, 0, 1 };
static GLboolean ButtonDown = GL_FALSE;
static GLint ButtonX, ButtonY;

static float Projection[4][4];


static void
CheckError(int line)
{
   GLenum err = glGetError();
   if (err) {
      printf("GL Error %s (0x%x) at line %d\n",
             gluErrorString(err), (int) err, line);
   }
}


static void
Redisplay(void)
{
   float rot[4][4];
   float mvp[4][4];

   glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

   /* Build the modelview * projection matrix */
   build_rotmatrix(rot, CurQuat);
   mat4_identity(mvp);
   mat4_multiply(mvp, Projection);
   mat4_translate(mvp, 0, 0, -10);
   mat4_multiply(mvp, rot);

   /* Set the MVP matrix */
   glUniformMatrix4fv(uModelViewProj, 1, GL_FALSE, (float *) mvp);

   /* Draw */
   glDrawArrays(GL_TRIANGLES, 0, 3);

   glutSwapBuffers();
}


static void
Idle(void)
{
   static const float yAxis[3] = {0, 1, 0};
   static double t0 = -1.;
   float quat[4];
   double dt, t = glutGet(GLUT_ELAPSED_TIME) / 2000.0;
   if (t0 < 0.0)
      t0 = t;
   dt = t - t0;
   t0 = t;

   axis_to_quat(yAxis, 2.0 * dt, quat);
   add_quats(quat, CurQuat, CurQuat);

   glutPostRedisplay();
}


static void
Reshape(int width, int height)
{
   float ar = (float) width / height;
   WinWidth = width;
   WinHeight = height;
   glViewport(0, 0, width, height);
   mat4_frustum_gl(Projection, -ar, ar, -1, 1, 3, 25);

   /* pass viewport dims to the shader */
   {
      GLfloat viewport[4];
      glGetFloatv(GL_VIEWPORT, viewport);
      glUniform2f(uViewportSize, viewport[2], viewport[3]);
   }
}


static void
MouseMotion(int x, int y)
{
   if (ButtonDown) {
      float x0 = (2.0 * ButtonX - WinWidth) / WinWidth;
      float y0 = (WinHeight - 2.0 * ButtonY) / WinHeight;
      float x1 = (2.0 * x - WinWidth) / WinWidth;
      float y1 = (WinHeight - 2.0 * y) / WinHeight;
      float q[4];

      trackball(q, x0, y0, x1, y1);
      ButtonX = x;
      ButtonY = y;
      add_quats(q, CurQuat, CurQuat);

      glutPostRedisplay();
   }
}


static void
MouseButton(int button, int state, int x, int y)
{
  if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
     ButtonDown = GL_TRUE;
     ButtonX = x;
     ButtonY = y;
  }
  else if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
     ButtonDown = GL_FALSE;
  }
}


static void
CleanUp(void)
{
   glDeleteShader(FragShader);
   glDeleteShader(VertShader);
   glDeleteShader(GeomShader);
   glDeleteProgram(Program);
   glDeleteVertexArrays(1, &vao);
   glDeleteBuffers(1, &vbo);
   glutDestroyWindow(Win);
}


static void
Key(unsigned char key, int x, int y)
{
  (void) x;
  (void) y;

   switch(key) {
   case ' ':
   case 'a':
      Anim = !Anim;
      if (Anim) {
         glutIdleFunc(Idle);
      }
      else
         glutIdleFunc(NULL);
      break;
   case 27:
      CleanUp();
      exit(0);
      break;
   }
   glutPostRedisplay();
}


static void
Init(void)
{
   static const char *vertShaderText =
      "#version 150 \n"
      "uniform mat4 ModelViewProjection; \n"
      "in vec4 Vertex; \n"
      "void main() \n"
      "{ \n"
      "   gl_Position = ModelViewProjection * Vertex; \n"
      "} \n";
   static const char *geomShaderText =
      "#version 150 \n"
      "layout(triangles) in; \n"
      "layout(triangle_strip, max_vertices = 3) out; \n"
      "uniform vec2 ViewportSize; \n"
      "out vec2 Vert0, Vert1, Vert2; \n"
      "\n"
      "// Transform NDC coord to window coord \n"
      "vec2 vpxform(vec4 p) \n"
      "{ \n"
      "   return (p.xy / p.w + 1.0) * 0.5 * ViewportSize; \n"
      "} \n"
      "\n"
      "void main() \n"
      "{ \n"
      "   Vert0 = vpxform(gl_in[0].gl_Position); \n"
      "   Vert1 = vpxform(gl_in[1].gl_Position); \n"
      "   Vert2 = vpxform(gl_in[2].gl_Position); \n"
      "   gl_Position = gl_in[0].gl_Position; \n"
      "   EmitVertex(); \n"
      "   gl_Position = gl_in[1].gl_Position; \n"
      "   EmitVertex(); \n"
      "   gl_Position = gl_in[2].gl_Position; \n"
      "   EmitVertex(); \n"
      "} \n";
   static const char *fragShaderText =
      "#version 150 \n"
      "#define LINE_WIDTH 2.5 \n"
      "uniform vec4 Color; \n"
      "in vec2 Vert0, Vert1, Vert2; \n"
      "out vec4 FragColor; \n"
      "// Compute distance from a point to a line \n"
      "float point_line_dist(vec2 p, vec2 v1, vec2 v2) \n"
      "{ \n"
      "   float s = (v2.x - v1.x) * (v1.y - p.y) - (v1.x - p.x) * (v2.y - v1.y); \n"
      "   float t = length(v2 - v1); \n"
      "   return abs(s) / t; \n"
      "} \n"
      "\n"
      "void main() \n"
      "{ \n"
      "   float d0 = point_line_dist(gl_FragCoord.xy, Vert0, Vert1); \n"
      "   float d1 = point_line_dist(gl_FragCoord.xy, Vert1, Vert2); \n"
      "   float d2 = point_line_dist(gl_FragCoord.xy, Vert2, Vert0); \n"
      "   float m = min(d0, min(d1, d2)); \n"
      "   FragColor = Color * smoothstep(0.0, LINE_WIDTH, m); \n"
      "} \n";
   static const GLfloat verts[3][2] = {
      { -1, -1 },
      {  1, -1 },
      {  0,  1 }
   };

   if (!ShadersSupported())
      exit(1);

   if (!GLAD_GL_VERSION_3_2) {
      fprintf(stderr, "Sorry, OpenGL 3.2 or later required.\n");
      exit(1);
   }

   VertShader = CompileShaderText(GL_VERTEX_SHADER, vertShaderText);
   FragShader = CompileShaderText(GL_FRAGMENT_SHADER, fragShaderText);
   GeomShader = CompileShaderText(GL_GEOMETRY_SHADER, geomShaderText);

   Program = LinkShaders3(VertShader, GeomShader, FragShader);
   assert(Program);
   CheckError(__LINE__);

   glBindAttribLocation(Program, 0, "Vertex");
   glBindFragDataLocation(Program, 0, "FragColor");

   /* relink */
   glLinkProgram(Program);

   assert(glIsProgram(Program));
   assert(glIsShader(FragShader));
   assert(glIsShader(VertShader));
   assert(glIsShader(GeomShader));

   glUseProgram(Program);

   uViewportSize = glGetUniformLocation(Program, "ViewportSize");
   uModelViewProj = glGetUniformLocation(Program, "ModelViewProjection");
   uColor = glGetUniformLocation(Program, "Color");

   glUniform4fv(uColor, 1, Orange);

   glGenBuffers(1, &vbo);
   glBindBuffer(GL_ARRAY_BUFFER, vbo);
   glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

   glGenVertexArrays(1, &vao);
   glBindVertexArray(vao);

   glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
   glEnableVertexAttribArray(0);

   glClearColor(0.3f, 0.3f, 0.3f, 0.0f);
   glEnable(GL_DEPTH_TEST);

   printf("GL_RENDERER = %s\n",(const char *) glGetString(GL_RENDERER));
}


int
main(int argc, char *argv[])
{
   glutInit(&argc, argv);
   glutInitWindowSize(WinWidth, WinHeight);
#ifdef HAVE_FREEGLUT
   glutInitContextVersion(3, 2);
   glutInitContextProfile(GLUT_CORE_PROFILE);
   glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
#elif defined __APPLE__
   glutInitDisplayMode(GLUT_3_2_CORE_PROFILE | GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
#else
   glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
#endif
   Win = glutCreateWindow(argv[0]);
   gladLoaderLoadGL();
   glGetError();
   glutReshapeFunc(Reshape);
   glutKeyboardFunc(Key);
   glutDisplayFunc(Redisplay);
   glutMotionFunc(MouseMotion);
   glutMouseFunc(MouseButton);
   if (Anim)
      glutIdleFunc(Idle);

   Init();
   glutMainLoop();
   gladLoaderUnloadGL();
   return 0;
}
