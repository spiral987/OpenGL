//シェーダークラス：シェーダーの読み込み、コンパイル、リンク、シェーダープログラムの使用と破棄をカプセル化する。

#include <shader.h> // 対応するヘッダファイルをインクルード
#include <fstream>   // std::ifstream
#include <sstream>   // std::stringstream
#include <iostream>  // std::cerr, std::endl


//--------------public--------------------------

// コンストラクタ: シェーダーファイルを読み込み、コンパイルし、リンクしてプログラムを作成
Shader::Shader(const char* vertexPath, const char* fragmentPath) {
    // 1. シェーダーソースをファイルから読み込む
    std::string vertexCodeStr = loadShaderSourceFromFile(vertexPath);
    std::string fragmentCodeStr = loadShaderSourceFromFile(fragmentPath);

    if (vertexCodeStr.empty() || fragmentCodeStr.empty()) {
        ID = 0; // エラー時はIDを0に設定 (または例外を投げる)
        return;
    }

    const char* vShaderCode = vertexCodeStr.c_str();
    const char* fShaderCode = fragmentCodeStr.c_str();

    // 2. シェーダーをコンパイル
    unsigned int vertex, fragment;

    // 頂点シェーダー
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);
    checkCompileErrors(vertex, "VERTEX");

    // フラグメントシェーダー
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
    checkCompileErrors(fragment, "FRAGMENT");

    // 3. シェーダープログラムを作成し、シェーダーをリンク
    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    glLinkProgram(ID);
    checkLinkErrors(ID);

    // リンクが成功したら、個々のシェーダーオブジェクトは不要になるので削除
    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

// デストラクタ: シェーダープログラムを削除
Shader::~Shader() {
    if (ID != 0) {
        glDeleteProgram(ID);
    }
}

// シェーダープログラムを使用 (アクティブ化)
void Shader::use() const {
    if (ID != 0) {
        glUseProgram(ID);
    }
}

// --- privateヘルパー関数 ---
// 
// シェーダーファイルを読み込んで文字列として返す関数
std::string Shader::loadShaderSourceFromFile(const char* filePath) {
    std::ifstream shaderFile;
    std::stringstream shaderStream;

    // ファイルを開く際の例外設定 (任意)
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

// シェーダーコンパイル時のエラーをチェックする関数
void Shader::checkCompileErrors(GLuint shader, std::string type) {
    GLint success;
    GLchar infoLog[1024]; // エラーログバッファのサイズは適宜調整
    if (type != "PROGRAM") {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            std::cerr << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
    // プログラムリンク時のエラーチェックは checkLinkErrors で行う
}

// シェーダープログラムリンク時のエラーをチェックする関数
void Shader::checkLinkErrors(GLuint program) {
    GLint success;
    GLchar infoLog[1024];
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program, 1024, NULL, infoLog);
        std::cerr << "ERROR::PROGRAM_LINKING_ERROR of program: " << program << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
    }
}
