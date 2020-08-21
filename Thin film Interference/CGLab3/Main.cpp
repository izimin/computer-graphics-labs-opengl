#include <iostream>
#include <Windows.h>
#include <math.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "matrix.h"
#include "v3.h"

#include "stb_image.h"
#include <string>

using namespace std;

// Размерность сетки
#define m 100

const int countTextures = 2;

// Окно
GLFWwindow *g_window;

// Шейдерная программа
GLuint g_shaderProgram;

// Модель
class Model
{
public:
	GLuint vbo;
	GLuint ibo;
	GLuint vao;
	GLsizei indexCount;
} g_model;

#pragma region Идентификаторы юниформов
GLint g_uMVP;
GLint g_uMV;
GLint g_uN;
GLint g_uL;
GLint g_uE;
GLint g_uCoef;

GLuint samplerLocation;
#pragma endregion

// Массив индексов (имен) текстур
GLuint texIDs[countTextures + 1];

void  Create1DTexture()
{
	GLint texW, texH, numComponents;

	// Берем указатель на данные изображения в памяти
	GLvoid *imageData = stbi_load("fringe.png", &texW, &texH, &numComponents, 4);

	// Привязка текстуры к цели текстурирования
	glBindTexture(GL_TEXTURE_1D, texIDs[0]);

	// Задаем изображение текстуры
	glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA, texW, 0, GL_BGRA, GL_UNSIGNED_BYTE, imageData);

	stbi_image_free(imageData);

	// Задаем параметры текстуры
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}


void initTextures()
{
	// Генерируем id для текстур и сохраняем их в массив
	glGenTextures(1, texIDs);
	Create1DTexture();
	samplerLocation = glGetUniformLocation(g_shaderProgram, "u_colorMap");
}


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
		"layout(location = 1) in vec2 a_texCoord;"
		""
		"uniform mat4 u_mv;"
		"uniform mat4 u_mvp;"
		"uniform mat3 u_n;"
		""
		"out vec3 v_p;"
		"out vec3 v_normal;"
		"out vec2 v_texCoord;"
		""
		"void main()"
		"{"
		"   v_texCoord = a_texCoord;"
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
		"in vec2 v_texCoord;"
		""
		"layout(location = 0) out vec4 o_color;"
		""
		"uniform vec3 u_lPos;"
		"uniform vec3 u_ePos;"
		"uniform float u_coef;"
		""
		"uniform sampler1D u_colorMap;"
		""
		"void main()"
		"{"
		"	const float filmDepth = 0.4; "
		"	vec3 n = normalize(v_normal);"
		"	vec3 l = normalize(v_p-u_lPos);"
		"	float cosa = dot(-l, n);"
		"	float d = max(cosa, 0.1);"
		"	vec3 r = reflect(l, n);"
		"	vec3 e = normalize(u_ePos-v_p);"
		"	float b = max(dot(r, e), 0.0); "
		"	float s = pow(b, 5.0) * int(cosa >= 0);"

		"	vec2 viewDepth = vec2(0.5 / dot(n, e));"
		"	o_color =  vec4(d*vec3(0.0, 1.0, 0.0) + s*texture(u_colorMap, viewDepth.x).rgb, 1.0);"
		"}"
		;
	/*
		P – точка поверхности в пространстве.
		L – точка, характеризующая положение источника света в пространстве.
		E – точка, характеризующая положение наблюдателя в пространстве.
		n – вектор внешней нормали к поверхности освещаемого объекта в точке P.
		d – настроечный коэффициент, определяющий минимально допустимый уровень освещённости объекта в точке P.
		s – настроечный коэффициент, определяющий сфокусированность зеркального блика на поверхности освещаемогообъекта в точке P.
		cbase – базовый цвет поверхности освещаемого объекта в точке
		clight – цвет источника света.
	*/

	GLuint vertexShader, fragmentShader;

	vertexShader = createShader(vsh, GL_VERTEX_SHADER);
	fragmentShader = createShader(fsh, GL_FRAGMENT_SHADER);

	g_shaderProgram = createProgram(vertexShader, fragmentShader);

	// Запрашиваем соответствующий идентификатор по имени юниформа
	g_uMVP = glGetUniformLocation(g_shaderProgram, "u_mvp");
	g_uMV = glGetUniformLocation(g_shaderProgram, "u_mv");
	g_uN = glGetUniformLocation(g_shaderProgram, "u_n");
	g_uL = glGetUniformLocation(g_shaderProgram, "u_lPos");
	g_uE = glGetUniformLocation(g_shaderProgram, "u_ePos");
	g_uCoef = glGetUniformLocation(g_shaderProgram, "u_ccoef");
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	initTextures();

	return g_shaderProgram != 0;
}

