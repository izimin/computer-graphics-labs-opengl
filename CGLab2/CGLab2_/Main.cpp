#pragma comment(lib, "opengl32.lib")

#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <dos.h>
#include "matrix.h"
#include "v3.h"
#include <Windows.h>

#define _USE_MATH_DEFINES
#include <math.h>


#define M 100

using namespace std;
GLFWwindow *g_window;

GLuint g_shaderProgram;
GLint g_uMVP;
GLint g_uMV;
GLint g_uN;
GLint g_uL;


class Model
{
public:
	GLuint vbo;
	GLuint ibo;
	GLuint vao;
	GLsizei indexCount;
};

Model g_model;

GLuint createShader(const GLchar *code, GLenum type)
{
	GLuint result = glCreateShader(type);

	glShaderSource(result, 1, &code, NULL);
	glCompileShader(result);

	GLint compiled;
	glGetShaderiv(result, GL_COMPILE_STATUS, &compiled);

	if (!compiled)
	{
		GLint infoLen = 0;
		glGetShaderiv(result, GL_INFO_LOG_LENGTH, &infoLen);
		if (infoLen > 0)
		{
			char *infoLog = new char[infoLen];
			glGetShaderInfoLog(result, infoLen, NULL, infoLog);
			cout << "Shader compilation error" << endl << infoLog << endl;
		}
		glDeleteShader(result);
		return 0;
	}

	return result;
}

GLuint createProgram(GLuint vsh, GLuint fsh)
{
	GLuint result = glCreateProgram();

	glAttachShader(result, vsh);
	glAttachShader(result, fsh);

	glLinkProgram(result);

	GLint linked;
	glGetProgramiv(result, GL_LINK_STATUS, &linked);

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

	return result;
}

bool createShaderProgram()
{
	g_shaderProgram = 0;

	const GLchar vsh[] =
		"#version 330\n"
		""
		"layout(location = 0) in vec2 a_position;"			
		"" 
		"uniform mat4 u_mv;"
		"uniform mat4 u_mvp;"
		"uniform mat3 u_n;"
		""	
		"out vec3 v_p;"
		"out vec3 v_normal;"
		""
		"void main()"
		"{"
		"	vec4 pos = vec4(a_position[0], sin(a_position[0]*10)*sin(a_position[1]*10)/5, a_position[1], 1);"
		"   gl_Position = u_mvp * pos;"
		"	v_p = (u_mv * pos).xyz;"
		"	v_normal = normalize(u_n*vec3(-cos(10*a_position[0])*sin(10*a_position[1]), 1.0, -cos(10*a_position[1])*sin(10*a_position[0])));"
		"}"
		;

	const GLchar fsh[] =
		"#version 330\n"
		""
		"in vec3 v_p;"
		"in vec3 v_normal;"
		""
		"layout(location = 0) out vec4 o_color;"
		""
		"uniform vec3 u_l;"
		""
		"void main()"
		"{"
		"	vec3 n = normalize(v_normal);"
		"	vec3 l = normalize(v_p-u_l);"
		"	float cosa = dot(-l, n);"
		"	float d = max(cosa, 0.1);"
		"	vec3 r = reflect(l, n);" 				 
		"	vec3 e = normalize(-v_p);"
		"	float s = pow(max(dot(r, e), 0.0), 5.0) * int(cosa >= 0);"
		"	o_color = vec4(d*vec3(1.0, 0.0, 0.0) + s * vec3(1.0), 1.0);"
		"}"
		;

	/*
		P Ц точка поверхности в пространстве.
		L Ц точка, характеризующа€ положение источника света в пространстве.
		E Ц точка, характеризующа€ положение наблюдател€ в пространстве.
		n Ц вектор внешней нормали к поверхности освещаемого объекта в точке P.
		d Ц настроечный коэффициент, определ€ющий минимально допустимый уровень освещЄнности объекта в точке P.
		s Ц настроечный коэффициент, определ€ющий сфокусированность зеркального блика на поверхности освещаемогообъекта в точке P.
		cbase Ц базовый цвет поверхности освещаемого объекта в точке
		clight Ц цвет источника света.
	*/

	GLuint vertexShader, fragmentShader;

	vertexShader = createShader(vsh, GL_VERTEX_SHADER);
	fragmentShader = createShader(fsh, GL_FRAGMENT_SHADER);

	g_shaderProgram = createProgram(vertexShader, fragmentShader);

	// «апрашиваем соответствующий идентификатор по имени юниформа
	g_uMVP = glGetUniformLocation(g_shaderProgram, "u_mvp");
	g_uMV = glGetUniformLocation(g_shaderProgram, "u_mv");
	g_uN = glGetUniformLocation(g_shaderProgram, "u_n");
	g_uL = glGetUniformLocation(g_shaderProgram, "u_l");


	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	return g_shaderProgram != 0;
}

