#ifndef RENDERER_H
#define RENDERER_H

#include <glad/glad.h>
#include <vector>
#include "Shader.h" // �쐬����Shader�N���X���g�p

// �`�惂�[�h��enum�������ɒ�`���邩�A�ʂ̋��ʃw�b�_�Ɉړ����Ă��ǂ�
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

    // VAO/VBO�̏����� (���_�����|�C���^�̐ݒ�Ȃ�)
    // bufferHint: GL_STATIC_DRAW, GL_DYNAMIC_DRAW �Ȃ�
    // initialBufferSize: VBO�ɍŏ��Ɋm�ۂ���T�C�Y (�o�C�g�P��)�B0�̏ꍇ�A�`�掞�ɖ���glBufferData�B
    bool initialize(GLenum bufferHint = GL_DYNAMIC_DRAW, GLsizeiptr initialBufferSize = 1000 * 2 * sizeof(float));

    // �`��f�[�^��VBO�ւ̑��M�ƕ`��̎��s
    // primitiveType: GL_POINTS, GL_LINES, GL_LINE_STRIP, GL_TRIANGLES
    // vertices: ���_�f�[�^ (x, y, x, y, ...)
    // vertexCount: �`�悷�钸�_�̐�
    void draw(PrimitiveType type, const std::vector<float>& vertices, const Shader& shader, float pointSize = 1.0f);

private:
    unsigned int VAO;
    unsigned int VBO;
    GLenum currentBufferHint;
    GLsizeiptr currentBufferSize; // ����VBO�Ɋm�ۂ���Ă���T�C�Y

    void cleanup(); // VAO/VBO�̍폜
};

#endif // RENDERER_H