
My Entry for Ludum Dare 25, theme is "You are the Villain"

December 15-16 2012

tristan.lasty@gmail.com
twitter @tristan_lasty

Controls: 

WASD or arrow keys move the player
 +/- change camera height
F5 Reload Level (Handy for editing level file while game is running)
ESC Exit game


Object:

Avoid the patrolling guards, and steal all the loot!

Thieves are classed as villains, right?

Still very much Work in progress... not quite playable or fun yet.

Wanted to make some kind of sneak around the guards, hide to the shadows
almost theif-like game, but had to simplify it due to time constraints.



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

webp is google's webp format ... the code to decode/read images is a one liner,
which is why i love it so much


Licence:

Modified BSD Licence.

