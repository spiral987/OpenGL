#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

const int MAX_VERTICES_PER_TRIANGLE = 3;
std::vector<float> triangleVertices; // �N���b�N���ꂽ���_�̍��W (x, y, x, y, ...)
int currentVertexCount = 0;

//�E�C���h�E�T�C�Y
unsigned int SCR_WIDTH = 800; // ��: 800
unsigned int SCR_HEIGHT = 600; // ��: 600

//�����I�ɂ������Ǘ�����N���X���쐬
unsigned int VAO, VBO;

unsigned int shaderProgram;


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

//�R�[���o�b�N�֐�
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

//�V�F�[�_�[��ǂݍ���ŕ�����Ƃ��ĕԂ�
std::string loadShaderSource(const char* filePath);

//shader���R���p�C������֐�
unsigned int compileShader(unsigned int type, const char* source);

//���_�V�F�[�_�[�ƃt���O�����g�V�F�[�_�[�������N���ăV�F�[�_�[�v���O�������쐬����
unsigned int createShaderProgram(const char* vertexPath, const char* fragmentPath);

int main() {
	std::cout << "Hello World!" << std::endl;
	
	// GLFW������
	if (!glfwInit()) {
		std::cerr << "Failed to initialize GLFW" << std::endl;
		return -1;
	}


	// openGL version 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // �R�A�v���t�@�C��

	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "2D Drawing App", NULL, NULL);
	if (window == NULL) {//window not created
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1; //�I������
	}
	glfwMakeContextCurrent(window);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// glad������: OpenGL�֐��|�C���^�����[�h
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cerr << "Failed to initialize GLAD" << std::endl;
		glfwTerminate();
		return -1;
	}

	// �V�F�[�_�[�v���O�����̍쐬
	shaderProgram = createShaderProgram("src/shader.vert", "src/shader.frag"); // �t�@�C���p�X���w��
	if (shaderProgram == 0) {
		// �V�F�[�_�[�쐬���s���̏��� (�Ⴆ�΃v���O�����I��)
		glfwTerminate();
		return -1;
	}


	// VAO��VBO�𐶐����Đݒ�
	/*
		VBO�͒��_�����i�[����GPB���̃o�b�t�@(�������̈�)
		VAO�̓o�b�t�@�I�u�W�F�N�g�̑������܂Ƃ߁A�V�F�[�_�[�֋��n������

		1.���_�f�[�^������
		2.VBO�ɒ��_�f�[�^���i�[
		3.VAO��VBO���܂Ƃ߂�
		4.�V�F�[�_�[��K�p���A�h���[�R�[���ŕ`��
	*/
	glGenVertexArrays(1, &VAO);//VAO�𐶐�
	glGenBuffers(1, &VBO);//VBO�𐶐�

	glBindVertexArray(VAO); // VAO���o�C���h(�I��)

	glBindBuffer(GL_ARRAY_BUFFER, VBO); // VBO���o�C���h(�I��)

	//glBufferData(GL_ARRAY_BUFFER, size, data, usage) 
	//���ۂ̒��_�f�[�^��VBO�ɃA�b�v���[�h����
	//GL_DYNAMIC__DRAW=�f�[�^���p�ɂɍX�V������
	glBufferData(GL_ARRAY_BUFFER, MAX_VERTICES_PER_TRIANGLE * 2 * sizeof(float), nullptr, GL_DYNAMIC_DRAW);

	// ���_�����̐ݒ� (�ʒu���)
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0); // VBO�̃o�C���h����
	glBindVertexArray(0); // VAO�̃o�C���h����

	// ... (�`�惋�[�v��)
	while (!glfwWindowShouldClose(window)) {
		// ���͏��� (��: ESC�L�[�ŕ���Ȃ�)
		processInput(window); // processInput�֐��͕ʓr��`����Ă���Ɖ���

		// �����_�����O�R�}���h
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// �V�F�[�_�[�v���O�������g�p
		glUseProgram(shaderProgram); // shaderProgram �͎��O�ɍ쐬�����V�F�[�_�[�v���O������ID

		// ���_�f�[�^���X�V���ꂽ��VBO�ɍđ��M
		if (!triangleVertices.empty()) {
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			// VBO�̐擪����f�[�^���X�V
			glBufferSubData(GL_ARRAY_BUFFER, 0, triangleVertices.size() * sizeof(float), triangleVertices.data());
			glBindBuffer(GL_ARRAY_BUFFER, 0); // ���S�̂��߃A���o�C���h
		}

		glBindVertexArray(VAO); // �`��Ɏg��VAO���o�C���h

		if (currentVertexCount == 1) {
			// �_��`�� (�I�v�V����)
			glPointSize(4.0f); // �_�̑傫����ݒ�
			glDrawArrays(GL_POINTS, 0, 1);
		}
		else if (currentVertexCount == 2) {
			// ����`�� (�I�v�V�����A�v���r���[�p)
			glDrawArrays(GL_LINES, 0, 2);
		}
		else if (currentVertexCount == MAX_VERTICES_PER_TRIANGLE) {
			// �O�p�`��`��

			glDrawArrays(GL_TRIANGLES, 0, MAX_VERTICES_PER_TRIANGLE);
		}

		glBindVertexArray(0); // VAO�̃o�C���h����

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteProgram(shaderProgram); // �V�F�[�_�[�v���O�������폜
	
	glfwTerminate();


	return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

// ���͏����֐� (ESC�L�[�ŃE�B���h�E������)
void processInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

//�N���b�N���ꂽ���W���擾���A���_�f�[�^�Ƃ��ĕۑ�����
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
	//�}�E�X�E�N���b�N�̎�
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		if (currentVertexCount == MAX_VERTICES_PER_TRIANGLE) {
			// ����1�̎O�p�`���������Ă�ꍇ �� �V�����O�p�`��`������
			triangleVertices.clear();
			currentVertexCount = 0;
			std::cout << "Triangle drawn. Ready for next triangle." << std::endl;
		}
		if (currentVertexCount < MAX_VERTICES_PER_TRIANGLE) {
			double xpos, ypos;
			glfwGetCursorPos(window, &xpos, &ypos); // �E�B���h�E���W���擾

			// �E�B���h�E���W��OpenGL�̐��K���f�o�C�X���W (-1.0 to 1.0) �ɕϊ�
			//�E�C���h�E���W�n�ł͍��オ���_�Ȃ��߁AY�������]����B
			float ndcX = (float)(xpos / SCR_WIDTH) * 2.0f - 1.0f;
			float ndcY = 1.0f - (float)(ypos / SCR_HEIGHT) * 2.0f; // Y���͔��]

			triangleVertices.push_back(ndcX);
			triangleVertices.push_back(ndcY);
			//���_�N���b�N���J�E���g++
			currentVertexCount++;

			std::cout << "Vertex " << currentVertexCount << " added at: (" << ndcX << ", " << ndcY << ")" << std::endl;

			// (VBO�X�V�����͕`�惋�[�v�������ōs�� - ��q)
		}
	}
}

