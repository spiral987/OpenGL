#include "Renderer.h"
#include <iostream> // エラー表示用


Renderer::Renderer() : VAO(0), VBO(0), currentBufferHint(GL_DYNAMIC_DRAW), currentBufferSize(0) {
}

//レンダラーを削除
Renderer::~Renderer() {
    cleanup();
}

bool Renderer::initialize(GLenum bufferHint, GLsizeiptr initialBufferSize) {
    currentBufferHint = bufferHint;
    currentBufferSize = initialBufferSize;

    	// VAOとVBOを生成して設定
	/*
		VBOは頂点情報を格納するGPB側のバッファ(メモリ領域)
		VAOはバッファオブジェクトの属性をまとめ、シェーダーへ橋渡しする

		1.頂点データを準備
		2.VBOに頂点データを格納
		3.VAOにVBOをまとめる
		4.シェーダーを適用し、ドローコールで描画
	*/

    glGenVertexArrays(1, &VAO);
    if (VAO == 0) {
        std::cerr << "ERROR::RENDERER::Failed to generate VAO" << std::endl;
        return false;
    }
    glGenBuffers(1, &VBO);
    if (VBO == 0) {
        std::cerr << "ERROR::RENDERER::Failed to generate VBO" << std::endl;
        glDeleteVertexArrays(1, &VAO); // 生成したVAOをクリーンアップ
        VAO = 0;
        return false;
    }

    glBindVertexArray(VAO); // VAOをバインド(選択する)

    glBindBuffer(GL_ARRAY_BUFFER, VBO); // VBOをバインド(選択する)
    if (initialBufferSize > 0) {
    //glBufferData(GL_ARRAY_BUFFER, size, data, usage) 
	//実際の頂点データをVBOにアップロードする
	//GL_DYNAMIC__DRAW=データが頻繁に更新されるよ
        glBufferData(GL_ARRAY_BUFFER, initialBufferSize, nullptr, currentBufferHint);
    }
    // else: 描画時に毎回glBufferDataでデータストアを確保する


    // VAOの頂点属性の設定 (位置情報 - layout (location = 0) in vec2 aPos;)
    // stride: 1頂点あたりのバイト数 (x, y なので float 2つ分)
    // offset: 頂点データの先頭からのオフセット (最初は0)
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0); // VBOのバインド解除
    glBindVertexArray(0);             // VAOのバインド解除

    std::cout << "Renderer initialized successfully." << std::endl;
    return true;
}

//頂点データやシェーダーを受け取り、描画する
void Renderer::draw(PrimitiveType type, const std::vector<float>& vertices, const Shader& shader, float pointSize) {
    if (vertices.empty() || VAO == 0 || VBO == 0) {
        return; // 描画データがない、または初期化されていない
    }

    shader.use(); // シェーダープログラムを使用

    glBindVertexArray(VAO);//バインド
    glBindBuffer(GL_ARRAY_BUFFER, VBO);//バインド

    // VBOにデータを送信
    GLsizeiptr requiredSize = vertices.size() * sizeof(float);
    if (requiredSize > currentBufferSize || currentBufferHint != GL_STREAM_DRAW) { // STREAM_DRAWは毎回再確保が良い場合も
        // 必要なサイズが現在のバッファサイズを超えるか、毎回データを送り直すようなヒントの場合
        // (より洗練されたバッファ管理も可能だが、ここではシンプルに再確保)
        glBufferData(GL_ARRAY_BUFFER, requiredSize, vertices.data(), currentBufferHint);
        currentBufferSize = requiredSize; // バッファサイズを更新 (glBufferDataの場合)
    } else {
        // 既存のバッファにデータを部分的に上書き
        glBufferSubData(GL_ARRAY_BUFFER, 0, requiredSize, vertices.data());
    }

    GLenum glPrimitiveType;
    switch (type) {
        case PrimitiveType::POINTS:
            glPrimitiveType = GL_POINTS;
            glPointSize(pointSize); // 点の場合のみサイズ設定
            break;
        case PrimitiveType::LINES:
            glPrimitiveType = GL_LINES;
            break;
        case PrimitiveType::LINE_STRIP:
            glPrimitiveType = GL_LINE_STRIP;
            break;
        case PrimitiveType::TRIANGLES:
            glPrimitiveType = GL_TRIANGLES;
            break;
        default:
            std::cerr << "ERROR::RENDERER::Unknown primitive type!" << std::endl;
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindVertexArray(0);
            return;
    }

    glDrawArrays(glPrimitiveType, 0, static_cast<GLsizei>(vertices.size() / 2)); // 頂点数は (x,y) のペアなので /2

    glBindBuffer(GL_ARRAY_BUFFER, 0); // 必須ではないが、良い習慣として
    glBindVertexArray(0);
}


void Renderer::cleanup() {
    if (VBO != 0) {
        glDeleteBuffers(1, &VBO);
        VBO = 0;
    }
    if (VAO != 0) {
        glDeleteVertexArrays(1, &VAO);
        VAO = 0;
    }
    std::cout << "Renderer cleaned up." << std::endl;
}
