OPEN GL FLAT PLANE ANIMATION - by Tom Lewis

Shader Loaders and Texture Loaders were written by our tutor Paul Angel.
This project makes use of freeGLUT, glew and GU CoreStructures Library. 

This project is STRICTLY non-profit and was produced for my coursework at the University of South Wales.

Build Information:

This project was originally developed using Visual Studio 2013. At the moment, the project has not been built into an independant executable, so other versions of VS(Such as
2015 Community) have to be run using a compatibility mode. This can be done by changing the Platform Toolset in the Project Properties. Executable building is still a WIP.

The gldemo.sln file located in Simulation/Simulation is the VS project file to run.

Instructions:

When ran through VS, the main window will start, as well as a comand prompt window. The command prompt window will have instructions on how to control the scene
printed to it. 

Notes:

This project was developed as an exercise to demonstrate my understanding of graphical modelling and rendering techniques, and to prove my 
understanding of techniques used for real-time animation.

For this piece of coursework, I got 81%, one of the best marks in the entire class. I am very proud of the success this project achieved 
and I believe I have learned a great deal about 3D graphics programming using OpenGL and GLSL.

Despite this, there are a number of things that I would improve upon were I to do this project again. Firstly, even though it is incredibley useful, I would not
use the freeGLUT library to handle the common processes associated with my program. FreeGLUT is a very powerful kit, but it comes at the cost of using global variables.
Freeglut requires the use of functions with no arguements to provide services such as scene updating, so globals have to be used for pieces of information that is shared
by different scopes. I did band all this global information into a singular structure to try and neaten the code up a bit, but it essentially works the same is if 
they were all individual variables. Secondly I would add more objects to at least one of the scenes. An idea I had would be to add a falling sphere in mode 5,
The Reverse Water Droplet mode. The animation of the plane would only begin when the sphere and plane collided. This would show interaction between objects in the scene,
effectively working as primitive physics. I would also like to expand the pixel shader to include lighting. I would like to introduce a Phong Blinn shader model to provide
shadows. 