// �V�F�[�_�[�t�@�C����ǂݍ���ŕ�����Ƃ��ĕԂ��֐�
std::string loadShaderSource(const char* filePath) {
	std::ifstream shaderFile;
	std::stringstream shaderStream;

	// �t�@�C�����J��
	shaderFile.open(filePath);
	if (shaderFile.is_open()) {
		// �t�@�C���̓��e���X�g���[���ɓǂݍ���
		shaderStream << shaderFile.rdbuf();
		shaderFile.close();
		// �X�g���[�����當����ɕϊ�
		return shaderStream.str();
	}
	else {
		std::cerr << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ: " << filePath << std::endl;
		return ""; // �󕶎���Ԃ�
	}
}

// �V�F�[�_�[���R���p�C������w���p�[�֐�
unsigned int compileShader(unsigned int type, const char* source) {
	unsigned int id = glCreateShader(type); // �V�F�[�_�[�I�u�W�F�N�g�쐬
	glShaderSource(id, 1, &source, nullptr); // �V�F�[�_�[�\�[�X���Z�b�g
	glCompileShader(id); // �R���p�C��

	// �R���p�C���G���[�`�F�b�N
	int success;
	char infoLog[512];
	glGetShaderiv(id, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(id, 512, nullptr, infoLog);
		std::cerr << "ERROR::SHADER::" << (type == GL_VERTEX_SHADER ? "VERTEX" : "FRAGMENT") << "::COMPILATION_FAILED\n" << infoLog << std::endl;
		glDeleteShader(id); // �s�v�ɂȂ����V�F�[�_�[�I�u�W�F�N�g���폜
		return 0;
	}
	return id;
}

// ���_�V�F�[�_�[�ƃt���O�����g�V�F�[�_�[�������N���ăV�F�[�_�[�v���O�������쐬����֐�
unsigned int createShaderProgram(const char* vertexPath, const char* fragmentPath) {
	// 1. �V�F�[�_�[�\�[�X���t�@�C������ǂݍ���
	std::string vertexCodeStr = loadShaderSource(vertexPath);
	std::string fragmentCodeStr = loadShaderSource(fragmentPath);
	if (vertexCodeStr.empty() || fragmentCodeStr.empty()) {
		return 0; // �ǂݍ��ݎ��s
	}
	const char* vertexShaderSource = vertexCodeStr.c_str();
	const char* fragmentShaderSource = fragmentCodeStr.c_str();

	// 2. ���_�V�F�[�_�[���R���p�C��
	unsigned int vertexShader = compileShader(GL_VERTEX_SHADER, vertexShaderSource);
	if (vertexShader == 0) return 0;

	// 3. �t���O�����g�V�F�[�_�[���R���p�C��
	unsigned int fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);
	if (fragmentShader == 0) {
		glDeleteShader(vertexShader); // ���_�V�F�[�_�[���폜
		return 0;
	}

	// 4. �V�F�[�_�[�v���O�������쐬���A�V�F�[�_�[�������N
	unsigned int program = glCreateProgram(); // �v���O�����I�u�W�F�N�g�쐬
	glAttachShader(program, vertexShader);     // ���_�V�F�[�_�[���A�^�b�`
	glAttachShader(program, fragmentShader);   // �t���O�����g�V�F�[�_�[���A�^�b�`
	glLinkProgram(program);                    // �����N

	// �����N�G���[�`�F�b�N
	int success;
	char infoLog[512];
	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(program, 512, nullptr, infoLog);
		std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
		glDeleteShader(vertexShader);   // �����N��ɂ͕s�v�Ȃ̂ō폜
		glDeleteShader(fragmentShader); // ����
		glDeleteProgram(program);       // �v���O�������폜
		return 0;
	}

	// �����N������������A�X�̃V�F�[�_�[�I�u�W�F�N�g�͕s�v�ɂȂ�̂ō폜
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	return program; // �쐬�����V�F�[�_�[�v���O������ID��Ԃ�
}


