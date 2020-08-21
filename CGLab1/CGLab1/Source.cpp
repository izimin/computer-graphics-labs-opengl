#pragma comment(lib, "opengl32.lib")

#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

using namespace std;

GLFWwindow *g_window;

// 0-� ���������� - ��������� ����������, � 1 � ���� - ��������
GLuint g_shaderProgram;

class Model
{
public:
	GLuint vbo;			// vertex buffer object. ��������� ������, ������� ������ �������� ������, ������ ������
	GLuint ibo;			// index buffer object
	GLuint vao;			// vertex array object. ������ ���������, ��������� � �������. ��������� ����, ��� ��� ������ ������������ �� ������� ����������,
						// ��� �������� ������ � ������ ������. ���������� ������ �������� ������� � ���� ������, ����� ��� ��������� ����� ��������� �����
	GLsizei indexCount;
};

Model g_model;


GLuint createShader(const GLchar *code, GLenum type)
{
	// ������ ������� ������������ ������ ������� � �������� �����, � �������� ����� 
	GLuint result = glCreateShader(type);

	// �������� � ������ ��� �������
	glShaderSource(result, 1, &code, NULL);

	// � ����������� ���
	glCompileShader(result);

	GLint compiled;
	// ������ ������ ���������� ������� � ���������� ��������� � compiled 
	glGetShaderiv(result, GL_COMPILE_STATUS, &compiled);

	// ���� �������� ������ ����������
	if (!compiled)
	{
		GLint infoLen = 0;			// ����� �����
		// ����������� ����� ���� �������� ������
		glGetShaderiv(result, GL_INFO_LOG_LENGTH, &infoLen);
		
		// ���� ������ �� ������� �����
		if (infoLen > 0)
		{
			// ���������� �� ����� 
			char *infoLog = (char *)alloca(infoLen);
			
			// �������� ��� ������ � �����
			glGetShaderInfoLog(result, infoLen, NULL, infoLog);

			// ������� ������ � �������
			cout << "Shader compilation error" << endl << infoLog << endl;
		}

		// �.�. ������, ������ ����� �������
		glDeleteShader(result);

		// ���������� 0 - ���������� ����������
		return 0;
	}

	//  ���� ��� �����, ���������� ���������� ���������� � �����������������
	return result;
}

// ������� ��������� ���������
GLuint createProgram(GLuint vsh, GLuint fsh)
{
	// �������� ���������� ���������
	GLuint result = glCreateProgram();

	// ���������� (�������) � ��� ��� �������
	glAttachShader(result, vsh);
	glAttachShader(result, fsh);

	// �������� �������� ��������� (���������� ��������� �������� � ���� ��������������� ���������)
	glLinkProgram(result);

	GLint linked;
	// ����������� ������ �������� (������� ����� �� ������� �� ������ � ������� � �� ����� ��������������� � ����� ���������� ������)
	glGetProgramiv(result, GL_LINK_STATUS, &linked);

	// ���� �������� �� �������, ������� ������
	if (!linked)
	{
		GLint infoLen = 0;
		glGetProgramiv(result, GL_INFO_LOG_LENGTH, &infoLen);
		if (infoLen > 0)
		{
			char *infoLog = (char *)alloca(infoLen);
			glGetProgramInfoLog(result, infoLen, NULL, infoLog);
			cout << "Shader program linking error" << endl << infoLog << endl;
		}
		glDeleteProgram(result);
		return 0;
	}

	// ���������� ���������� ������� � ������������� ���������
	return result;
}

