#include <stdlib.h>
#include <stdio.h>
#include <GL/glew.h>
#include <SDL2/SDL.h>

/* headers */

GLuint compileProgram(GLuint fragmentShaderId, GLuint vertexShaderId);
void checkProgram(GLuint programId);
GLuint loadShader(GLenum shaderType, const GLchar* source);
void checkShader(GLuint shaderId);

/* shaders */

const GLchar* fragmentShaderSource = 
	"#version 130\n"
	"out vec4 outColor;"
	"uniform vec4 color;"
	"void main()"
	"{"
		"outColor = color;"
	"}";
	
const GLchar* vertexShaderSource = 
	"#version 130\n"
	"in vec2 position;"
	"void main()"
	"{"
		"gl_Position = vec4(position, 0.0, 1.0);"
	"}";
	
/* vertices */

const GLfloat redSquareVertices[] = {
	-0.5f, -0.5f,
	 0.25f,-0.5f,
	 0.25f, 0.25f,
	-0.5f,  0.25f
};

const GLfloat blueSquareVertices[] = {
	-0.25f,-0.25f,
	 0.5f, -0.25f,
	 0.5f,  0.5f,
	-0.25f, 0.5f
};

int main(int argc, char* argv[])
{
	SDL_Init(SDL_INIT_VIDEO);
	
	SDL_Event e;
	
	SDL_Window* window;
	SDL_Renderer* renderer;
	
	SDL_CreateWindowAndRenderer(512, 512, SDL_WINDOW_OPENGL, &window, &renderer);
	
	glewInit();
		
	GLuint fragmentShaderId = loadShader(GL_FRAGMENT_SHADER, fragmentShaderSource);
	GLuint vertexShaderId = loadShader(GL_VERTEX_SHADER, vertexShaderSource);
	GLuint programId = compileProgram(fragmentShaderId, vertexShaderId);
	
	GLint positionAttribLocation = glGetAttribLocation(programId, "position");
	if (positionAttribLocation == -1)
		fprintf(stderr, "warning: positionAttribLocation invalid\n");
		
	GLint colorUniformLocation = glGetUniformLocation(programId, "color");
	if (colorUniformLocation == -1)
		fprintf(stderr, "warning: colorUniformLocation invalid\n");
	
	
	glViewport(0, 0, 512, 512);
	
	int keepRunning = 1;
	while (keepRunning)
	{
		SDL_PollEvent(&e);
		if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE)
			keepRunning = 0;
		
		glClear(GL_COLOR_BUFFER_BIT);
		
		glUseProgram(programId);
		
		glEnableVertexAttribArray(positionAttribLocation);
		
		glUniform4f(colorUniformLocation, 1.0f, 0.0f, 0.0f, 1.0f);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, redSquareVertices);
		glDrawArrays(GL_QUADS, 0, 4);
		
		glUniform4f(colorUniformLocation, 0.0f, 0.0f, 1.0f, 1.0f);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, blueSquareVertices);
		glDrawArrays(GL_QUADS, 0, 4);
		
		glDisableVertexAttribArray(positionAttribLocation);
		
		glUseProgram(0);
		
		glFlush();
		SDL_RenderPresent(renderer);
	}
	
	SDL_Quit();
	return 0;
}

GLuint compileProgram(GLuint fragmentShaderId, GLuint vertexShaderId)
{
	GLuint programId = glCreateProgram();
	glAttachShader(programId, vertexShaderId);
	glAttachShader(programId, fragmentShaderId);
	glLinkProgram(programId);
	checkProgram(programId);
	return programId;
}

void checkProgram(GLuint programId)
{
	GLint result = GL_FALSE;
	glGetProgramiv(programId, GL_LINK_STATUS, &result);

	if (!result)
	{
	    GLint infoLogLength;
	    glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &infoLogLength);
	    GLchar* message = alloca(infoLogLength * sizeof(GLchar));
		glGetProgramInfoLog(programId, infoLogLength, NULL, message);
		fprintf(stderr, "Warning: %s\n", message);
	}
}

GLuint loadShader(GLenum shaderType, const GLchar* source)
{
	GLuint shaderId = glCreateShader(shaderType);
	glShaderSource(shaderId, 1, &source, NULL);
	glCompileShader(shaderId);
	checkShader(shaderId);
	return shaderId;
}

void checkShader(GLuint shaderId)
{
	GLint result = GL_FALSE;
	glGetShaderiv(shaderId, GL_COMPILE_STATUS, &result);

	if (!result)
	{
	    GLint infoLogLength;
	    glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &infoLogLength);
		GLchar* message = alloca(infoLogLength * sizeof(GLchar));
		glGetShaderInfoLog(shaderId, infoLogLength, NULL, message);
		fprintf(stderr, "Warning: %s\n", message);
	}
}


