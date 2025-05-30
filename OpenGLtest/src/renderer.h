#ifndef RENDERER_H
#define RENDERER_H

#include <glad/glad.h>
#include <vector>
#include "Shader.h" // 作成したShaderクラスを使用

// 描画モードのenumもここに定義するか、別の共通ヘッダに移動しても良い
enum class PrimitiveType {
    POINTS,
    LINES,
    LINE_STRIP,
    TRIANGLES
};

class Renderer {
public:
    Renderer();
    ~Renderer();

    // VAO/VBOの初期化 (頂点属性ポインタの設定など)
    // bufferHint: GL_STATIC_DRAW, GL_DYNAMIC_DRAW など
    // initialBufferSize: VBOに最初に確保するサイズ (バイト単位)。0の場合、描画時に毎回glBufferData。
    bool initialize(GLenum bufferHint = GL_DYNAMIC_DRAW, GLsizeiptr initialBufferSize = 1000 * 2 * sizeof(float));

    // 描画データのVBOへの送信と描画の実行
    // primitiveType: GL_POINTS, GL_LINES, GL_LINE_STRIP, GL_TRIANGLES
    // vertices: 頂点データ (x, y, x, y, ...)
    // vertexCount: 描画する頂点の数
    void draw(PrimitiveType type, const std::vector<float>& vertices, const Shader& shader, float pointSize = 1.0f);

private:
    unsigned int VAO;
    unsigned int VBO;
    GLenum currentBufferHint;
    GLsizeiptr currentBufferSize; // 現在VBOに確保されているサイズ

    void cleanup(); // VAO/VBOの削除
};

#endif // RENDERER_H