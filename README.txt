
"Rogue Thief Stealer Person"
(Cloak, but no dagger)
By Lasty

My Entry for Ludum Dare 25, theme was "You are the Villain"

December 15th-17th 2012

tristan.lasty@gmail.com
twitter @tristan_lasty

Source Code: http://code.google.com/p/lasty-ld25-villain/
Ludum Dare website: http://www.ludumdare.com/

Win32 Binary in Google Code website, I may upload MACOSX app later.


Controls: 

F1 Help/Title screen (Displayed at start anyway)
WASD or arrow keys move the player
 +/- change camera height
F5 Reload Level (Also handy for editing level file while game is running)
ESC Exit game


Object:

Avoid the patrolling guards, and steal all the loot!

Thieves are classed as villains, right?

The game is pretty simple, and only one level. 
I had more Ideas but ran out of time.

Wanted to make some kind of sneak around the guards, hide to the shadows
almost theif-like game, but had to simplify it due to time constraints.


System Requirements:

A graphics card and driver that has OpenGL 3.x  (#version 120 shaders)
A sound device that works with SDL.
A Keyboard.


Compiling:

I had the extra libraries in source form under the /libs folder
but did not include them for space reasons

SDL2 (latest SVN/Git/hg)
webp2.0
glm-0.9.4.0
glee

Requires GCC 4.7 or clang 3.1 (untested)  It uses a lot of C++11 code.

My platform was Windows Vista 32 bit with MSYS/MinGW GCC4.7

I used CMake and ninja to compile:

mkdir Build
cd Build
cmake -G"Ninja" ..
ninja



Hacking Data Files:

Level files and GLSL shaders are in the Data directory, in plain text format.

OBJ files are exported from Blender, you must include normals and triangulate.

webp is Google's Webp format ... the code to decode/read images is a one liner,
which is why I love it so much.  Compression is better than PNG, too.


Hidden Controls:

o - toggle depth sorting
l - toggle show lights


Licence:

Modified BSD Licence.

