#version 330

uniform float numberPass; // Time passed from c++ program, used for animation.
uniform int mode; // Mode number passed in from c++ OpenGL program used to change the behaviour of the simulation. Simulation mode can be changed with keyboard actions c++ side.
uniform float randomNumber, randomNumber2; // Random numbers passed from c++ OpenGL program. Random number one is used for vertex positions in mode 4, random number 2 is used for random colours in the pixel shader.

// UBO to store the camera projection and view matrices
uniform CameraMatrixBlock {

	mat4 viewMatrix;
	mat4 projectionMatrix;

} cam;

layout (location=0) in vec4 vertexPos;
layout (location=1) in vec4 vertexColour;

out vec4 colour;
out vec4 posFrag;
out float randomNumberCol; // pass random number for use in colour selection in the pixel shader. 
out float changingNumber; // pass time to pixel shader for animated colour.
flat out int fragMode;


vec4 calcOffset(vec4 pos) // operations performed per vertex to create the desired effects.
{

	if (mode == 1) //example terrain
	{

		if (pos.x <8 || pos.z < 8)
		{
			pos.y = cos(pos.x) + cos(pos.z);
		}
		else
		{
			pos.y = pos.y + 10;
		}

	}

	if (mode == 2) // Low Poly Look Ocean
	{

		pos.y = cos(numberPass*sin(pos.z)+cosh(pos.x));

	}

	if (mode == 3) // Red Hills
	{	

		pos.y = cos(pos.z * sin(pos.x));

	}

	if (mode == 4) // Colour blending and random movement
	{

		pos.y = sin(randomNumber*numberPass) * cos((pos.x * randomNumber * pos.z * randomNumber) + numberPass);
		pos.y = pos.y * 3 * (randomNumber2+0.5);
		
	}

	if (mode == 5) // Reverse water droplet
	{
		
		pos.y = (3*cos(sqrt((((pos.z*pos.z)+numberPass+1)+((pos.x*pos.x)+numberPass))*0.025*numberPass))/exp((((pos.z*pos.z)+numberPass)+((pos.x*pos.x)+numberPass))*0.00002*numberPass));
		
	}

	if (mode == 6) // Fractional Pixels
	{

		pos.y = cos(pos.x/5 + numberPass) + sin(pos.z/5 + numberPass) ;

	}

	return pos;

}

void main(void) {

	vec4 posOffset;
	fragMode = mode;
	changingNumber = numberPass;
	randomNumberCol = randomNumber2;
	posOffset = calcOffset(vertexPos);

	posFrag = posOffset;
	
	colour = vertexColour;

	mat4 P = cam.projectionMatrix;
	mat4 V = cam.viewMatrix;

	mat4 mvpMatrix = P * V;

	

	gl_Position = mvpMatrix * posOffset;
}
