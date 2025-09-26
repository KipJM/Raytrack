![Raytrack banner](https://github.com/KipJM/Raytrack/blob/master/promo/banner.png)
# Raytrack
| An interactive, multithreaded software toy raytracing renderer with an user interface, written in c++.

## Installation / Usage
Go to [Releases](https://github.com/KipJM/Raytrack/releases/), download the latest version of Raytrack.
Simply run `Raytrack.exe` within the zip file to launch the Raytrack graphical user interface.

![Raytrack screenshot 1](https://github.com/KipJM/Raytrack/blob/master/promo/screenshot1.png)

## Requirements
Only Windows is currently supported, I don't have Linux/MacOS systems on hand to build the projects on.  

Raytracing itself heavily depends on your system's multicore CPU performance, it will take longer to render if your computer's CPU is weak.  

**However**, to draw the graphical user interface, and to draw the render on screen, **OpenGL** is used and used only for that purpose. OpenGL 4.1 or higher is required, which should be supported by any machine made after 2010. Note that iGPUs (PCs and laptops with no dedicated GPU) ARE supported.  

**If for whatever reason your system does not support OpenGL 4.1 (e.g. you're using a virtual machine with no GPU), you'll need to use an OpenGL software renderer such as Mesa3D, however this is not explicitily supported by the scope of this project.**

(For advanced users) If your system is more locked down, Raytrack has a feature which you can import an image file as a texture. This feature is not mandatory, but you have to allow filesystem access if you wish to use it.

![Raytrack screenshot 2](https://github.com/KipJM/Raytrack/blob/master/promo/screenshot2.png)

## About
The raytracer is based on the [Ray Tracing in One Weekend](https://raytracing.github.io/) series of books, which I highly recommend.  
I made this project mostly to learn C++ and computer graphics. Along the way I decided to add a GUI and multithreading, then things got out of hand.
So after learning a bit about OpenGL, multithreading, and memory management hell, now I have a fully featured """realtime""" raytracer with a full-featured graphical user interface.

The raytracer itself is completely CPU-based, however for drawing the GUI, I decided to use OpenGL and ImGUI.

some unique features about the project:
- Pretty okay performance and memory footprint: It's c++, what do you expect?
- Arbitrary multithreading with generally no crashes
- Intuitive UI with plenty of help content
- Pretty full-featured renderer with all the basics: geometry, material, textures, BVH optimization, etc...
- A random rendering system that very much improves rendering responsiveness

*Yes, the banner is a parody of those 3D software*

## Building
*coming soon!*  
Built using Visual Studio (For building to Windows), but MinGW also works with minimal or nochanges.  
Just use the root CmakeLists.txt, everything should build with no problem.

## This project uses
The Raytrack raytracing engine does not use any external libraries.
However to interface with image files and to draw the GUI, this project uses:
- GLAD generated loader (CC0)
- glfw (zlib)
- stb_image (MIT)
- ImGUI (MIT)
- ImFileDialog (MIT)

Their source code can be found within the include/ folder.

Happy rendering!  
**KIP**  
[kip.gay](https://kip.gay/)
