//�V�F�[�_�[�N���X�F�V�F�[�_�[�̓ǂݍ��݁A�R���p�C���A�����N�A�V�F�[�_�[�v���O�����̎g�p�Ɣj�����J�v�Z��������B

#include <shader.h> // �Ή�����w�b�_�t�@�C�����C���N���[�h
#include <fstream>   // std::ifstream
#include <sstream>   // std::stringstream
#include <iostream>  // std::cerr, std::endl


//--------------public--------------------------

// �R���X�g���N�^: �V�F�[�_�[�t�@�C����ǂݍ��݁A�R���p�C�����A�����N���ăv���O�������쐬
Shader::Shader(const char* vertexPath, const char* fragmentPath) {
    // 1. �V�F�[�_�[�\�[�X���t�@�C������ǂݍ���
    std::string vertexCodeStr = loadShaderSourceFromFile(vertexPath);
    std::string fragmentCodeStr = loadShaderSourceFromFile(fragmentPath);

    if (vertexCodeStr.empty() || fragmentCodeStr.empty()) {
        ID = 0; // �G���[����ID��0�ɐݒ� (�܂��͗�O�𓊂���)
        return;
    }

    const char* vShaderCode = vertexCodeStr.c_str();
    const char* fShaderCode = fragmentCodeStr.c_str();

    // 2. �V�F�[�_�[���R���p�C��
    unsigned int vertex, fragment;

    // ���_�V�F�[�_�[
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);
    checkCompileErrors(vertex, "VERTEX");

    // �t���O�����g�V�F�[�_�[
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
    checkCompileErrors(fragment, "FRAGMENT");

    // 3. �V�F�[�_�[�v���O�������쐬���A�V�F�[�_�[�������N
    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    glLinkProgram(ID);
    checkLinkErrors(ID);

    // �����N������������A�X�̃V�F�[�_�[�I�u�W�F�N�g�͕s�v�ɂȂ�̂ō폜
    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

// �f�X�g���N�^: �V�F�[�_�[�v���O�������폜
Shader::~Shader() {
    if (ID != 0) {
        glDeleteProgram(ID);
    }
}

// �V�F�[�_�[�v���O�������g�p (�A�N�e�B�u��)
void Shader::use() const {
    if (ID != 0) {
        glUseProgram(ID);
    }
}

// --- private�w���p�[�֐� ---
// 
// �V�F�[�_�[�t�@�C����ǂݍ���ŕ�����Ƃ��ĕԂ��֐�
std::string Shader::loadShaderSourceFromFile(const char* filePath) {
    std::ifstream shaderFile;
    std::stringstream shaderStream;

    // �t�@�C�����J���ۂ̗�O�ݒ� (�C��)
    shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try {
        shaderFile.open(filePath);
        shaderStream << shaderFile.rdbuf();
        shaderFile.close();
        return shaderStream.str();
    }
    catch (const std::ifstream::failure& e) {
        std::cerr << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ: " << filePath << " (" << e.what() << ")" << std::endl;
        return "";
    }
}

// �V�F�[�_�[�R���p�C�����̃G���[���`�F�b�N����֐�
void Shader::checkCompileErrors(GLuint shader, std::string type) {
    GLint success;
    GLchar infoLog[1024]; // �G���[���O�o�b�t�@�̃T�C�Y�͓K�X����
    if (type != "PROGRAM") {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            std::cerr << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
    // �v���O���������N���̃G���[�`�F�b�N�� checkLinkErrors �ōs��
}

// �V�F�[�_�[�v���O���������N���̃G���[���`�F�b�N����֐�
void Shader::checkLinkErrors(GLuint program) {
    GLint success;
    GLchar infoLog[1024];
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program, 1024, NULL, infoLog);
        std::cerr << "ERROR::PROGRAM_LINKING_ERROR of program: " << program << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
    }
}
