#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <glad/glad.h> // GLuint のために必要

class Shader {
public:
    unsigned int ID; // シェーダープログラムのID

    Shader(const char* vertexPath, const char* fragmentPath);
    ~Shader();

    void use() const;
    // 必要に応じて、uniform変数を設定する関数などを追加
    // void setBool(const std::string &name, bool value) const;
    // void setInt(const std::string &name, int value) const;
    // void setFloat(const std::string &name, float value) const;

private:
    std::string loadShaderSourceFromFile(const char* filePath);
    void checkCompileErrors(GLuint shader, std::string type);
    void checkLinkErrors(GLuint program);
};

#endif