bool createModel()
{
	GLfloat vertices[(M+1)*(M+1)*2];

	int k = 0;
	for (int i = 0; i <= M; i++)
	{
		for (int j = 0; j <= M; j += 1)
		{
			vertices[k++] = float(j) / M;
			vertices[k++] = float(i) / M;
		}
	}

	GLuint indices[M*M*6];
	k = 0;
	for (int i = 0; i < M; i++)
	{
		for (int j = 0; j < M; j++)
		{
			indices[k++] = (M + 1)*i + j;
			indices[k++] = (M + 1)*i + j + 1;
			indices[k++] = (M + 1)*i + j + M + 2;
			indices[k++] = (M + 1)*i + j + M + 2;
			indices[k++] = (M + 1)*i + j + M + 1;
			indices[k++] = (M + 1)*i + j;
		}
	}

	glGenVertexArrays(1, &g_model.vao);
	glBindVertexArray(g_model.vao);

	glGenBuffers(1, &g_model.vbo);
	glBindBuffer(GL_ARRAY_BUFFER, g_model.vbo);
	glBufferData(GL_ARRAY_BUFFER, (M+1)*(M+1) * 2 * sizeof(GLfloat), vertices, GL_STATIC_DRAW);

	glGenBuffers(1, &g_model.ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_model.ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, M*M * 6 * sizeof(GLuint), indices, GL_STATIC_DRAW);

	g_model.indexCount = M*M * 6;

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (const GLvoid *)0);

	return g_model.vbo != 0 && g_model.ibo != 0 && g_model.vao != 0;
}

bool init()
{
	// Set initial color of color buffer to white.
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

	//¬ключение теста глубины. ѕри выключеном куб выворачиваетс€ наизнанку
	glEnable(GL_DEPTH_TEST);

	return createShaderProgram() && createModel();
}

