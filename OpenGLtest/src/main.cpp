#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#include <shader.h>




// --------------- グローバル変数 ----------------
const int MAX_VERTICES_PER_TRIANGLE = 3;
//頂点データを管理するベクタ
std::vector<float> triangleVertices;
std::vector<float> freehandVertices;
int currentVertexCount = 0; // 三角形描画時の頂点カウント

unsigned int SCR_WIDTH = 800;
unsigned int SCR_HEIGHT = 600;

unsigned int VAO, VBO; // 描画オブジェクト (今回は共有)

bool isMouseButtonDown = false; // マウスボタンが押されているか


//---------------------描画モード------------------------
//enum:列挙型
enum class DrawingMode {
	TRIANGLE,
	FREEHAND
};
DrawingMode currentMode = DrawingMode::TRIANGLE;

//---------------------コールバック関数のプロトタイプ宣言-----------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
//マウスクリック時のコールバック
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
// マウス移動時のコールバック
void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
//キーが押されたときのコールバック
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods); // 新設


//-----------------シェーダー関連---------------------
Shader* mainShader = nullptr; // ポインタとして宣言 (またはShaderオブジェクト)


int main() {
	std::cout << "init" << std::endl;
	
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

	//コールバック関数の登録
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetCursorPosCallback(window, cursor_position_callback);
	glfwSetKeyCallback(window, key_callback); // キーコールバックを登録

	// glad初期化: OpenGL関数ポインタをロード
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cerr << "Failed to initialize GLAD" << std::endl;
		glfwTerminate();
		return -1;
	}

	// --- ImGuiのセットアップ ---
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	// io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // キーボードナビゲーションが必要な場合
	// io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // ゲームパッドナビゲーションが必要な場合

	// ImGuiのスタイル設定 (任意)
	ImGui::StyleColorsDark();
	// ImGui::StyleColorsClassic();

	// プラットフォーム/レンダラーバックエンドの初期化
	ImGui_ImplGlfw_InitForOpenGL(window, true); // 'true' はコールバックをインストールするかどうか
	ImGui_ImplOpenGL3_Init("#version 330 core"); // GLSLのバージョン文字列


	// シェーダープログラムの作成
	//相対パスにしたいけどできない…
	mainShader = new Shader("shader.vert", "shader.frag");  // ファイルパスを指定
	if (mainShader == nullptr || mainShader->ID == 0) { // エラーチェック
		std::cerr << "Failed to create shader program." << std::endl;
		glfwTerminate();
		return -1;
	}

	// ... (描画ループ内)
	while (!glfwWindowShouldClose(window)) {
		// 入力処理 (例: ESCキーで閉じるなど)
		processInput(window); // processInput関数は別途定義されていると仮定

		glfwPollEvents(); // ImGui_ImplGlfw_NewFrame の前にイベント処理

		// --- ImGuiの新しいフレームを開始 ---
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		// --- ここにImGuiのUI要素を記述 ---
		// 画面左上に現在のモードを表示
		ImGui::SetNextWindowPos(ImVec2(10, 10)); // 表示位置 (左上から10,10ピクセル)
		ImGui::Begin("Current Mode", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav);
		if (currentMode == DrawingMode::TRIANGLE) {
			ImGui::Text("Mode: Triangle");
		}
		else if (currentMode == DrawingMode::FREEHAND) {
			ImGui::Text("Mode: Freehand");
		}
		ImGui::End();
		// --- ImGuiのUI要素記述ここまで ---
		

		// レンダリングコマンド
		glClearColor(0.9f, 0.9f, 0.9f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// シェーダープログラムを使用
		mainShader->use();        // ← 変更後

		glBindVertexArray(VAO); // 描画に使うVAOをバインド

		if (currentMode == DrawingMode::TRIANGLE) {
			// --- 三角形描画モードの処理 ---
			if (!triangleVertices.empty()) {
				glBindBuffer(GL_ARRAY_BUFFER, VBO);
				glBufferSubData(GL_ARRAY_BUFFER, 0, triangleVertices.size() * sizeof(float), triangleVertices.data());
				// VBOをアンバインドしない（VAOがVBOの参照を保持しているため、描画直前までバインドされている想定でOK）
			}

			if (currentVertexCount == 1) {
				glPointSize(4.0f);
				glDrawArrays(GL_POINTS, 0, 1);
			}
			else if (currentVertexCount == 2) {
				glDrawArrays(GL_LINES, 0, 2);
			}
			else if (currentVertexCount == MAX_VERTICES_PER_TRIANGLE) {
				glDrawArrays(GL_TRIANGLES, 0, MAX_VERTICES_PER_TRIANGLE);
				// 三角形描画後のリセットはmouse_button_callbackで行うように変更済み
			}
		}
		else if (currentMode == DrawingMode::FREEHAND) {
			// --- 自由線描画モードの処理 ---
			if (!freehandVertices.empty() && freehandVertices.size() >= 4) { // 少なくとも2点(4float)ないと線は描けない
				glBindBuffer(GL_ARRAY_BUFFER, VBO);
				glBufferSubData(GL_ARRAY_BUFFER, 0, freehandVertices.size() * sizeof(float), freehandVertices.data());

				//GL_LINE_STRIPでは頂点を結ぶ直線を描く
				glDrawArrays(GL_LINE_STRIP, 0, freehandVertices.size() / 2);
			}
		}

		glBindVertexArray(0); // VAOのバインド解除


		// --- ImGuiの描画 ---
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		// --- ImGuiの描画ここまで ---

		glfwSwapBuffers(window);

	}

	// --- ImGuiのクリーンアップ ---
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	// --- ImGuiのクリーンアップここまで ---


	glDeleteVertexArrays(1, &VAO); // VAOの削除
	glDeleteBuffers(1, &VBO);   // VBOの削除
	//シェーダーの削除
	delete mainShader; // ポインタの場合 (オブジェクトなら不要)
	mainShader = nullptr;
	glfwTerminate();


	return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
	SCR_WIDTH = width;  // ウィンドウリサイズ時にグローバル変数も更新
	SCR_HEIGHT = height;
}