bool createModel()
{
	GLfloat vertices[(m + 1)*(m + 1) * 4];

	int k = 0;
	for (int i = 0; i <= m; i++)
	{
		for (int j = 0; j <= m; j += 1)
		{
			vertices[k++] = GLfloat(j) / m;
			vertices[k++] = GLfloat(i) / m;
			vertices[k++] = GLfloat(j) / m;
			vertices[k++] = GLfloat(i) / m;
		}
	}
	GLuint indices[m*m * 6];
	k = 0;
	for (int i = 0; i < m; i++)
	{
		for (int j = 0; j < m; j++)
		{
			indices[k++] = (m + 1)*i + j;
			indices[k++] = (m + 1)*i + j + 1;
			indices[k++] = (m + 1)*i + j + m + 2;
			indices[k++] = (m + 1)*i + j + m + 2;
			indices[k++] = (m + 1)*i + j + m + 1;
			indices[k++] = (m + 1)*i + j;
		}
	}

	glGenVertexArrays(1, &g_model.vao);
	glBindVertexArray(g_model.vao);

	glGenBuffers(1, &g_model.vbo);
	glBindBuffer(GL_ARRAY_BUFFER, g_model.vbo);
	glBufferData(GL_ARRAY_BUFFER, (m + 1)*(m + 1) * 4 * sizeof(GLfloat), vertices, GL_STATIC_DRAW);

	glGenBuffers(1, &g_model.ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_model.ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m*m * 6 * sizeof(GLuint), indices, GL_STATIC_DRAW);

	g_model.indexCount = m * m * 6;

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (const GLvoid *)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (const GLvoid *)(2 * sizeof(GLfloat)));

	return g_model.vbo != 0 && g_model.ibo != 0 && g_model.vao != 0;
}

bool init()
{
	// Set initial color of color buffer to white.
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	//Включение теста глубины. При выключеном куб выворачивается наизнанку
	glEnable(GL_DEPTH_TEST);

	// Разрешает наложение текстуры
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_TEXTURE_1D);

	return createShaderProgram() && createModel();
}

void reshape(GLFWwindow *window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void draw(float ax, float ay, float az, float ey, float ez, float ly, float lz, float coef)
{
	GLint w, h = 0;
	glfwGetWindowSize(g_window, &w, &h);

	// очистка буфера цвета и очистка буфера глубины
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Включаем шейдерную программу
	glUseProgram(g_shaderProgram);

	// Биндим модель
	glBindVertexArray(g_model.vao);

	matrix model = matrix::getModelMatrix();

	matrix view = matrix::getViewMatrix(
		v3(3, ey, ez),				// Точка E, определяющая позицию наблюдателя на сцене.
		v3(0, 0, 0),				// Точка C, определяющая центр видимой области.
		v3(0, 1, 0)					// Вектор u, определяющий направление «вверх» для наблюдателя.
	);

	matrix projection = matrix::getPperspMatrix(
		0.1,						// Ближняя плоскость отсечения. Должна быть больше 0.
		100.,						// Дальняя плоскость отсечения.  
		w, h,						// Ширина и высота окна
		45.							// Угол обзора
	);

	model = matrix::scale(model, v3(2, 2, 2));
	model = matrix::translate(model, v3(-0.5, 0, 0));
	model = matrix::rotate(model, ax, v3(1, 0, 0));
	model = matrix::rotate(model, ay + 45, v3(0, 1, 0));
	model = matrix::rotate(model, az, v3(0, 0, 1));

	matrix mv = view * model;
	matrix mvp = projection * mv;

	GLfloat n[9];
	memcpy(n, matrix::getInvMatrixNotTranspose(mv), 9 * sizeof(GLfloat));

	glUniformMatrix4fv(g_uMVP, 1, GL_FALSE, mvp.get_value());
	glUniformMatrix4fv(g_uMV, 1, GL_FALSE, mv.get_value());
	glUniformMatrix3fv(g_uN, 1, GL_FALSE, n);

	// Положение источника света
	GLfloat  l[3]{ 0, ly, lz };

	// Позиция наблюдателя 
	GLfloat  e[3]{ 10, 5, 5 };

	glUniform3fv(g_uL, 1, l);
	glUniform3fv(g_uE, 1, e);
	glUniform1f(g_uCoef, coef);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_1D, texIDs[0]);
	glUniform1i(samplerLocation, 0);

	// Запускаем отрисовку и отображаем на экран 
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

	glDeleteTextures(1, texIDs);
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
	// Initialize OpenGL
	if (!initOpenGL())
		return -1;

	// Initialize graphical resources.
	bool isOk = init();

	if (isOk)
	{
		// Main loop until window closed or escape pressed.
		int ax = 0, ay = 0, az = 0, ez = 1, ey = 1, ly = 0, lz = 0;
		float coef = 5;
		while (glfwGetKey(g_window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(g_window) == 0)
		{
#pragma region обработка кнопок
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
			if (glfwGetKey(g_window, GLFW_KEY_KP_ADD) == GLFW_PRESS) {
				coef += 0.5;
			}
			if (glfwGetKey(g_window, GLFW_KEY_KP_SUBTRACT) == GLFW_PRESS) {
				coef -= 0.5;
			}
#pragma endregion

			// Draw scene.

			draw(ax, ay, az, ey, ez, ly, lz, coef);

			// Swap buffers.
			glfwSwapBuffers(g_window);
			// Poll window events.
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
