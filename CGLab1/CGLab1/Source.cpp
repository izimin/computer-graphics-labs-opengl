#pragma comment(lib, "opengl32.lib")

#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

using namespace std;

GLFWwindow *g_window;

// 0-й дескриптор - невалиднй дескриптор, а 1 и выше - валидный
GLuint g_shaderProgram;

class Model
{
public:
	GLuint vbo;			// vertex buffer object. Структура данных, которая хранит атрибуты вершин, подряд идущие
	GLuint ibo;			// index buffer object
	GLuint vao;			// vertex array object. Хранит настройки, связанные с моделью. Настройки того, как эту модель обрабатывать на стороне видиокарты,
						// Как получать доступ к данным модели. Сохрасняет массив настроек доступа к этой модели, чтобы все настройки потом применить разом
	GLsizei indexCount;
};

Model g_model;


GLuint createShader(const GLchar *code, GLenum type)
{
	// Просим систему аллоцировать объект шейдера с заданной ролью, с заданным типом 
	GLuint result = glCreateShader(type);

	// Заливаем в объект код шейдера
	glShaderSource(result, 1, &code, NULL);

	// И компилируем его
	glCompileShader(result);

	GLint compiled;
	// Узнаем статус компиляции объекта и записываем результат в compiled 
	glGetShaderiv(result, GL_COMPILE_STATUS, &compiled);

	// Если возникли ошибки компиляции
	if (!compiled)
	{
		GLint infoLen = 0;			// Дляна лонга
		// Запрашиваем длину лога описания ошибки
		glGetShaderiv(result, GL_INFO_LOG_LENGTH, &infoLen);
		
		// Если ошибка не нулевой длины
		if (infoLen > 0)
		{
			// Аллоцируем на стеке 
			char *infoLog = (char *)alloca(infoLen);
			
			// Копируем лог ощибки в буфер
			glGetShaderInfoLog(result, infoLen, NULL, infoLog);

			// Выводим ошибку в консоль
			cout << "Shader compilation error" << endl << infoLog << endl;
		}

		// Т.к. ошибка, объект можно удалять
		glDeleteShader(result);

		// Возвращаем 0 - невалидный дескриптор
		return 0;
	}

	//  Если все четко, возвращаем дескриптор созданного и скомпилированного
	return result;
}

// Создает шейдерную программу
GLuint createProgram(GLuint vsh, GLuint fsh)
{
	// получаем дескриптор программы
	GLuint result = glCreateProgram();

	// Подцепляем (аттачим) к ней два шейдера
	glAttachShader(result, vsh);
	glAttachShader(result, fsh);

	// Вызываем линковку программы (объединяет несколько шейдеров в одну работоспособную программу)
	glLinkProgram(result);

	GLint linked;
	// запрашиваем статус линковки (шейдеры могли не сойтись по входам и выходам и не могут функционировать в одном контейнере вместе)
	glGetProgramiv(result, GL_LINK_STATUS, &linked);

	// Если линковка не удалась, выводим ошибку
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

	// Возвращаем дескриптор готовой к использованию программы
	return result;
}

