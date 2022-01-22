#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// GLM library to deal with matrix operations
#include <glm/glm.hpp>
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::perspective
#include <glm/gtc/type_ptr.hpp>
#include "glm/ext.hpp"


#include "textfile.c"

int gl_width = 1042;
int gl_height = 620;

void glfw_window_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void render(double);
unsigned int loadTexture(const char *path);

GLuint shader_program = 0; // shader program to set render pipeline
GLuint vao = 0; // Vertext Array Object to set input data
GLint model_location, view_location, materialA_location, materialD_location, materialS_location, materialSH_location, view_pos_location, proj_location, lightP_location, lightA_location, lightD_location, lightS_location, normal_to_world_location, lightP_location_2, lightA_location_2, lightD_location_2, lightS_location_2; // Uniforms for transformation matrices

// Shader names
const char *vertexFileName = "spinningcube_withlight_vs.glsl";
const char *fragmentFileName = "spinningcube_withlight_fs.glsl";

// Camera
glm::vec3 camera_pos(0.0f, 0.0f, 3.0f);

// Lighting
glm::vec3 light_pos(1.2f, 1.0f, 2.0f);
glm::vec3 light_ambient(0.2f, 0.2f, 0.2f);
glm::vec3 light_diffuse(0.5f, 0.5f, 0.5f);
glm::vec3 light_specular(1.0f, 1.0f, 1.0f);

// Material
glm::vec3 material_ambient(1.0f, 0.5f, 0.31f);
const GLfloat material_shininess = 10.0f;

// Texture
unsigned int diffuseMap;
unsigned int specularMap;

