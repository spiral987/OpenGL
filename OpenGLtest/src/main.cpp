#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <vector>

const int MAX_VERTICES_PER_TRIANGLE = 3;
std::vector<float> triangleVertices; // クリックされた頂点の座標 (x, y, x, y, ...)
int currentVertexCount = 0;

//ウインドウサイズ
extern const unsigned int SCR_WIDTH = 800; // 例: 800
extern const unsigned int SCR_HEIGHT = 600; // 例: 600

unsigned int VAO, VBO;



void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

//コールバック関数
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

int main() {
	std::cout << "Hello World!" << std::endl;
	
	// GLFW初期化
	if (!glfwInit()) {
		std::cerr << "Failed to initialize GLFW" << std::endl;
		return -1;
	}

	glfwInit();

	// openGL version 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // コアプロファイル

	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "2D Drawing App", NULL, NULL);
	if (window == NULL) {//window not created
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1; //終了処理
	}
	glfwMakeContextCurrent(window);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// glad初期化: OpenGL関数ポインタをロード
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cerr << "Failed to initialize GLAD" << std::endl;
		glfwTerminate();
		return -1;
	}

	// ... (gladLoadGLLoader の後)
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO); // VAOをバインド

	glBindBuffer(GL_ARRAY_BUFFER, VBO); // VBOをバインド
	// 最初は十分なサイズを確保しておくか、描画時にデータを送る
	// ここでは、最大3頂点 * 2座標 (x,y) 分のfloatを確保
	glBufferData(GL_ARRAY_BUFFER, MAX_VERTICES_PER_TRIANGLE * 2 * sizeof(float), nullptr, GL_DYNAMIC_DRAW);

	// 頂点属性の設定 (位置情報)
	// layout (location = 0) に対応
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0); // VBOのバインド解除
	glBindVertexArray(0); // VAOのバインド解除

	// ... (描画ループ内)
	while (!glfwWindowShouldClose(window)) {
		// 入力処理 (例: ESCキーで閉じるなど)
		processInput(window); // processInput関数は別途定義されていると仮定

		// レンダリングコマンド
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// シェーダープログラムを使用
		glUseProgram(shaderProgram); // shaderProgram は事前に作成したシェーダープログラムのID

		// 頂点データが更新されたらVBOに再送信
		if (!triangleVertices.empty()) {
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			// VBOの先頭からデータを更新
			glBufferSubData(GL_ARRAY_BUFFER, 0, triangleVertices.size() * sizeof(float), triangleVertices.data());
			glBindBuffer(GL_ARRAY_BUFFER, 0); // 安全のためアンバインド
		}

		glBindVertexArray(VAO); // 描画に使うVAOをバインド

		if (currentVertexCount == 1) {
			// 点を描画 (オプション)
			glPointSize(10.0f); // 点の大きさを設定
			glDrawArrays(GL_POINTS, 0, 1);
		}
		else if (currentVertexCount == 2) {
			// 線を描画 (オプション、プレビュー用)
			glDrawArrays(GL_LINES, 0, 2);
		}
		else if (currentVertexCount == MAX_VERTICES_PER_TRIANGLE) {
			// 三角形を描画
			glDrawArrays(GL_TRIANGLES, 0, MAX_VERTICES_PER_TRIANGLE);

			// 描画後、次の三角形のためにリセット
			triangleVertices.clear();
			currentVertexCount = 0;
			std::cout << "Triangle drawn. Ready for next triangle." << std::endl;
		}

		glBindVertexArray(0); // VAOのバインド解除

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	
	
	glfwTerminate();


	return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

// 入力処理関数 (ESCキーでウィンドウを閉じる例)
void processInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

//クリックされた座標を取得し、頂点データとして保存する
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
	//マウス右クリックの時
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		if (currentVertexCount < MAX_VERTICES_PER_TRIANGLE) {
			double xpos, ypos;
			glfwGetCursorPos(window, &xpos, &ypos); // ウィンドウ座標を取得

			// ウィンドウ座標をOpenGLの正規化デバイス座標 (-1.0 to 1.0) に変換
			float ndcX = (float)(xpos / SCR_WIDTH) * 2.0f - 1.0f;
			float ndcY = 1.0f - (float)(ypos / SCR_HEIGHT) * 2.0f; // Y軸は反転

			triangleVertices.push_back(ndcX);
			triangleVertices.push_back(ndcY);
			//頂点クリック数カウント++
			currentVertexCount++;

			std::cout << "Vertex " << currentVertexCount << " added at: (" << ndcX << ", " << ndcY << ")" << std::endl;

			// (VBO更新処理は描画ループかここで行う - 後述)
		}
	}
}
