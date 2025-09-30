# For Github Actions, reduced from 4.0
cmake_minimum_required(VERSION 3.30)
project(RaytracingWeekend)

set(CMAKE_CXX_STANDARD 23)

if(WIN32) # Windows
set(CMAKE_EXE_LINKER_FLAGS "-static -static-libgcc -static-libstdc++")
ENDIF()

IF(APPLE) # AppleClang fixes taken from Internet, hopefully will work
    set(CMAKE_THREAD_LIBS_INIT "-lpthread")
    set(CMAKE_HAVE_THREADS_LIBRARY 1)
    set(CMAKE_USE_WIN32_THREADS_INIT 0)
    set(CMAKE_USE_PTHREADS_INIT 1)
    set(THREADS_PREFER_PTHREAD_FLAG ON)
ENDIF()

# glfw: disable tests
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

add_subdirectory(include/glfw-3.4)
include_directories(include)

set(IMGUI_SRC
        include/imgui/imconfig.h
        include/imgui/imgui.cpp
        include/imgui/imgui.h
        include/imgui/imgui_demo.cpp
        include/imgui/imgui_draw.cpp
        include/imgui/imgui_internal.h
        include/imgui/imgui_tables.cpp
        include/imgui/imgui_widgets.cpp
        include/imgui/imstb_rectpack.h
        include/imgui/imstb_textedit.h
        include/imgui/imstb_truetype.h

        include/imgui/backends/imgui_impl_glfw.h
        include/imgui/backends/imgui_impl_glfw.cpp

        include/imgui/backends/imgui_impl_opengl3.h
        include/imgui/backends/imgui_impl_opengl3.cpp
        include/imgui/misc/cpp/imgui_stdlib.h
        include/imgui/misc/cpp/imgui_stdlib.cpp
)

include_directories(include/imgui include/imgui/backends)

set(IMFILE_SRC
    include/im-file-dialog/ImFileDialog.cpp
    include/im-file-dialog/ImFileDialog.h)

add_executable(RaytracingWeekend
        ${IMGUI_SRC}
        ${IMFILE_SRC}
        window_main.cpp
#        main.cpp
        vec3.h color.h
        ray.h
        hittable.h
        primitives/geometry/geo_sphere.h
        hittable_list.h
        misc.h
        interval.h
        camera.h
        material.h
        primitives/geometry/geo_disk.h
        aabb.h
        bvh.h
        primitives/materials/mat_debug.h
        texture.h
        primitives/textures/tex_color.h
        primitives/textures/tex_checker.h
        scene.h
        include/stb_image.h
        rtw_image.h
        primitives/textures/tex_image.h
        primitives/textures/tex_uv_debug.h
        primitives/generator/perlin.h
        primitives/textures/tex_perlin.h
        primitives/geometry/geo_quad.h
        primitives/materials/mat_diffuse.h
        primitives/materials/mat_metallic.h
        primitives/materials/mat_translucent.h
        primitives/materials/mat_emissive.h
        primitives/geometry/geo_cube.h
        transformers.h
        volume_convex.h
        primitives/materials/mat_volumetric.h
        include/glad/glad.c
        viewport.h
        render_worker.h
        render_worker.cpp
        user_interface.h
        ui_components.h
        viewport.cpp
        hittable_list.cpp
        ui_components.cpp
        stb_img_include.h
        scene_presets.h
)
target_link_libraries(RaytracingWeekend PRIVATE glfw)

