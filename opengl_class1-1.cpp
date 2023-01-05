#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "SharderInfo.h"
#define BUFFER_OFFSET(offset) ((void *)(offset))

enum VertexAarryObjectID {
	Triangles = 0,
	VAOSize = 1 
}; // 顶点数组对象的id
enum BufferID {
	ArrayBuffer = 0,
	NumBuffers = 1 
};
enum AttribID { 
	vPosition = 0 //顶点数组数据在缓存中的位置
};

const int vaosNum = 1;
GLuint VertexAarryObjects[VAOSize] = { 0 }; //顶点数组
const int buffersNum = 1;
GLuint Buffers[buffersNum] = { 0 };

const GLuint NumVertices = 6;
const int logInfoBufferSize = 512;

void initVerticesArray() {
	// 	static const GLfloat vertices[NumVertices][2] = {
	// 		{ -0.90f, -0.90f },
	// 		{  0.85f, -0.90f },
	// 		{ -0.90f,  0.85f }, // triangles1
	// 		{  0.90f, -0.85f },
	// 		{  0.90f,  0.90f },
	// 		{ -0.85f,  0.90f }  // triangles2
	// 	};
	static const GLfloat vertices[NumVertices][2] = {
		{-0.9f, -0.5f},  // left 
		{-0.0f, -0.5f},  // right
		{-0.45f, 0.5f},  // top 
				// second triangle
// 	{0.0f, -0.5f},  // left
// 	{0.9f, -0.5f},  // right
// 	{0.45f, 0.5f}   // top 
	};


	// step1 创建顶点数组对象
	glGenVertexArrays(vaosNum, VertexAarryObjects);  //创建顶点数组对象,NumVAOs 创建的个数 VAOs缓存对象数组	
	// step2 将顶点数组绑定到opengl (将顶点数组对象设置为当前对象)
	glBindVertexArray(VertexAarryObjects[Triangles]);

	// step3 创建缓存
	glGenBuffers(buffersNum, Buffers); //创建缓存buffer
	// step4 将当前的缓存绑定到opengl
	glBindBuffer(GL_ARRAY_BUFFER, Buffers[ArrayBuffer]); //绑定时设置类型为数组类型

	// step5 分配缓存在gpu的储存空间,并将数据从拷贝到gpu内存中
	glNamedBufferStorage(Buffers[ArrayBuffer], sizeof(vertices), vertices, 0);
	glBufferStorage(GL_ARRAY_BUFFER, sizeof(vertices), vertices, 0);
	
	// step6 把gpu缓存内的数据输入到着色器中
	glVertexAttribPointer(vPosition, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
	// step7 启动顶点数组属性
	glEnableVertexAttribArray(vPosition);
}

//-------------initGLProgram 创建着色器程序--------------//
//创建着色器程序需要创建着色器程序和着色器对象,并将着色器对象编译好后链接到着色器程序中
GLuint initGLProgram() {
	char logInfo[logInfoBufferSize] = { 0 }; //用于记录错误日志
	int success = -1;
	//step1 创建着色器程序
	GLuint glProgram = glCreateProgram();
	//step1 end

	//---一个opengl程序至少应该含有两个着色器对象(顶点着色器与片元着色器)---//
	//step2 创建一个顶点着色器对象
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER); //创建顶点着色器对象
	
	SharderReader vertInfo("vert.glsl"); 
	vertInfo.load();
	const char* strVert = vertInfo.data();
	glShaderSource(vertexShader, 1, &strVert, nullptr); //将着色器源码关联到着色器对象
	glCompileShader(vertexShader); //编译着色器对象源码
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);//查看着色器对象编译情况
	if (!success) {
		glGetShaderInfoLog(vertexShader, logInfoBufferSize, nullptr, logInfo); //查看着色器对象编译报错信息
		std::cout << "vertext shader compile error. err_msg = " << logInfo << std::endl;
		return 0;
	}
	//step2 end

	//step3 创建一个片元着色器对象
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER); //创建一个片元着色器对象
	SharderReader fragmentInfo("frag.glsl");
	fragmentInfo.load();
	const char* strFrag= fragmentInfo.data();
	glShaderSource(fragmentShader, 1, &strFrag, nullptr);
	glCompileShader(fragmentShader);
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vertexShader, logInfoBufferSize, nullptr, logInfo);
		std::cout << "fragment shader compile error. err_msg = " << logInfo << std::endl;
		return 0;
	}
	//step3 end
	
	//step4 将着色器对象关联到着色器程序
	glAttachShader(glProgram, vertexShader);
	glAttachShader(glProgram, fragmentShader);
	//step4 end

	//step5 着色器程序链接着色器对象
	glLinkProgram(glProgram); //着色器程序链接着色器对象
	glGetProgramiv(glProgram, GL_LINK_STATUS, &success); //查看着色器程序链接情况
	if (!success) {
		glGetProgramInfoLog(glProgram, logInfoBufferSize, nullptr, logInfo); //查看着色器程序报错信息
		std::cout << "glprogram link error. err_msg = " << logInfo << std::endl;
		return 0;
	}
	//step5 end

	//step6 删掉已经无用了的着色器对象,着色器对象在程序链接后就没有用了(相当于静态库加载到程序中可以删掉了)
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	//step6 end

	glUseProgram(glProgram);
	return glProgram;
}

void display() {
	static const GLfloat red[] = { 0.1f, 0.1f, 0.1f, 1.0f };
	//step1 清除之前帧缓存的内容
	glClearBufferfv(GL_COLOR, 0, red);
	// step2 不断刷新顶点数组
	glBindVertexArray(VertexAarryObjects[Triangles]);
	// 使用当前绑定的顶点数组建立一系列的几何图元
	glDrawArrays(GL_TRIANGLES, 0, NumVertices); // GL_TRIANGLES 表示绘制三角形, 
}

int main() {
	// step1 初始化glfw
	glfwInit();
	// step1 end

	// step2 创建窗口
	GLFWwindow* window = glfwCreateWindow(640, 480, "Triangles", NULL, NULL);
	glfwMakeContextCurrent(window);

	// step3 初始化glad
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// step4 初始化着色器
	initGLProgram();
	// step5 初始化顶点数组
	initVerticesArray();
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	// step6 开启渲染循环
	while (!glfwWindowShouldClose(window)) {
		display();
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	//step6 析构资源
	glfwDestroyWindow(window);

	glfwTerminate();

	return 0;
}