bool createShaderProgram()
{
	g_shaderProgram = 0;

	// Коды шейдера
	const GLchar vsh[] =    // вершинный шейдер
		// 2 атрибут-переменные.Описываются при помощи ключевого слова in, типа входные 
		// a_position - имеет тип двухкомпонентного float вектора (vec2)
		// a_color - имеет тип трехкомпонентного float вектора (vec3)
		// layout - задаем некоторые идентификаторы для ныших атрибутов. a_position в памяти по индексу 0, a_color по индексу 1. Могли бы не писать, тогда бы пришлось
		// использовать функцию для получения индекса, система сама бы проиндексирвоала
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

	// -- вершинного и фрагментного шейдеров 
	GLuint vertexShader, fragmentShader;


	vertexShader = createShader(vsh, GL_VERTEX_SHADER);
	fragmentShader = createShader(fsh, GL_FRAGMENT_SHADER);

	// Получаем работоспособную программу (в идеале)
	g_shaderProgram = createProgram(vertexShader, fragmentShader);

	// Теперь, когда есть готовая программа, объектные файлы отдельных шейдеров можно удалить
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	// если все ок, возвращется истина
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

	glGenVertexArrays(1, &g_model.vao);				// Аллоцируем объект в системной памяти
	glBindVertexArray(g_model.vao);					// Этот объект у нас будет активным и все дальнейшие действия неявно будут влиять на этот активный объект

	glGenBuffers(1, &g_model.vbo);					// Генерируем массив вершин 
	glBindBuffer(GL_ARRAY_BUFFER, g_model.vbo);		// Делаем его активным и прияваем ему роль GL_ARRAY_BUFFER
	glBufferData(GL_ARRAY_BUFFER, 20 * sizeof(GLfloat), vertices, GL_STATIC_DRAW);		// В активный буфер такой роли заливаем кусок памяти 20*4 из vertices
																						// GL_STATIC_DRAW - мы просим по возможности разместить эти данные в память
																						// так, чтобы доступ к ним был оптимизирован на чтение. Мы обещаем системе,
																						// что мы не будем эти данные менять
	// После этого вызова теоретически можно удалять vertices, данные загружены в видеопамять
	// вызов glBufferData - давольно медленный !!! надо его минимизировать 

	// Аналогично для другого массива
	glGenBuffers(1, &g_model.ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_model.ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(GLuint), indices, GL_STATIC_DRAW);
	g_model.indexCount = 6;

	// Включает доступ  к нулевом атрибуту - a_position
	glEnableVertexAttribArray(0);

	// 0-й атрибут состоит из 2-х компонентов типа float, которые мы должны использовать без нормировки (GL_FALSE - почитать про нормировку),
	// которые выбирать нужно с шагом 5 * sizeof(GLfloat) байтов 
	// последний параметр - смещение относительно начала, по которому нужно выюирать атрибут первой вершины (смещение нулевое - начинаем с начала массива)
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (const GLvoid *)0);

	// Аналогично для атрибута - a_color
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (const GLvoid *)(2 * sizeof(GLfloat)));

	return g_model.vbo != 0 && g_model.ibo != 0 && g_model.vao != 0;
}

bool init()	
{
	// Set initial color of color buffer to white.
	// Установка цвета, которым будет очищаться цветовой буфер (цвет фона)
	// RGBA, диапазон от 0 до 1 
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

	// createShaderProgram - создание шейдерной программы
	return createShaderProgram() && createModel();
}

void reshape(GLFWwindow *window, int width, int height)
{
	// сообщаем системе размер нашего вью порта, чтобы страло возможным преобразование вью-порт транспор
	glViewport(0, 0, width, height);
}

void draw()
{
	// Clear color buffer. Очистка буфера цвета
	glClear(GL_COLOR_BUFFER_BIT);

	// Использовать такую-то шейдерную программу
	glUseProgram(g_shaderProgram);
	
	// Использовать настройки из такого-то аррэй обжекта
	glBindVertexArray(g_model.vao);

	// Ресует треугольники, далее какое кол-во индексов обходить, далее откуда брать индексы, с каким смещениемм	
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
	// 4 метода, которые запрашивают необходимую версию графического стандарта
	//                glfwWindowHint 
	// первым аргументом необходимо передать идентификатор параметра, 
	//       который подвергается изменению
	// вторым параметром передается значение, которое устанавливается 
	//       соответствующему параметру.

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);					// Мажория
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);					// Минория
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);			// Выключение возможности изменения размера окна 
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // Установка профайла для которого создается контекст

	// Create window.
	// Создаем окно с поверхностью рендеринга (рендеринг - процесс получения изображения по модели)
	// “Высота окна”, “Ширина окна”, “Название окна”, ...
	g_window = glfwCreateWindow(600, 600, "OpenGL Test", NULL, NULL);
	if (g_window == NULL)
	{
		cout << "Failed to open GLFW window" << endl;
		glfwTerminate();
		return false;
	}

	// Initialize OpenGL context with.
	// Активируем контекст OpenGL
	glfwMakeContextCurrent(g_window);

	// Set internal GLEW variable to activate OpenGL core profile.
	//Установка значения glewExperimental в GL_TRUE позволяет GLEW использовать 
	//	новейшие техники для управления функционалом OpenGL.Также, если 
	//	оставить эту переменную со значением по умолчанию, то могут
	//	возникнуть проблемы с использованием Core - profile режима
	glewExperimental = true;

	// Initialize GLEW functions.
	// Инициализация библиотеки GLEW
	if (glewInit() != GLEW_OK)
	{
		cout << "Failed to initialize GLEW" << endl;
		return false;
	}

	// ЕСЛИ ВСЕ НОРМАЛЬНО, OPENGL ГОТОВ К РАБОТЕ

	// Ensure we can capture the escape key being pressed.
	// Настройка способа ввода, для того чтобы отлавливать клавишу esc
	glfwSetInputMode(g_window, GLFW_STICKY_KEYS, GL_TRUE);

	// Set callback for framebuffer resizing event.
	// Определяем функцию обратного вызова на событие изменение размера окна
	// g_window - хэндлер нашего окна,
	// reshape - указатель на функцию
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