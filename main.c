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

/* 1st pass */

const GLchar* vertexShaderSource1 = 
	"#version 130\n"
	"in vec2 position;"
	"void main()"
	"{"
		"gl_Position = vec4(position, 0.0, 1.0);"
	"}";

const GLchar* fragmentShaderSource1 = 
	"#version 130\n"
	"out vec4 outColor;"
	"uniform vec4 color;"
	"void main()"
	"{"
		"outColor = color;"
	"}";
	
/* 2nd pass */
	
const GLchar* vertexShaderSource2 = 
	"#version 130\n"
	"in vec2 position;"
	"in vec2 texturePosition;"
	"out vec2 texturePosition2;"
	"void main()"
	"{"
		"gl_Position = vec4(position, 0.0, 1.0);"
		"texturePosition2 = texturePosition;"
	"}";

const GLchar* fragmentShaderSource2 = 
	"#version 130\n"
	"in vec2 texturePosition2;"
	"out vec4 outColor;"
	"uniform sampler2D renderTexture;"
	"void main()"
	"{"
		"ivec2 renderTextureSize = textureSize(renderTexture, 0);"
		"float dx = 1.0 / renderTextureSize.x;"
		"float dy = 1.0 / renderTextureSize.y;"
		"float x = texturePosition2.x;"
		"float y = texturePosition2.y;"
		"vec3 color = vec3(0.0, 0.0, 0.0);"
		"int blurRadius = 4;"
		"for (int i = -blurRadius; i <= blurRadius; i++)"
		"{"
			"for (int j = -blurRadius; j <= blurRadius; j++)"
			"{"
				"color += texture(renderTexture, vec2(x + i * dx, y + j * dy)).xyz;"
			"}"
		"}"
		"color /= (blurRadius * 2 - 1) * (blurRadius * 2 - 1);"
		"outColor = vec4(color, 1.0);"
	"}";
	
/* vertices */

/* 1st pass */

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

/* 2nd pass */

const GLfloat screenVertices[] = {
	-0.9f,-0.9f,
	 0.9f,-0.9f,
	 0.9f, 0.9f,
	-0.9f, 0.9f
};

const GLfloat screenTextureCoordinates[] = {
	0.0f, 0.0f,
	1.0f, 0.0f,
	1.0f, 1.0f,
	0.0f, 1.0f
};

/* window size */
#define WINDOW_WIDTH 512
#define WINDOW_HEIGHT 512

int main(int argc, char* argv[])
{
	SDL_Init(SDL_INIT_VIDEO);
	
	SDL_Event e;
	
	SDL_Window* window;
	SDL_Renderer* renderer;
	
	SDL_CreateWindowAndRenderer(WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_OPENGL, &window, &renderer);
	
	glewInit();
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
		
	/* load programs */
	
	/* 1st pass */
	GLuint fragmentShaderId1 = loadShader(GL_FRAGMENT_SHADER, fragmentShaderSource1);
	GLuint vertexShaderId1 = loadShader(GL_VERTEX_SHADER, vertexShaderSource1);
	GLuint programId1 = compileProgram(fragmentShaderId1, vertexShaderId1);
	
	/* 2nd pass */
	GLuint fragmentShaderId2 = loadShader(GL_FRAGMENT_SHADER, fragmentShaderSource2);
	GLuint vertexShaderId2 = loadShader(GL_VERTEX_SHADER, vertexShaderSource2);
	GLuint programId2 = compileProgram(fragmentShaderId2, vertexShaderId2);
	
	/* retrieve attribs and uniforms */
	
	/* 1st pass */
	GLint positionAttribLocation1 = glGetAttribLocation(programId1, "position");
	if (positionAttribLocation1 == -1)
		fprintf(stderr, "warning: positionAttribLocation1 invalid\n");
		
	GLint colorUniformLocation1 = glGetUniformLocation(programId1, "color");
	if (colorUniformLocation1 == -1)
		fprintf(stderr, "warning: colorUniformLocation1 invalid\n");
		
	/* 2nd pass */
	GLint positionAttribLocation2 = glGetAttribLocation(programId2, "position");
	if (positionAttribLocation2 == -1)
		fprintf(stderr, "warning: positionAttribLocation2 invalid\n");
		
	GLint texturePositionAttribLocation2 = glGetAttribLocation(programId2, "texturePosition");
	if (texturePositionAttribLocation2 == -1)
		fprintf(stderr, "warning: texturePositionAttribLocation2 invalid\n");
	
	GLint renderTextureUniformLocation2 = glGetUniformLocation(programId2, "renderTexture");
	if (renderTextureUniformLocation2 == -1)
		fprintf(stderr, "warning: renderTextureUniformLocation2 invalid\n");
		
	/* create FBO */
	GLuint fboId;
	glGenFramebuffers(1, &fboId);
	
	GLuint renderTextureId;
	glGenTextures(1, &renderTextureId);
	
    glBindTexture(GL_TEXTURE_2D, renderTextureId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, WINDOW_WIDTH, WINDOW_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glBindFramebuffer(GL_FRAMEBUFFER, fboId);
    glBindTexture(GL_TEXTURE_2D, renderTextureId);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderTextureId, 0);
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
	int keepRunning = 1;
	while (keepRunning)
	{
		SDL_PollEvent(&e);
		if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE)
			keepRunning = 0;
		
		/* 1st pass */
		glBindFramebuffer(GL_FRAMEBUFFER, fboId);
		glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
		GLenum bufferlist [] = {
			GL_COLOR_ATTACHMENT0
		};
		glDrawBuffers(1, bufferlist);
		glClearColor(0.0f, 1.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glUseProgram(programId1);
		
		glEnableVertexAttribArray(positionAttribLocation1);
		
		glUniform4f(colorUniformLocation1, 1.0f, 0.0f, 0.0f, 1.0f);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, redSquareVertices);
		glDrawArrays(GL_QUADS, 0, 4);
		
		glUniform4f(colorUniformLocation1, 0.0f, 0.0f, 1.0f, 1.0f);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, blueSquareVertices);
		glDrawArrays(GL_QUADS, 0, 4);
		
		glDisableVertexAttribArray(positionAttribLocation1);
		
		/* 2nd pass */
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glUseProgram(programId2);
		
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, renderTextureId);
		glUniform1i(renderTextureUniformLocation2, 0);
		
		glEnableVertexAttribArray(positionAttribLocation2);
		glEnableVertexAttribArray(texturePositionAttribLocation2);
		
		glVertexAttribPointer(positionAttribLocation2, 2, GL_FLOAT, GL_FALSE, 0, screenVertices);
		glVertexAttribPointer(texturePositionAttribLocation2, 2, GL_FLOAT, GL_FALSE, 0, screenTextureCoordinates);
		glDrawArrays(GL_QUADS, 0, 4);
		
		glDisableVertexAttribArray(positionAttribLocation2);
		glDisableVertexAttribArray(texturePositionAttribLocation2);
		
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


