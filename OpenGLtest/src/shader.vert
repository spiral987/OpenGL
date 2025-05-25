#version 330 core
// layout (location = 0) は、C++側で頂点属性を設定する際のインデックスに対応します。
// in vec2 aPos; は、2次元ベクトル型の頂点位置データを受け取ることを意味します。
layout (location = 0) in vec2 aPos;

void main() {
    // vec2のaPosをvec4に変換してgl_Positionに設定します。
    // 2D描画なので、z座標は0.0、w座標は1.0（通常のおまじない）とします。
    gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);
}