//  олбэк на изменение размера окна
void reshape(GLFWwindow *window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void draw(float ax, float ay, float az, float ey, float ez, float ly, float lz)
{
	// очистка буфера цвета и очистка буфера глубины
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// ¬ключаем шейдерную программу
	glUseProgram(g_shaderProgram);
	
	// Ѕиндим модель
	glBindVertexArray(g_model.vao);

	matrix m = matrix::getModelMatrix();
	matrix v = matrix::getViewMatrix(
		v3(3, ey, ez),				// “очка E, определ€юща€ позицию наблюдател€ на сцене.
		v3(0, 0, 0),				// “очка C, определ€юща€ центр видимой области.
		v3(0, 1, 0)					// ¬ектор u, определ€ющий направление Ђвверхї дл€ наблюдател€.
	);
	
	int w, h = 0;
	glfwGetWindowSize(g_window, &w, &h);
	
	matrix p = matrix::getPperspMatrix(
		0.1,						// Ѕлижн€€ плоскость отсечени€. ƒолжна быть больше 0.
		100.,						// ƒальн€€ плоскость отсечени€.  
		w, h,						// Ўирина и высота окна
		45.							// ”гол обзора
	);		

	// ¬се изменени€ производ€тс€ с матрицей M
	m = matrix::scale(m, v3(2, 2, 2));
	m = matrix::translate(m, v3(-0.5, 0, 0));
	m = matrix::rotate(m, ax, v3(1, 0, 0));
	m = matrix::rotate(m, ay+45, v3(0, 1, 0));
	m = matrix::rotate(m, az, v3(0, 0, 1));

	matrix mv = v * m;
	matrix mvp = p * mv;

	// ћатрица нормалей
	float n[9];
	memcpy(n, matrix::getInvMatrixNotTranspose(mv), 9 * sizeof(float));

	// ќтправл€ем матрицу в пам€ть.  g_uMVP - идентификатор юниформа 
	// 1 - кол-во матриц
	// GL_FALSE - матрица не транспонирована
	glUniformMatrix4fv(g_uMVP, 1, GL_FALSE, mvp.get_value()); 
	glUniformMatrix4fv(g_uMV, 1, GL_FALSE, mv.get_value()); 
	glUniformMatrix3fv(g_uN, 1, GL_FALSE, n);
	
	float l[3] { 0, ly, lz };
	glUniform3fv(g_uL, 1, l);

	// «апускаем отрисовку и отображаем на экран 
	glDrawElements(GL_TRIANGLES, g_model.indexCount, GL_UNSIGNED_INT, NULL);
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
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Create window.
	g_window = glfwCreateWindow(800, 600, "OpenGL Test", NULL, NULL);
	if (g_window == NULL)
	{
		cout << "Failed to open GLFW window" << endl;
		glfwTerminate();
		return false;
	}

	// Initialize OpenGL context with.
	glfwMakeContextCurrent(g_window);

	// Set internal GLEW variable to activate OpenGL core profile.
	glewExperimental = true;

	// Initialize GLEW functions.
	if (glewInit() != GLEW_OK)
	{
		cout << "Failed to initialize GLEW" << endl;
		return false;
	}

	// Ensure we can capture the escape key being pressed.
	glfwSetInputMode(g_window, GLFW_STICKY_KEYS, GL_TRUE);

	// Set callback for framebuffer resizing event.
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
	if (!initOpenGL())
		return -1;

	bool isOk = init();

	if (isOk)
	{
		int ax = 0, ay = 0, az = 0, ez = 1, ey = 1, ly = 0, lz = 0;
		while (glfwGetKey(g_window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(g_window) == 0)
		{
			if (glfwGetKey(g_window, GLFW_KEY_UP) == GLFW_PRESS) {
				ax--;
			}
			if (glfwGetKey(g_window, GLFW_KEY_DOWN) == GLFW_PRESS) {
				ax++;
			}
			if (glfwGetKey(g_window, GLFW_KEY_LEFT) == GLFW_PRESS) {
				ay++;
			}
			if (glfwGetKey(g_window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
				ay--;
			}
			if (glfwGetKey(g_window, GLFW_KEY_KP_8) == GLFW_PRESS) {
				ey++;
			}
			if (glfwGetKey(g_window, GLFW_KEY_KP_2) == GLFW_PRESS) {
				ey--;
			}
			if (glfwGetKey(g_window, GLFW_KEY_KP_6) == GLFW_PRESS) {
				ez++;
			}
			if (glfwGetKey(g_window, GLFW_KEY_KP_4) == GLFW_PRESS) {
				ez--;
			}
			if (glfwGetKey(g_window, GLFW_KEY_W) == GLFW_PRESS) {
				ly++;
			}
			if (glfwGetKey(g_window, GLFW_KEY_S) == GLFW_PRESS) {
				ly--;
			}
			if (glfwGetKey(g_window, GLFW_KEY_D) == GLFW_PRESS) {
				lz++;
			}
			if (glfwGetKey(g_window, GLFW_KEY_A) == GLFW_PRESS) {
				lz--;
			}

			draw(ax, ay, az, ey, ez, ly, lz);

			glfwSwapBuffers(g_window);

			glfwPollEvents();

			Sleep(20);
		}
	}

	// Cleanup graphical resources.
	cleanup();

	// Tear down OpenGL.
	tearDownOpenGL();

	return isOk ? 0 : -1;
}
