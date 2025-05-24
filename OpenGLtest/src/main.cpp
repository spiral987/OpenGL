#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
//#include <glm/glm.hpp>
//#include <glm/gtc/matrix_transform.hpp>
//#include <glm/gtc/type_ptr.hpp>


// --------------- �O���[�o���ϐ� ----------------
const int MAX_VERTICES_PER_TRIANGLE = 3;
//���_�f�[�^���Ǘ�����x�N�^
std::vector<float> triangleVertices;
std::vector<float> freehandVertices;
int currentVertexCount = 0; // �O�p�`�`�掞�̒��_�J�E���g

unsigned int SCR_WIDTH = 800;
unsigned int SCR_HEIGHT = 600;

unsigned int VAO, VBO; // �`��I�u�W�F�N�g (����͋��L)
unsigned int shaderProgram;

bool isMouseButtonDown = false; // �}�E�X�{�^����������Ă��邩


//---------------------�`�惂�[�h------------------------
enum class DrawingMode {
	TRIANGLE,
	FREEHAND
};
DrawingMode currentMode = DrawingMode::TRIANGLE;

//---------------------�R�[���o�b�N�֐��̃v���g�^�C�v�錾-----------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
//�}�E�X�N���b�N���̃R�[���o�b�N
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
// �}�E�X�ړ����̃R�[���o�b�N
void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
//�L�[�������ꂽ�Ƃ��̃R�[���o�b�N
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods); // �V��


//-----------------�V�F�[�_�[�֘A�̃v���g�^�C�v�錾---------------------
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

	//�R�[���o�b�N�֐��̓o�^
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetCursorPosCallback(window, cursor_position_callback);
	glfwSetKeyCallback(window, key_callback); // �L�[�R�[���o�b�N��o�^

	// glad������: OpenGL�֐��|�C���^�����[�h
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cerr << "Failed to initialize GLAD" << std::endl;
		glfwTerminate();
		return -1;
	}

	// --- ImGui�̃Z�b�g�A�b�v ---
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	// io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // �L�[�{�[�h�i�r�Q�[�V�������K�v�ȏꍇ
	// io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // �Q�[���p�b�h�i�r�Q�[�V�������K�v�ȏꍇ

	// ImGui�̃X�^�C���ݒ� (�C��)
	ImGui::StyleColorsDark();
	// ImGui::StyleColorsClassic();

	// �v���b�g�t�H�[��/�����_���[�o�b�N�G���h�̏�����
	ImGui_ImplGlfw_InitForOpenGL(window, true); // 'true' �̓R�[���o�b�N���C���X�g�[�����邩�ǂ���
	ImGui_ImplOpenGL3_Init("#version 330 core"); // GLSL�̃o�[�W����������


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
	//�璸�_���o�^
	glBufferData(GL_ARRAY_BUFFER, 1000 * 2 * sizeof(float), nullptr, GL_DYNAMIC_DRAW);

	// ���_�����̐ݒ� (�ʒu���)
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0); // VBO�̃o�C���h����
	glBindVertexArray(0); // VAO�̃o�C���h����

	// ... (�`�惋�[�v��)
	while (!glfwWindowShouldClose(window)) {
		// ���͏��� (��: ESC�L�[�ŕ���Ȃ�)
		processInput(window); // processInput�֐��͕ʓr��`����Ă���Ɖ���

		glfwPollEvents(); // ImGui_ImplGlfw_NewFrame �̑O�ɃC�x���g����

		// --- ImGui�̐V�����t���[�����J�n ---
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		// --- ������ImGui��UI�v�f���L�q ---
		// ��ʍ���Ɍ��݂̃��[�h��\��
		ImGui::SetNextWindowPos(ImVec2(10, 10)); // �\���ʒu (���ォ��10,10�s�N�Z��)
		ImGui::Begin("Current Mode", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav);
		if (currentMode == DrawingMode::TRIANGLE) {
			ImGui::Text("Mode: Triangle");
		}
		else if (currentMode == DrawingMode::FREEHAND) {
			ImGui::Text("Mode: Freehand");
		}
		ImGui::End();
		// --- ImGui��UI�v�f�L�q�����܂� ---
		

		// �����_�����O�R�}���h
		glClearColor(0.9f, 0.9f, 0.9f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// �V�F�[�_�[�v���O�������g�p
		glUseProgram(shaderProgram); // shaderProgram �͎��O�ɍ쐬�����V�F�[�_�[�v���O������ID

		glBindVertexArray(VAO); // �`��Ɏg��VAO���o�C���h

		if (currentMode == DrawingMode::TRIANGLE) {
			// --- �O�p�`�`�惂�[�h�̏��� ---
			if (!triangleVertices.empty()) {
				glBindBuffer(GL_ARRAY_BUFFER, VBO);
				glBufferSubData(GL_ARRAY_BUFFER, 0, triangleVertices.size() * sizeof(float), triangleVertices.data());
				// VBO���A���o�C���h���Ȃ��iVAO��VBO�̎Q�Ƃ�ێ����Ă��邽�߁A�`�撼�O�܂Ńo�C���h����Ă���z���OK�j
			}

			if (currentVertexCount == 1) {
				glPointSize(4.0f);
				glDrawArrays(GL_POINTS, 0, 1);
			}
			else if (currentVertexCount == 2) {
				glDrawArrays(GL_LINES, 0, 2);
			}
			else if (currentVertexCount == MAX_VERTICES_PER_TRIANGLE) {
				glDrawArrays(GL_TRIANGLES, 0, MAX_VERTICES_PER_TRIANGLE);
				// �O�p�`�`���̃��Z�b�g��mouse_button_callback�ōs���悤�ɕύX�ς�
			}
		}
		else if (currentMode == DrawingMode::FREEHAND) {
			// --- ���R���`�惂�[�h�̏��� ---
			if (!freehandVertices.empty() && freehandVertices.size() >= 4) { // ���Ȃ��Ƃ�2�_(4float)�Ȃ��Ɛ��͕`���Ȃ�
				glBindBuffer(GL_ARRAY_BUFFER, VBO);
				glBufferSubData(GL_ARRAY_BUFFER, 0, freehandVertices.size() * sizeof(float), freehandVertices.data());

				//GL_LINE_STRIP�ł͒��_�����Ԓ�����`��
				glDrawArrays(GL_LINE_STRIP, 0, freehandVertices.size() / 2);
			}
		}

		glBindVertexArray(0); // VAO�̃o�C���h����


		// --- ImGui�̕`�� ---
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		// --- ImGui�̕`�悱���܂� ---

		glfwSwapBuffers(window);

	}

	// --- ImGui�̃N���[���A�b�v ---
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	// --- ImGui�̃N���[���A�b�v�����܂� ---


	glDeleteVertexArrays(1, &VAO); // VAO�̍폜
	glDeleteBuffers(1, &VBO);   // VBO�̍폜
	glDeleteProgram(shaderProgram);
	glfwTerminate();


	return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
	SCR_WIDTH = width;  // �E�B���h�E���T�C�Y���ɃO���[�o���ϐ����X�V
	SCR_HEIGHT = height;
}

