
This is a set of cross-platform wrappers over the OpenGL ES 2.0 API.

Please note that OpenGL ES context creation and management is outside of the scope
of this library.

This library wraps all OpenGL types, constants, and functions into *GL*
and *gl* namespaces. For example, to call *glClear* function you may write the
following piece of code:

     gl::Clear(GL::COLOR_BUFFER_BIT | GL::DEPTH_BUFFER_BIT);

In debug builds, all calls to OpenGL functions are checked with *glError*
calls.

Currently, there are three wrappers available:

1. The **normal** wrapper is for use in all projects except for those built
   using the Qt library. On systems with OpenGL ES 2.0 support this wrapper
   invokes OpenGL ES functions directly. On desktop systems (Windows, Unix,
   OSX) OpenGL ES calls are implemented through the full version of OpenGL (with
   the help of the [OpenGL Extension Wrangler](http://glew.sourceforge.net/)
   library).

2. The **qt** wrapper is intended for use in projects built with the
   [Qt library](http://qt-project.org/)). OpenGL calls are made through the
   [QGLFunctions](http://qt-project.org/doc/qt-5/qglfunctions.html) class.

3. At last, there is the **dummy** wrapper with all functions implemented as
   empty stubs. This wrapper is useful for compilation on platforms without
   OpenGL support and as a skeleton for new wrappers.

**Before usage, wrappers should be initialized by calling *gl::InitWrappers()*
function.**


License
=======

Copyright © 2014-2015 Nikolay Zapolnov (zapolnov@gmail.com).

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
