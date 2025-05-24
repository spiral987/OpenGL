#version 330 core
// out vec4 FragColor; は、最終的なピクセルの色をvec4型（R, G, B, A）で出力することを意味します。
out vec4 FragColor;

void main() {
    // RGBA値を指定して色を設定します。各値は0.0から1.0の範囲です。
    // この例では、R=1.0, G=0.5, B=0.2, A=1.0 でオレンジ色になります。
    FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
}