#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <vector>

const int MAX_VERTICES_PER_TRIANGLE = 3;
std::vector<float> triangleVertices; // �N���b�N���ꂽ���_�̍��W (x, y, x, y, ...)
int currentVertexCount = 0;

//�E�C���h�E�T�C�Y
extern const unsigned int SCR_WIDTH = 800; // ��: 800
extern const unsigned int SCR_HEIGHT = 600; // ��: 600

unsigned int VAO, VBO;



void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

//�R�[���o�b�N�֐�
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

int main() {
	std::cout << "Hello World!" << std::endl;
	
	// GLFW������
	if (!glfwInit()) {
		std::cerr << "Failed to initialize GLFW" << std::endl;
		return -1;
	}

	glfwInit();

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

	// ... (gladLoadGLLoader �̌�)
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO); // VAO���o�C���h

	glBindBuffer(GL_ARRAY_BUFFER, VBO); // VBO���o�C���h
	// �ŏ��͏\���ȃT�C�Y���m�ۂ��Ă������A�`�掞�Ƀf�[�^�𑗂�
	// �����ł́A�ő�3���_ * 2���W (x,y) ����float���m��
	glBufferData(GL_ARRAY_BUFFER, MAX_VERTICES_PER_TRIANGLE * 2 * sizeof(float), nullptr, GL_DYNAMIC_DRAW);

	// ���_�����̐ݒ� (�ʒu���)
	// layout (location = 0) �ɑΉ�
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
			glPointSize(10.0f); // �_�̑傫����ݒ�
			glDrawArrays(GL_POINTS, 0, 1);
		}
		else if (currentVertexCount == 2) {
			// ����`�� (�I�v�V�����A�v���r���[�p)
			glDrawArrays(GL_LINES, 0, 2);
		}
		else if (currentVertexCount == MAX_VERTICES_PER_TRIANGLE) {
			// �O�p�`��`��
			glDrawArrays(GL_TRIANGLES, 0, MAX_VERTICES_PER_TRIANGLE);

			// �`���A���̎O�p�`�̂��߂Ƀ��Z�b�g
			triangleVertices.clear();
			currentVertexCount = 0;
			std::cout << "Triangle drawn. Ready for next triangle." << std::endl;
		}

		glBindVertexArray(0); // VAO�̃o�C���h����

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	
	
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
		if (currentVertexCount < MAX_VERTICES_PER_TRIANGLE) {
			double xpos, ypos;
			glfwGetCursorPos(window, &xpos, &ypos); // �E�B���h�E���W���擾

			// �E�B���h�E���W��OpenGL�̐��K���f�o�C�X���W (-1.0 to 1.0) �ɕϊ�
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