bool createShaderProgram()
{
	g_shaderProgram = 0;

	// ���� �������
	const GLchar vsh[] =    // ��������� ������
		// 2 �������-����������.����������� ��� ������ ��������� ����� in, ���� ������� 
		// a_position - ����� ��� ����������������� float ������� (vec2)
		// a_color - ����� ��� ����������������� float ������� (vec3)
		// layout - ������ ��������� �������������� ��� ����� ���������. a_position � ������ �� ������� 0, a_color �� ������� 1. ����� �� �� ������, ����� �� ��������
		// ������������ ������� ��� ��������� �������, ������� ���� �� ����������������
		"#version 330\n"
		""
		"layout(location = 0) in vec2 a_position;"
		"layout(location = 1) in vec3 a_color;"
		""
		"out vec3 v_color;"
		""
		"void main()"
		"{"
		"    v_color = a_color;"
		"    gl_Position = vec4(a_position, 0.0, 1.0);"
		"}"
		;

	const GLchar fsh[] =
		"#version 330\n"
		""
		"in vec3 v_color;"
		""
		"layout(location = 0) out vec4 o_color;"
		""
		"void main()"
		"{"
//		"	float intensity = sin((sin(v_color[0]*9)/9-v_color[1])*3.145926*12);"
		"   float intensity = sin(length(vec2(v_color[0],v_color[1]))*3.1415926*14);"
		"   o_color =  vec4(intensity, intensity, intensity, 1.0);"
		"}"
		;

	// -- ���������� � ������������ �������� 
	GLuint vertexShader, fragmentShader;


	vertexShader = createShader(vsh, GL_VERTEX_SHADER);
	fragmentShader = createShader(fsh, GL_FRAGMENT_SHADER);

	// �������� ��������������� ��������� (� ������)
	g_shaderProgram = createProgram(vertexShader, fragmentShader);

	// ������, ����� ���� ������� ���������, ��������� ����� ��������� �������� ����� �������
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	// ���� ��� ��, ����������� ������
	return g_shaderProgram != 0;
}

bool createModel()
{
	const GLfloat vertices[] =
	{		
		-0.5f, -0.5f, -1.0f, -1.0f,  0.0f,	
		 0.5f, -0.5f,  1.0f, -1.0f,  0.0f,
		 0.5f,  0.5f,  1.0f,  1.0f,  0.0f,
		-0.5f,  0.5f, -1.0f,  1.0f,  0.0f
	};

	const GLuint indices[] =
	{
		0, 1, 2, 2, 3, 0
	};

	glGenVertexArrays(1, &g_model.vao);				// ���������� ������ � ��������� ������
	glBindVertexArray(g_model.vao);					// ���� ������ � ��� ����� �������� � ��� ���������� �������� ������ ����� ������ �� ���� �������� ������

	glGenBuffers(1, &g_model.vbo);					// ���������� ������ ������ 
	glBindBuffer(GL_ARRAY_BUFFER, g_model.vbo);		// ������ ��� �������� � �������� ��� ���� GL_ARRAY_BUFFER
	glBufferData(GL_ARRAY_BUFFER, 20 * sizeof(GLfloat), vertices, GL_STATIC_DRAW);		// � �������� ����� ����� ���� �������� ����� ������ 20*4 �� vertices
																						// GL_STATIC_DRAW - �� ������ �� ����������� ���������� ��� ������ � ������
																						// ���, ����� ������ � ��� ��� ������������� �� ������. �� ������� �������,
																						// ��� �� �� ����� ��� ������ ������
	// ����� ����� ������ ������������ ����� ������� vertices, ������ ��������� � �����������
	// ����� glBufferData - �������� ��������� !!! ���� ��� �������������� 

	// ���������� ��� ������� �������
	glGenBuffers(1, &g_model.ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_model.ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(GLuint), indices, GL_STATIC_DRAW);
	g_model.indexCount = 6;

	// �������� ������  � ������� �������� - a_position
	glEnableVertexAttribArray(0);

	// 0-� ������� ������� �� 2-� ����������� ���� float, ������� �� ������ ������������ ��� ���������� (GL_FALSE - �������� ��� ����������),
	// ������� �������� ����� � ����� 5 * sizeof(GLfloat) ������ 
	// ��������� �������� - �������� ������������ ������, �� �������� ����� �������� ������� ������ ������� (�������� ������� - �������� � ������ �������)
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (const GLvoid *)0);

	// ���������� ��� �������� - a_color
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (const GLvoid *)(2 * sizeof(GLfloat)));

	return g_model.vbo != 0 && g_model.ibo != 0 && g_model.vao != 0;
}

bool init()	
{
	// Set initial color of color buffer to white.
	// ��������� �����, ������� ����� ��������� �������� ����� (���� ����)
	// RGBA, �������� �� 0 �� 1 
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

	// createShaderProgram - �������� ��������� ���������
	return createShaderProgram() && createModel();
}

