#version 330

// UBO to store the camera projection and view matrices
uniform CameraMatrixBlock {

	mat4 viewMatrix;
	mat4 projectionMatrix;

} cam;


uniform mat4 modelMatrix;

layout (location=0) in vec4 vertexPos;
layout (location=1) in vec4 vertexColour;

out vec4 colour;

void main(void) {

	colour = vertexColour;

	mat4 P = cam.projectionMatrix;
	mat4 V = cam.viewMatrix;

	mat4 mvpMatrix = P * V * modelMatrix;

	gl_Position = mvpMatrix * vertexPos;
}