int main() {
	// start GL context and O/S window using the GLFW helper library
	if (!glfwInit()) {
		fprintf(stderr, "ERROR: could not start GLFW3\n");
		return 1;
	}

	//glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	//glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(gl_width, gl_height, "My spinning cube", NULL, NULL);
	if (!window) {
		fprintf(stderr, "ERROR: could not open window with GLFW3\n");
		glfwTerminate();
		return 1;
	}
	glfwSetWindowSizeCallback(window, glfw_window_size_callback);
	glfwMakeContextCurrent(window);

	// start GLEW extension handler
	// glewExperimental = GL_TRUE;
	glewInit();

	// get version info
	const GLubyte* vendor = glGetString(GL_VENDOR); // get vendor string
	const GLubyte* renderer = glGetString(GL_RENDERER); // get renderer string
	const GLubyte* glversion = glGetString(GL_VERSION); // version as a string
	const GLubyte* glslversion = glGetString(GL_SHADING_LANGUAGE_VERSION); // version as a string
	printf("Vendor: %s\n", vendor);
	printf("Renderer: %s\n", renderer);
	printf("OpenGL version supported %s\n", glversion);
	printf("GLSL version supported %s\n", glslversion);
	printf("Starting viewport: (width: %d, height: %d)\n", gl_width, gl_height);

	// Enable Depth test: only draw onto a pixel if fragment closer to viewer
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS); // set a smaller value as "closer"

	// Vertex Shader
	char* vertex_shader = textFileRead(vertexFileName);

	// Fragment Shader
	char* fragment_shader = textFileRead(fragmentFileName);

	// Shaders compilation
	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs, 1, &vertex_shader, NULL);
	free(vertex_shader);
	glCompileShader(vs);

	int  success;
	char infoLog[512];
	glGetShaderiv(vs, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vs, 512, NULL, infoLog);
		printf("ERROR: Vertex Shader compilation failed!\n%s\n", infoLog);

		return(1);
	}

	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs, 1, &fragment_shader, NULL);
	free(fragment_shader);
	glCompileShader(fs);

	glGetShaderiv(fs, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fs, 512, NULL, infoLog);
		printf("ERROR: Fragment Shader compilation failed!\n%s\n", infoLog);

		return(1);
	}

	// Create program, attach shaders to it and link it
	shader_program = glCreateProgram();
	glAttachShader(shader_program, fs);
	glAttachShader(shader_program, vs);
	glLinkProgram(shader_program);

	glValidateProgram(shader_program);
	glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
	if(!success) {
		glGetProgramInfoLog(shader_program, 512, NULL, infoLog);
		printf("ERROR: Shader Program linking failed!\n%s\n", infoLog);

		return(1);
	}

	// Release shader objects
	glDeleteShader(vs);
	glDeleteShader(fs);

	// Vertex Array Object
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// Cube to be rendered
	//
	//          0        3
	//       7        4 <-- top-right-near
	// bottom
	// left
	// far ---> 1        2
	//       6        5
	//
	const GLfloat vertex_positions[] = { 
        // positions          // normals           // texture coords
	        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
	};

	// Vertex Buffer Object (for vertex coordinates)
	GLuint vbo = 0;
	glGenBuffers(3, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_positions), vertex_positions, GL_STATIC_DRAW);

	// Vertex attributes
	// 0: vertex position (x, y, z)
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// 1: vertex normals (x, y, z)
	// normal
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
	
	  // texture
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);


	// Unbind vbo (it was conveniently registered by VertexAttribPointer)
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Unbind vao
	glBindVertexArray(0);

	// Uniforms
	// - Model matrix
	// - View matrix
	// - Projection matrix
	// - Normal matrix: normal vectors from local to world coordinates
	// - Camera position
	// - Light data
	// - Material data
	

	
	// get location of fs and vs variables
	model_location = glGetUniformLocation(shader_program, "model");
	view_location = glGetUniformLocation(shader_program, "view");
	proj_location = glGetUniformLocation(shader_program, "projection");

        //material
	normal_to_world_location = glGetUniformLocation(shader_program, "normal_to_world");
	materialD_location = glGetUniformLocation(shader_program, "material.diffuse");
	materialS_location = glGetUniformLocation(shader_program, "material.specular");
	materialSH_location = glGetUniformLocation(shader_program, "material.shininess");
	view_pos_location = glGetUniformLocation(shader_program, "view_pos");
	
	// light 1
	lightA_location = glGetUniformLocation(shader_program, "pointLights[0].ambient");
	lightD_location = glGetUniformLocation(shader_program, "pointLights[0].diffuse");
	lightS_location = glGetUniformLocation(shader_program, "pointLights[0].specular");
	lightP_location = glGetUniformLocation(shader_program, "pointLights[0].position");


	// light 2
	lightA_location_2 = glGetUniformLocation(shader_program, "pointLights[1].ambient");
	lightD_location_2 = glGetUniformLocation(shader_program, "pointLights[1].diffuse");
	lightS_location_2 = glGetUniformLocation(shader_program, "pointLights[1].specular");
	lightP_location_2 = glGetUniformLocation(shader_program, "pointLights[1].position");
	
	// textures
        diffuseMap = loadTexture("container2.png");
        specularMap = loadTexture("container2_specular.png");

	// Render loop
	while(!glfwWindowShouldClose(window)) {

		processInput(window);

		render(glfwGetTime());

		glfwSwapBuffers(window);

		glfwPollEvents();
	}

	glfwTerminate();

	return 0;
}

