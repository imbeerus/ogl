// Include standard headers
#include <stdio.h>
#include <stdlib.h>
// Include GLEW
#include <GL/glew.h>
// Include GLFW
#include <GLFW/glfw3.h>
// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <common/shader.hpp>

static const int WINDOW_WIDTH = 1280;
static const int WINDOW_HEIGHT = 720;

// Our vertices. Three consecutive floats give a 3D vertex; Three consecutive vertices give a triangle.
// A cube has 6 faces with 2 triangles each, so this makes 6*2=12 triangles, and 12*3 vertices
static const GLfloat g_cube_vertex_buffer_data[] = {
	-1.0f,-1.0f,-1.0f, // triangle 1 : begin
	-1.0f,-1.0f, 1.0f,
	-1.0f, 1.0f, 1.0f, // triangle 1 : end
	1.0f, 1.0f,-1.0f, // triangle 2 : begin
	-1.0f,-1.0f,-1.0f,
	-1.0f, 1.0f,-1.0f, // triangle 2 : end
	1.0f,-1.0f, 1.0f,
	-1.0f,-1.0f,-1.0f,
	1.0f,-1.0f,-1.0f,
	1.0f, 1.0f,-1.0f,
	1.0f,-1.0f,-1.0f,
	-1.0f,-1.0f,-1.0f,
	-1.0f,-1.0f,-1.0f,
	-1.0f, 1.0f, 1.0f,
	-1.0f, 1.0f,-1.0f,
	1.0f,-1.0f, 1.0f,
	-1.0f,-1.0f, 1.0f,
	-1.0f,-1.0f,-1.0f,
	-1.0f, 1.0f, 1.0f,
	-1.0f,-1.0f, 1.0f,
	1.0f,-1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f,-1.0f,-1.0f,
	1.0f, 1.0f,-1.0f,
	1.0f,-1.0f,-1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f,-1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f, 1.0f,-1.0f,
	-1.0f, 1.0f,-1.0f,
	1.0f, 1.0f, 1.0f,
	-1.0f, 1.0f,-1.0f,
	-1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	-1.0f, 1.0f, 1.0f,
	1.0f,-1.0f, 1.0f
};

static const GLfloat g_triangle_vertex_buffer_data[] = {
	-3.0f, -2.0f, -2.0f,
	-2.0f, -2.0f,-3.0f,
	-1.0f, 0.0f,-1.0f
};

// One color for each vertex. 
// Our vertices. Three consecutive floats give a 3D vertex; Three consecutive vertices give a triangle.
// A cube has 6 faces with 2 triangles each, so this makes 6*2=12 triangles, and 12*3 vertices
static GLfloat g_color_buffer_data[12 * 3 * 3];

GLuint VertexArrayID;

glm::mat4 Projection;
glm::mat4 View;

GLuint shaderProgramID;

GLuint CubeMatrixID;
GLuint cubeVertexbuffer;
glm::mat4 CubeMVP;

GLuint TriangleMatrixID;
GLuint triangleVertexbuffer;
glm::mat4 TriangleMVP;

GLuint colorbuffer;

GLFWwindow* window;

bool handleEscapeKey() {
	return glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0;
}

void setupGLFW() {
	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_FALSE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //We don't want the old OpenGL 
}

void updateColors() {
	for (int v = 0; v < 12 * 3; v++) {
		g_color_buffer_data[3 * v + 0] = (float(rand()) / float((RAND_MAX - v)));
		g_color_buffer_data[3 * v + 1] = (float(rand()) / float((RAND_MAX - v)));
		g_color_buffer_data[3 * v + 2] = (float(rand()) / float((RAND_MAX - v)));
	}
}

void initVertexArray() {
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);
}

void bindColorBuffer() {
	glGenBuffers(1, &colorbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_color_buffer_data), g_color_buffer_data, GL_STATIC_DRAW);
}

void initCubeMVP() {
	// Get a handle for our "MVP" uniform
	CubeMatrixID = glGetUniformLocation(shaderProgramID, "MVP");
	// Model matrix : an identity matrix (model will be at the origin)
	glm::mat4 CubeModel = glm::mat4(1.0f);
	// Our ModelViewProjection : multiplication of our 3 matrices
	CubeMVP = Projection * View * CubeModel; // Remember, matrix multiplication is the other way around
}

