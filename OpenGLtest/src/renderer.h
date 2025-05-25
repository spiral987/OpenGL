#ifndef RENDERER_H
#define RENDERER_H

#include <vector>
#include <glad/glad.h>
// #include "Shader.h" // シェーダークラスを使うならインクルード

class Renderer {
public:
    Renderer();
    ~Renderer();

    void setupTriangleDrawing(); // VAO/VBOの初期セットアップ用
    void setupFreehandDrawing(); // (もしVAO/VBOを分けるなら)

    // 描画関数 (描画モードやデータに応じて)
    void drawTriangle(const std::vector<float>& vertices, int vertexCount, const Shader& shader);
    void drawFreehandLine(const std::vector<float>& vertices, const Shader& shader);
    void drawPoint(const std::vector<float>& vertices, float pointSize, const Shader& shader);
    void drawLines(const std::vector<float>& vertices, const Shader& shader);


private:
    unsigned int VAO_triangle, VBO_triangle;
    unsigned int VAO_freehand, VBO_freehand; // モードごとにVAO/VBOを分ける場合
    // もしVAO/VBOを共有するなら unsigned int genericVAO, genericVBO;
};
#endif