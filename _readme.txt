Ray Tracer in the JACCO Template:
Ray Tracer code and tmpl8 modifications by Angel Angelov 
Credit to Jacco Bikker and Peter Shirley for helping me with the ray tracer.

This is a real-time whitted style ray tracer that I had to do for Block B of my first year of IGAD in Breda University of Applied Sciences.
It currently includes: UV texture mapping, point and directional lights, reflections and refractions.

Note: Stay in one place for a bit without changing the image for Temporal Anti-Aliasing to kick in to generate a clearer picture. 

Controls:
Scroll through scenes - C
Generate new scene - \ (backslash)

Camera Controls:
Camera yaw and pitch - mouse
Camera Movement - W, A, S, D
Vertical Camera Movement - R, D

Different View modes:
Regular view - 1 
View of the normals of objects - 2 
Full bright, lights ignored - 3

Lightsource movement:
Movement on the X Axis - Arrow Keys Up and Down
Movement on the Z Axis - Arrow Keys Left and Right
Movement on the Y Axis - Control and Shift

------------------------------------------------------------------------

BRIEF INFO ON THE 2021-09 TEMPLATE
Template, BUAS version https://www.buas.nl/games
IGAD/BUAS(NHTV)/UU - Jacco Bikker - 2006-2020

Purpose:
The template has been designed to make it easy to start coding C++
using games and graphics. It intends to offer the programmer a
simple library with the main purpose of providing a 32-bit graphical
window with a linear frame buffer. Some basic additional functionality
is available, such as sprites, bitmap fonts, and vector math support.

How to use:
1. Copy the template folder (or extract the zip) to a fresh folder for
   your project. 
2. Open the .sln file with any version of Visual Studio 2019.
3. Replace the example code in game.cpp with your own code.
4. Copy the 64-bit dll's from dlls_x64 to the project folder if you
   want to run a 64-bit build.
You can go further by:
- Expanding the game class in game.h;
- Implementing some of the empty functions for mouse and keyboard
  handling;
- Exploring the code of the template in surface.cpp and template.cpp.

When handing in assignments based on this template, please run
clean.bat prior to zipping the folder. This deletes any intermediate
files created during compilation.

The Template is a 'quickstart' template, and not meant to be elaborate
performant or complete. 
At some point, and depending on your requirements, you may want to
advance to a more full-fledged library, or you can expand the template
with OpenGL or SDL2 code.

Credits
Although the template is small and bare bones, it still uses a lot of
code gathered over the years:
- EasyCE's 5x5 bitmap font (primarily used for debugging);
- EasyCE's surface class (with lots of modifications);
- This version of the template uses SDL2 for low-level window handling,
  hopefully improving on the compatibility of earlier versions.

Copyright
This code is completely free to use and distribute in any form.

Template Naming
Starting January 2017, the name of the template represents the version.
This version also appears in the title of the window. Make sure you
are using the most recent version.

Breda, 2014-2019, Games @ Breda University (NHTV/IGAD)
Utrecht, 2015-2019, Utrecht University
Report problems and suggestions to Robbie Grigg if working on this 
connected to Breda University then email grigg.r@buas.nl
otherwise bikker.j@gmail.com .


Changelog
v2021-09
re-added precompiled headers for the sake of simplicity, and set-up the precompiled header compilation properly. 
fixed issues with delta time calculation
small changes to make the code more "compliant"

v2019-08:
removed headers from the precompiled header file to speed up compilation 
fixed uninitialised values in the surface class. 

v2017-01: 
initial DGDARC release.

v2017-02:
added a changelog.
debug mode now also emits .exe to project folder.
removed rogue SDL2 folder.
added assert.h to precomp.h.