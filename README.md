![globjects Logo](globjects-logo.png "globjects")

*globjects* provides an object oriented C++ interface for OpenGL's programmable pipeline (3.0+).
*globjects* is a cross-platform library licenced under the [MIT license](http://opensource.org/licenses/MIT).

The current release is [globjects-0.4.1](https://github.com/hpicgs/globjects/releases/tag/v0.4.1).
To find out more about globjects and how to use it, check out our [wiki](https://github.com/hpicgs/globjects/wiki).


### Documentation

The documentation is regularly generated and can be found [here](http://libglow.org/doxygen-master).


### Dependencies

The following dev-libraries and programs need to be provided for correct CMake configuration:
* C++11 compatible compiler (e.g. gcc >=4.7, clang >=3.4, MSVC >= 2013 **Update 3**)
* CMake (>= 2.8.12): http://www.cmake.org/
* OpenGL C++ Bindings (glbinding >= 1.0): https://github.com/hpicgs/glbinding
* OpenGL Mathematics (GLM >= 0.9.5): http://glm.g-truc.net/, https://github.com/g-truc/glm
* Window and Context creation (GLFW >= 3.0.4): http://www.glfw.org/ (optional, needed for examples)


### Development Notes

If you are contributing to this project, please keep the following notes in mind:
* Add your name and email to the AUTHORS file.
* Follow coding conventions according to google's [C++ Style Guide](http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml).
* Commits should provide a meaningful  message, which uses the imperative, present tense: "change", not "changed" nor "changes", AND start with a capital letter.
* Commits should always refer to an issue: use ```#xxx```, ```fix #xxx```, or ```close #xxx```.
* Pull Requests are reviewed by at least one other developer on another platform.
* Use lazy initialization as often as possible for time consuming tasks.
* Member Variables always start with ```m_```, ```g_```, and ```s_``` (e.g.,```m_member```, ```g_global```, and ```s_static```)..
* Interface or abstract class names always start with Abstract (e.g., ```AbstractArray```).
* Enforce strict include sequence: cpp-related header, std, glm, glbinding, globjects.
