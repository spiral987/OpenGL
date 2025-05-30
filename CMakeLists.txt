cmake_minimum_required(VERSION 4.0.2) # CMakeの最低バージョンを指定

project(OpenGLDraw VERSION 1.0 LANGUAGES CXX C) # プロジェクト名とバージョン、言語

# C++標準の指定 (C++17を推奨しますが、必要に応じて変更)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)


# GLFWをFetchContentで取得
include(FetchContent)

FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG        3.3.8  # 好きなバージョンに変更可能
)
FetchContent_MakeAvailable(glfw)

# 実行ファイル名
set(EXECUTABLE_NAME OpenGLtest)




# ソースファイル
file(GLOB SOURCES
    "OpenGLtest/src/*.cpp"
    "OpenGLtest/src/*.c"
    "imgui/*.cpp"
    "imgui/backends/*.cpp"
)

add_executable(${EXECUTABLE_NAME} ${SOURCES})


add_custom_command(TARGET ${EXECUTABLE_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_CURRENT_SOURCE_DIR}/OpenGLtest/src/shader.vert" # CMakeLists.txtから見たシェーダーのパス
        $<TARGET_FILE_DIR:${EXECUTABLE_NAME}>/shader.vert
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_CURRENT_SOURCE_DIR}/OpenGLtest/src/shader.frag" # CMakeLists.txtから見たシェーダーのパス
        $<TARGET_FILE_DIR:${EXECUTABLE_NAME}>/shader.frag
    COMMENT "Copying shaders to build directory"
)

# インクルードディレクトリ
target_include_directories(${EXECUTABLE_NAME} PRIVATE
    "Linking/include"  # gladやKHR, glmのヘッダなど
    "imgui"
    "imgui/backends"
    "OpenGLtest/src"
)

# GLFWとOpenGLをリンク
target_link_libraries(${EXECUTABLE_NAME}
    PRIVATE 
        glfw       # FetchContentで利用可能になったglfwターゲット
        opengl32   # Windowsの場合
)

