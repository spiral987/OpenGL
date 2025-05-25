#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <glad/glad.h> // GLuint �̂��߂ɕK�v

class Shader {
public:
    unsigned int ID; // �V�F�[�_�[�v���O������ID

    Shader(const char* vertexPath, const char* fragmentPath);
    ~Shader();

    void use() const;
    // �K�v�ɉ����āAuniform�ϐ���ݒ肷��֐��Ȃǂ�ǉ�
    // void setBool(const std::string &name, bool value) const;
    // void setInt(const std::string &name, int value) const;
    // void setFloat(const std::string &name, float value) const;

private:
    std::string loadShaderSourceFromFile(const char* filePath);
    void checkCompileErrors(GLuint shader, std::string type);
    void checkLinkErrors(GLuint program);
};

#endif