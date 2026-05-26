cmake_minimum_required(VERSION 3.22.1)
project("firstgameproject")

# C++ стандарт
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Все исходники
add_library(${CMAKE_PROJECT_NAME} SHARED
        jni/jni_bridge.cpp
        platform/egl_manager.cpp
        render/fgp_render.cpp
        render/engine/shader/fgp_shader.cpp
        render/engine/ebo/fgp_ebo.cpp
        render/engine/vao/fgp_vao.cpp
        render/engine/vbo/fgp_vbo.cpp
        render/engine/model/fgp_model.cpp
        render/engine/model/fgp_primitive.cpp
        render/engine/texture/fgp_texture.cpp
        render/engine/model/fgp_meshdata.cpp
        render/engine/math/fgp_math.cpp
        render/engine/camera/fgp_camera.cpp
        game/game_loop.cpp
        includes/tiny_gltf_impl.cpp
)

# Include paths
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
        ${CMAKE_SOURCE_DIR}/includes
        ${CMAKE_SOURCE_DIR}/platform
        ${CMAKE_SOURCE_DIR}/render
        ${CMAKE_SOURCE_DIR}/render/engine
        ${CMAKE_SOURCE_DIR}/game
        ${CMAKE_SOURCE_DIR}/jni
        ${CMAKE_SOURCE_DIR}/models
)

# Системные библиотеки Android
find_library(log-lib log)
find_library(android-lib android)
find_library(EGL-lib EGL)
find_library(GLESv3-lib GLESv3)

target_link_libraries(${CMAKE_PROJECT_NAME}
        ${log-lib}
        ${android-lib}
        ${EGL-lib}
        ${GLESv3-lib}
)