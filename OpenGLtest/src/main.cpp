#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

const int MAX_VERTICES_PER_TRIANGLE = 3;
std::vector<float> triangleVertices; // クリックされた頂点の座標 (x, y, x, y, ...)
int currentVertexCount = 0;

//ウインドウサイズ
unsigned int SCR_WIDTH = 800; // 例: 800
unsigned int SCR_HEIGHT = 600; // 例: 600

//将来的にこれらを管理するクラスを作成
unsigned int VAO, VBO;

unsigned int shaderProgram;


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

//コールバック関数
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

//シェーダーを読み込んで文字列として返す
std::string loadShaderSource(const char* filePath);

//shaderをコンパイルする関数
unsigned int compileShader(unsigned int type, const char* source);

//頂点シェーダーとフラグメントシェーダーをリンクしてシェーダープログラムを作成する
unsigned int createShaderProgram(const char* vertexPath, const char* fragmentPath);

int main() {
	std::cout << "Hello World!" << std::endl;
	
	// GLFW初期化
	if (!glfwInit()) {
		std::cerr << "Failed to initialize GLFW" << std::endl;
		return -1;
	}


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

	// シェーダープログラムの作成
	shaderProgram = createShaderProgram("src/shader.vert", "src/shader.frag"); // ファイルパスを指定
	if (shaderProgram == 0) {
		// シェーダー作成失敗時の処理 (例えばプログラム終了)
		glfwTerminate();
		return -1;
	}


	// VAOとVBOを生成して設定
	/*
		VBOは頂点情報を格納するGPB側のバッファ(メモリ領域)
		VAOはバッファオブジェクトの属性をまとめ、シェーダーへ橋渡しする

		1.頂点データを準備
		2.VBOに頂点データを格納
		3.VAOにVBOをまとめる
		4.シェーダーを適用し、ドローコールで描画
	*/
	glGenVertexArrays(1, &VAO);//VAOを生成
	glGenBuffers(1, &VBO);//VBOを生成

	glBindVertexArray(VAO); // VAOをバインド(選択)

	glBindBuffer(GL_ARRAY_BUFFER, VBO); // VBOをバインド(選択)

	//glBufferData(GL_ARRAY_BUFFER, size, data, usage) 
	//実際の頂点データをVBOにアップロードする
	//GL_DYNAMIC__DRAW=データが頻繁に更新されるよ
	glBufferData(GL_ARRAY_BUFFER, MAX_VERTICES_PER_TRIANGLE * 2 * sizeof(float), nullptr, GL_DYNAMIC_DRAW);

	// 頂点属性の設定 (位置情報)
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
			glPointSize(4.0f); // 点の大きさを設定
			glDrawArrays(GL_POINTS, 0, 1);
		}
		else if (currentVertexCount == 2) {
			// 線を描画 (オプション、プレビュー用)
			glDrawArrays(GL_LINES, 0, 2);
		}
		else if (currentVertexCount == MAX_VERTICES_PER_TRIANGLE) {
			// 三角形を描画

			glDrawArrays(GL_TRIANGLES, 0, MAX_VERTICES_PER_TRIANGLE);
		}

		glBindVertexArray(0); // VAOのバインド解除

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteProgram(shaderProgram); // シェーダープログラムを削除
	
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
		if (currentVertexCount == MAX_VERTICES_PER_TRIANGLE) {
			// 既に1つの三角形を完成してる場合 → 新しい三角形を描く準備
			triangleVertices.clear();
			currentVertexCount = 0;
			std::cout << "Triangle drawn. Ready for next triangle." << std::endl;
		}
		if (currentVertexCount < MAX_VERTICES_PER_TRIANGLE) {
			double xpos, ypos;
			glfwGetCursorPos(window, &xpos, &ypos); // ウィンドウ座標を取得

			// ウィンドウ座標をOpenGLの正規化デバイス座標 (-1.0 to 1.0) に変換
			//ウインドウ座標系では左上が原点なため、Y軸が反転する。
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

// シェーダーファイルを読み込んで文字列として返す関数
std::string loadShaderSource(const char* filePath) {
	std::ifstream shaderFile;
	std::stringstream shaderStream;

	// ファイルを開く
	shaderFile.open(filePath);
	if (shaderFile.is_open()) {
		// ファイルの内容をストリームに読み込む
		shaderStream << shaderFile.rdbuf();
		shaderFile.close();
		// ストリームから文字列に変換
		return shaderStream.str();
	}
	else {
		std::cerr << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ: " << filePath << std::endl;
		return ""; // 空文字を返す
	}
}

// シェーダーをコンパイルするヘルパー関数
unsigned int compileShader(unsigned int type, const char* source) {
	unsigned int id = glCreateShader(type); // シェーダーオブジェクト作成
	glShaderSource(id, 1, &source, nullptr); // シェーダーソースをセット
	glCompileShader(id); // コンパイル

	// コンパイルエラーチェック
	int success;
	char infoLog[512];
	glGetShaderiv(id, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(id, 512, nullptr, infoLog);
		std::cerr << "ERROR::SHADER::" << (type == GL_VERTEX_SHADER ? "VERTEX" : "FRAGMENT") << "::COMPILATION_FAILED\n" << infoLog << std::endl;
		glDeleteShader(id); // 不要になったシェーダーオブジェクトを削除
		return 0;
	}
	return id;
}

// 頂点シェーダーとフラグメントシェーダーをリンクしてシェーダープログラムを作成する関数
unsigned int createShaderProgram(const char* vertexPath, const char* fragmentPath) {
	// 1. シェーダーソースをファイルから読み込む
	std::string vertexCodeStr = loadShaderSource(vertexPath);
	std::string fragmentCodeStr = loadShaderSource(fragmentPath);
	if (vertexCodeStr.empty() || fragmentCodeStr.empty()) {
		return 0; // 読み込み失敗
	}
	const char* vertexShaderSource = vertexCodeStr.c_str();
	const char* fragmentShaderSource = fragmentCodeStr.c_str();

	// 2. 頂点シェーダーをコンパイル
	unsigned int vertexShader = compileShader(GL_VERTEX_SHADER, vertexShaderSource);
	if (vertexShader == 0) return 0;

	// 3. フラグメントシェーダーをコンパイル
	unsigned int fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);
	if (fragmentShader == 0) {
		glDeleteShader(vertexShader); // 頂点シェーダーを削除
		return 0;
	}

	// 4. シェーダープログラムを作成し、シェーダーをリンク
	unsigned int program = glCreateProgram(); // プログラムオブジェクト作成
	glAttachShader(program, vertexShader);     // 頂点シェーダーをアタッチ
	glAttachShader(program, fragmentShader);   // フラグメントシェーダーをアタッチ
	glLinkProgram(program);                    // リンク

	// リンクエラーチェック
	int success;
	char infoLog[512];
	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(program, 512, nullptr, infoLog);
		std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
		glDeleteShader(vertexShader);   // リンク後には不要なので削除
		glDeleteShader(fragmentShader); // 同上
		glDeleteProgram(program);       // プログラムも削除
		return 0;
	}

	// リンクが成功したら、個々のシェーダーオブジェクトは不要になるので削除
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	return program; // 作成したシェーダープログラムのIDを返す
}