// 入力処理関数 (ESCキーでウィンドウを閉じる例)
void processInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

// --- キー入力コールバック関数 ---
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_M && action == GLFW_PRESS) {
		if (currentMode == DrawingMode::TRIANGLE) {
			currentMode = DrawingMode::FREEHAND;
			glfwSetWindowTitle(window, "2D Drawing App (Mode: FREEHAND)");
			std::cout << "Mode changed to: FREEHAND" << std::endl;
			// モード変更時に描画途中のデータをクリアする
			triangleVertices.clear();
			currentVertexCount = 0;
			freehandVertices.clear();
		}
		else {
			currentMode = DrawingMode::TRIANGLE;
			glfwSetWindowTitle(window, "2D Drawing App (Mode: TRIANGLE)");
			std::cout << "Mode changed to: TRIANGLE" << std::endl;
			// モード変更時に描画途中のデータをクリアする
			triangleVertices.clear();
			currentVertexCount = 0;
			freehandVertices.clear();
		}
	}
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            isMouseButtonDown = true;
            if (currentMode == DrawingMode::TRIANGLE) {
                if (currentVertexCount == MAX_VERTICES_PER_TRIANGLE) {
                    triangleVertices.clear();
                    currentVertexCount = 0;
                    std::cout << "Ready for next triangle." << std::endl;
                }
                // currentVertexCount < MAX_VERTICES_PER_TRIANGLE のチェックは不要になる（常に新しい頂点を追加）
                double xpos, ypos;
                glfwGetCursorPos(window, &xpos, &ypos);
				//ディスプレイ座標からOpenGLの座標系に
                float ndcX = (float)(xpos / SCR_WIDTH) * 2.0f - 1.0f;
                float ndcY = 1.0f - (float)(ypos / SCR_HEIGHT) * 2.0f;
				//頂点ベクタに格納
                triangleVertices.push_back(ndcX);
                triangleVertices.push_back(ndcY);
				//カウントアップ
                currentVertexCount++;
                std::cout << "Vertex " << currentVertexCount << " added for triangle at: (" << ndcX << ", " << ndcY << ")" << std::endl;

            } else if (currentMode == DrawingMode::FREEHAND) {
                freehandVertices.clear(); // 新しい線を描き始めるのでクリア
                // 最初の点を追加 (cursor_position_callbackでも追加されるので、ここでは不要かもしれない)
                // double xpos, ypos;
                // glfwGetCursorPos(window, &xpos, &ypos);
                // float ndcX = (float)(xpos / SCR_WIDTH) * 2.0f - 1.0f;
                // float ndcY = 1.0f - (float)(ypos / SCR_HEIGHT) * 2.0f;
                // freehandVertices.push_back(ndcX);
                // freehandVertices.push_back(ndcY);
                // std::cout << "Freehand drawing started at: (" << ndcX << ", " << ndcY << ")" << std::endl;
            }
        } else if (action == GLFW_RELEASE) {
            isMouseButtonDown = false;
            if (currentMode == DrawingMode::FREEHAND) {
                std::cout << "Freehand drawing ended. Points: " << freehandVertices.size() / 2 << std::endl;
                // ここでfreehandVerticesを永続的な描画リストに追加するなどの処理も考えられる
            }
        }
    }
}

//カーソル位置のコールバック
void cursor_position_callback(GLFWwindow* window, double xpos_window, double ypos_window) {
	if (isMouseButtonDown && currentMode == DrawingMode::FREEHAND) {
		float ndcX = (float)(xpos_window / SCR_WIDTH) * 2.0f - 1.0f;
		float ndcY = 1.0f - (float)(ypos_window / SCR_HEIGHT) * 2.0f;

		// 前の点と同じでなければ追加 (点の重複を避ける)
		if (freehandVertices.empty() ||
			freehandVertices[freehandVertices.size() - 2] != ndcX ||
			freehandVertices[freehandVertices.size() - 1] != ndcY)
		{
			freehandVertices.push_back(ndcX);
			freehandVertices.push_back(ndcY);
		}
	}
}