void render(double currentTime) {
	float f = (float)currentTime * 0.3f;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glViewport(0, 0, gl_width, gl_height);

	glUseProgram(shader_program);
	glBindVertexArray(vao);

	glm::mat4 model, view, projection, cube_2;
	glm::mat3 normal_to_world; //declarar variables

	//matrices
	glUniform1i(materialD_location, 0);
	glUniform1i(materialS_location, 1);
	glUniform1f(materialSH_location, material_shininess);

	glUniform3f(view_pos_location, camera_pos.x, camera_pos.y, camera_pos.z);  
	
	// light 1
	glUniform3f(lightA_location, light_ambient.x, light_ambient.y, light_ambient.z);
	glUniform3f(lightD_location, light_diffuse.x, light_diffuse.y, light_diffuse.z);
	glUniform3f(lightS_location, light_specular.x, light_specular.y, light_specular.z);
	glUniform3f(lightP_location, light_pos.x, light_pos.y, light_pos.z);
	
	// light 2
	glUniform3f(lightA_location_2, light_ambient.x, light_ambient.y, light_ambient.z);
	glUniform3f(lightD_location_2, light_diffuse.x, light_diffuse.y, light_diffuse.z);
	glUniform3f(lightS_location_2, light_specular.x, light_specular.y, light_specular.z);
	glUniform3f(lightP_location_2, light_pos.x, -light_pos.y, light_pos.z);

	// view_matrix

	
	view = glm::lookAt(                 camera_pos,  // pos
							glm::vec3(0.0f, 0.0f, 0.0f),  // target
							glm::vec3(0.0f, 1.0f, 0.0f)); // up

	glUniformMatrix4fv(view_location, 1, GL_FALSE, glm::value_ptr(view));
	// cube1
	model = glm::mat4(1.f);
	model = glm::translate(model, glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::rotate(model,
						  glm::radians((float)currentTime * 45.0f),
						  glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::rotate(model,
						  glm::radians((float)currentTime * 81.0f),
						  glm::vec3(1.0f, 0.0f, 0.0f));
	glUniformMatrix4fv(model_location, 1, GL_FALSE, glm::value_ptr(model));
	//proj_matrix
	projection = glm::perspective(glm::radians(50.0f),
								 (float) gl_width / (float) gl_height,
								 0.1f, 1000.0f);
	glUniformMatrix4fv(proj_location, 1, GL_FALSE, glm::value_ptr(projection));


	normal_to_world = glm::inverseTranspose(glm::mat3(model));
	glUniformMatrix3fv(normal_to_world_location, 1, GL_FALSE, glm::value_ptr(normal_to_world)); 
	
	// Activate textures
	glActiveTexture(GL_TEXTURE0); // Texture 1
	glBindTexture(GL_TEXTURE_2D, diffuseMap);
	
	glActiveTexture(GL_TEXTURE1); // Texture 2 
	glBindTexture(GL_TEXTURE_2D, specularMap);

	//  Draw cube 1
	glDrawArrays(GL_TRIANGLES, 0, 36);
	
	//cube2 
	cube_2 = glm::mat4(1.f);
	cube_2 = glm::translate(cube_2, glm::vec3(-1.0f, 0.0f, 0.0f));
	cube_2 = glm::translate(cube_2, glm::vec3(0.0f, 0.0f, 0.0f));
	cube_2 = glm::translate(cube_2,
		                     glm::vec3(sinf(2.1f * f) * 0.5f,
		                               cosf(1.7f * f) * 0.5f,
		                               sinf(1.3f * f) * cosf(1.5f * f) * 2.0f));
	cube_2 = glm::scale(cube_2, glm::vec3(0.5f));;
	cube_2 = glm::rotate(cube_2,
						  glm::radians((float)currentTime * 45.0f),
						  glm::vec3(0.0f, 1.0f, 0.0f));
	cube_2 = glm::rotate(cube_2,
						  glm::radians((float)currentTime * 81.0f),
						  glm::vec3(1.0f, 0.0f, 0.0f));
	glUniformMatrix4fv(model_location, 1, GL_FALSE, glm::value_ptr(cube_2));
	
	normal_to_world = glm::inverseTranspose(glm::mat3(cube_2));
	glUniformMatrix3fv(normal_to_world_location, 1, GL_FALSE, glm::value_ptr(normal_to_world)); 

	glDrawArrays(GL_TRIANGLES, 0, 36);
	
}


void processInput(GLFWwindow *window) {
	if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	glfwSetWindowShouldClose(window, 1);
}

	// Callback function to track window size and update viewport
void glfw_window_size_callback(GLFWwindow* window, int width, int height) {
	gl_width = width;
	gl_height = height;
	printf("New viewport: (width: %d, height: %d)\n", width, height);
}

unsigned int loadTexture(char const * path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    
    int width, height, nrComponents;
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        printf("Texture failed to load at path:  ");
        stbi_image_free(data);
    }

    return textureID;
}