// ���͏����֐� (ESC�L�[�ŃE�B���h�E������)
void processInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

// --- �L�[���̓R�[���o�b�N�֐� ---
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_M && action == GLFW_PRESS) {
		if (currentMode == DrawingMode::TRIANGLE) {
			currentMode = DrawingMode::FREEHAND;
			glfwSetWindowTitle(window, "2D Drawing App (Mode: FREEHAND)");
			std::cout << "Mode changed to: FREEHAND" << std::endl;
			// ���[�h�ύX���ɕ`��r���̃f�[�^���N���A����
			triangleVertices.clear();
			currentVertexCount = 0;
			freehandVertices.clear();
		}
		else {
			currentMode = DrawingMode::TRIANGLE;
			glfwSetWindowTitle(window, "2D Drawing App (Mode: TRIANGLE)");
			std::cout << "Mode changed to: TRIANGLE" << std::endl;
			// ���[�h�ύX���ɕ`��r���̃f�[�^���N���A����
			triangleVertices.clear();
			currentVertexCount = 0;
			freehandVertices.clear();
		}
	}
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            isMouseButtonDown = true;
            if (currentMode == DrawingMode::TRIANGLE) {
                if (currentVertexCount == MAX_VERTICES_PER_TRIANGLE) {
                    triangleVertices.clear();
                    currentVertexCount = 0;
                    std::cout << "Ready for next triangle." << std::endl;
                }
                // currentVertexCount < MAX_VERTICES_PER_TRIANGLE �̃`�F�b�N�͕s�v�ɂȂ�i��ɐV�������_��ǉ��j
                double xpos, ypos;
                glfwGetCursorPos(window, &xpos, &ypos);
				//�f�B�X�v���C���W����OpenGL�̍��W�n��
                float ndcX = (float)(xpos / SCR_WIDTH) * 2.0f - 1.0f;
                float ndcY = 1.0f - (float)(ypos / SCR_HEIGHT) * 2.0f;
				//���_�x�N�^�Ɋi�[
                triangleVertices.push_back(ndcX);
                triangleVertices.push_back(ndcY);
				//�J�E���g�A�b�v
                currentVertexCount++;
                std::cout << "Vertex " << currentVertexCount << " added for triangle at: (" << ndcX << ", " << ndcY << ")" << std::endl;

            } else if (currentMode == DrawingMode::FREEHAND) {
                freehandVertices.clear(); // �V��������`���n�߂�̂ŃN���A
                // �ŏ��̓_��ǉ� (cursor_position_callback�ł��ǉ������̂ŁA�����ł͕s�v��������Ȃ�)
                // double xpos, ypos;
                // glfwGetCursorPos(window, &xpos, &ypos);
                // float ndcX = (float)(xpos / SCR_WIDTH) * 2.0f - 1.0f;
                // float ndcY = 1.0f - (float)(ypos / SCR_HEIGHT) * 2.0f;
                // freehandVertices.push_back(ndcX);
                // freehandVertices.push_back(ndcY);
                // std::cout << "Freehand drawing started at: (" << ndcX << ", " << ndcY << ")" << std::endl;
            }
        } else if (action == GLFW_RELEASE) {
            isMouseButtonDown = false;
            if (currentMode == DrawingMode::FREEHAND) {
                std::cout << "Freehand drawing ended. Points: " << freehandVertices.size() / 2 << std::endl;
                // ������freehandVertices���i���I�ȕ`�惊�X�g�ɒǉ�����Ȃǂ̏������l������
            }
        }
    }
}

//�J�[�\���ʒu�̃R�[���o�b�N
void cursor_position_callback(GLFWwindow* window, double xpos_window, double ypos_window) {
	if (isMouseButtonDown && currentMode == DrawingMode::FREEHAND) {
		float ndcX = (float)(xpos_window / SCR_WIDTH) * 2.0f - 1.0f;
		float ndcY = 1.0f - (float)(ypos_window / SCR_HEIGHT) * 2.0f;

		// �O�̓_�Ɠ����łȂ���Βǉ� (�_�̏d���������)
		if (freehandVertices.empty() ||
			freehandVertices[freehandVertices.size() - 2] != ndcX ||
			freehandVertices[freehandVertices.size() - 1] != ndcY)
		{
			freehandVertices.push_back(ndcX);
			freehandVertices.push_back(ndcY);
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