void reshape(GLFWwindow *window, int width, int height)
{
	// �������� ������� ������ ������ ��� �����, ����� ������ ��������� �������������� ���-���� ��������
	glViewport(0, 0, width, height);
}

void draw()
{
	// Clear color buffer. ������� ������ �����
	glClear(GL_COLOR_BUFFER_BIT);

	// ������������ �����-�� ��������� ���������
	glUseProgram(g_shaderProgram);
	
	// ������������ ��������� �� ������-�� ����� �������
	glBindVertexArray(g_model.vao);

	// ������ ������������, ����� ����� ���-�� �������� ��������, ����� ������ ����� �������, � ����� ����������	
	glDrawElements(GL_TRIANGLES, g_model.indexCount, GL_UNSIGNED_INT, (const GLvoid *) 0);
}

void cleanup()
{
	if (g_shaderProgram != 0)
		glDeleteProgram(g_shaderProgram);
	if (g_model.vbo != 0)
		glDeleteBuffers(1, &g_model.vbo);
	if (g_model.ibo != 0)
		glDeleteBuffers(1, &g_model.ibo);
	if (g_model.vao != 0)
		glDeleteVertexArrays(1, &g_model.vao);
}

bool initOpenGL()
{
	// Initialize GLFW functions.
	if (!glfwInit())
	{
		cout << "Failed to initialize GLFW" << endl;
		return false;
	}

	// Request OpenGL 3.3 without obsoleted functions.
	// 4 ������, ������� ����������� ����������� ������ ������������ ���������
	//                glfwWindowHint 
	// ������ ���������� ���������� �������� ������������� ���������, 
	//       ������� ������������ ���������
	// ������ ���������� ���������� ��������, ������� ��������������� 
	//       ���������������� ���������.

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);					// �������
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);					// �������
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);			// ���������� ����������� ��������� ������� ���� 
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // ��������� �������� ��� �������� ��������� ��������

	// Create window.
	// ������� ���� � ������������ ���������� (��������� - ������� ��������� ����������� �� ������)
	// ������� �����, ������� �����, ��������� �����, ...
	g_window = glfwCreateWindow(600, 600, "OpenGL Test", NULL, NULL);
	if (g_window == NULL)
	{
		cout << "Failed to open GLFW window" << endl;
		glfwTerminate();
		return false;
	}

	// Initialize OpenGL context with.
	// ���������� �������� OpenGL
	glfwMakeContextCurrent(g_window);

	// Set internal GLEW variable to activate OpenGL core profile.
	//��������� �������� glewExperimental � GL_TRUE ��������� GLEW ������������ 
	//	�������� ������� ��� ���������� ������������ OpenGL.�����, ���� 
	//	�������� ��� ���������� �� ��������� �� ���������, �� �����
	//	���������� �������� � �������������� Core - profile ������
	glewExperimental = true;

	// Initialize GLEW functions.
	// ������������� ���������� GLEW
	if (glewInit() != GLEW_OK)
	{
		cout << "Failed to initialize GLEW" << endl;
		return false;
	}

	// ���� ��� ���������, OPENGL ����� � ������

	// Ensure we can capture the escape key being pressed.
	// ��������� ������� �����, ��� ���� ����� ����������� ������� esc
	glfwSetInputMode(g_window, GLFW_STICKY_KEYS, GL_TRUE);

	// Set callback for framebuffer resizing event.
	// ���������� ������� ��������� ������ �� ������� ��������� ������� ����
	// g_window - ������� ������ ����,
	// reshape - ��������� �� �������
	glfwSetFramebufferSizeCallback(g_window, reshape);

	return true;
}

void tearDownOpenGL()
{
	// Terminate GLFW.
	glfwTerminate();
}

int main()
{
	// Initialize OpenGL
	if (!initOpenGL())
		return -1;

	// Initialize graphical resources.
	bool isOk = init();

	if (isOk)
	{
		// Main loop until window closed or escape pressed.
		while (glfwGetKey(g_window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(g_window) == 0)
		{
			// Draw scene.
			draw();

			// Swap buffers.
			glfwSwapBuffers(g_window);
			// Poll window events.
			glfwPollEvents();
		}
	}

	// Cleanup graphical resources.
	cleanup();

	// Tear down OpenGL.
	tearDownOpenGL();

	return isOk ? 0 : -1;
} 