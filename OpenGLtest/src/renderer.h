#ifndef RENDERER_H
#define RENDERER_H

#include <vector>
#include <glad/glad.h>
// #include "Shader.h" // �V�F�[�_�[�N���X���g���Ȃ�C���N���[�h

class Renderer {
public:
    Renderer();
    ~Renderer();

    void setupTriangleDrawing(); // VAO/VBO�̏����Z�b�g�A�b�v�p
    void setupFreehandDrawing(); // (����VAO/VBO�𕪂���Ȃ�)

    // �`��֐� (�`�惂�[�h��f�[�^�ɉ�����)
    void drawTriangle(const std::vector<float>& vertices, int vertexCount, const Shader& shader);
    void drawFreehandLine(const std::vector<float>& vertices, const Shader& shader);
    void drawPoint(const std::vector<float>& vertices, float pointSize, const Shader& shader);
    void drawLines(const std::vector<float>& vertices, const Shader& shader);


private:
    unsigned int VAO_triangle, VBO_triangle;
    unsigned int VAO_freehand, VBO_freehand; // ���[�h���Ƃ�VAO/VBO�𕪂���ꍇ
    // ����VAO/VBO�����L����Ȃ� unsigned int genericVAO, genericVBO;
};
#endif