void initTriangleMVP() {
	TriangleMatrixID = glGetUniformLocation(shaderProgramID, "MVP");
	glm::mat4 TriangleModel = glm::mat4(1.0f);
	TriangleMVP = Projection * View * TriangleModel;
}

void initCubeBuffer() {
	glGenBuffers(1, &cubeVertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, cubeVertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_cube_vertex_buffer_data), g_cube_vertex_buffer_data, GL_STATIC_DRAW);
}

void initTriangleBuffer() {
	glGenBuffers(1, &triangleVertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, triangleVertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_triangle_vertex_buffer_data), g_triangle_vertex_buffer_data, GL_STATIC_DRAW);
}

void drawCube() {
	// 1rst attribute buffer : vertices
	glBindBuffer(GL_ARRAY_BUFFER, cubeVertexbuffer);
	glVertexAttribPointer(
		0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);

	// 2nd attribute buffer : colors
	glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_cube_vertex_buffer_data), g_color_buffer_data, GL_STATIC_DRAW);
	glVertexAttribPointer(
		1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
		3,                                // size
		GL_FLOAT,                         // type
		GL_FALSE,                         // normalized?
		0,                                // stride
		(void*)0                          // array buffer offset
	);

	// Draw the cube !
	glDrawArrays(GL_TRIANGLES, 0, 12 * 3); // 12*3 indices starting at 0 -> 12 triangles -> 6 squares
}

void drawTriangle() {
	glBindBuffer(GL_ARRAY_BUFFER, triangleVertexbuffer);
	glVertexAttribPointer(
		0,                  
		3,                
		GL_FLOAT,           
		GL_FALSE,          
		0,                  
		(void*)0            
	);

	glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_triangle_vertex_buffer_data), g_color_buffer_data, GL_STATIC_DRAW);
	glVertexAttribPointer(
		1,                                
		3,                               
		GL_FLOAT,                         
		GL_FALSE,                         
		0,                                
		(void*)0                          
	);

	glDrawArrays(GL_TRIANGLES, 0, 3);
}

int main(void)
{
	// Initialise GLFW
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		getchar();
		return -1;
	}

	setupGLFW();

	// Open a window and create its OpenGL context
	window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Playground", NULL, NULL);
	if (window == NULL) {
		fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
		getchar();
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

    // Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);

	// Dark background
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	initVertexArray();

	// Create and compile our GLSL program from the shaders
	shaderProgramID = LoadShaders("SimpleVertexShader.vertexshader", "SimpleFragmentShader.fragmentshader");

	// Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	Projection = glm::perspective(glm::radians(45.0f), (float) WINDOW_WIDTH / (float) WINDOW_HEIGHT, 0.1f, 100.0f);
	// Or, for an ortho camera :
	// glm::mat4 Projection = glm::ortho(-10.0f,10.0f,-10.0f,10.0f,0.0f,100.0f); // In world coordinates

	// Camera matrix
    View = glm::lookAt(
		glm::vec3(10, -10, 10), // Camera is at (4,-3,3), in World Space
		glm::vec3(0, 0, 0), // and looks at the origin
		glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
	);

	initCubeMVP();
	initCubeBuffer();

	initTriangleMVP();
	initTriangleBuffer();

	updateColors();
	bindColorBuffer();

	do {
		
		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

		// Use our shader
		glUseProgram(shaderProgramID);

		// Send our transformation to the currently bound shader, 
		// in the "MVP" uniform
		glUniformMatrix4fv(CubeMatrixID, 1, GL_FALSE, &CubeMVP[0][0]);
		glUniformMatrix4fv(TriangleMatrixID, 1, GL_FALSE, &TriangleMVP[0][0]);

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);

		updateColors();

		drawCube();
		drawTriangle();

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

	} 
	while (handleEscapeKey());

	// Cleanup VBO and shader
	glDeleteBuffers(1, &cubeVertexbuffer);
	glDeleteBuffers(1, &triangleVertexbuffer);
	glDeleteBuffers(1, &colorbuffer);
	glDeleteProgram(shaderProgramID);
	glDeleteVertexArrays(1, &VertexArrayID);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}
