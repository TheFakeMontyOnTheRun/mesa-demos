# Copyright © 2022 Collabora Ltd

# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:

# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.

# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

progs = [
  'aaindex',
  'aapoly',
  'aargb',
  'accanti',
  'accpersp',
  'alpha',
  'alpha3D',
  'anti',
  'bezcurve',
  'bezmesh',
  'checker',
  'clip',
  'colormat',
  'combiner',
  'cube',
  'cubemap',
  'depthcue',
  'dof',
  'double',
  'drawf',
  'feedback',
  'fog',
  'fogcoord',
  'fogindex',
  'font',
  'hello',
  'image',
  'light',
  'lines',
  'list',
  'material',
  'mipmap',
  'model',
  'movelight',
  'multisamp',
  'multitex',
  'mvarray',
  'nurbs',
  'pickdepth',
  'picksquare',
  'plane',
  'planet',
  'pointp',
  'polyoff',
  'polys',
  'quadric',
  'robot',
  'sccolorlight',
  'scene',
  'scenebamb',
  'sceneflat',
  'select',
  'shadowmap',
  'smooth',
  'stencil',
  'stroke',
  'surface',
  'surfpoints',
  'teaambient',
  'teapots',
  'tess',
  'tesswind',
  'texbind',
  'texgen',
  'texprox',
  'texsub',
  'texture3d',
  'texturesurf',
  'torus',
  'trim',
  'unproject',
  'varray',
  'wrap',
]
foreach p : progs
  executable(
    p, files(p + '.c'),
    dependencies: [
      dep_gl, dep_glu, dep_glut, dep_m, idep_glad, idep_util
    ])
endforeach
