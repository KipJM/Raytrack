cmake_minimum_required(VERSION 4.0)
project(RaytracingWeekend)

set(CMAKE_CXX_STANDARD 23)
# glfw: disable tests
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

add_subdirectory(glfw-3.4)
include_directories(include)

set(IMGUI_SRC
    imgui/imconfig.h
        imgui/imgui.cpp
        imgui/imgui.h
        imgui/imgui_demo.cpp
        imgui/imgui_draw.cpp
        imgui/imgui_internal.h
        imgui/imgui_tables.cpp
        imgui/imgui_widgets.cpp
        imgui/imstb_rectpack.h
        imgui/imstb_textedit.h
        imgui/imstb_truetype.h

        imgui/backends/imgui_impl_glfw.h
        imgui/backends/imgui_impl_glfw.cpp

        imgui/backends/imgui_impl_opengl3.h
        imgui/backends/imgui_impl_opengl3.cpp
        imgui/misc/cpp/imgui_stdlib.h
        imgui/misc/cpp/imgui_stdlib.cpp
)

include_directories(imgui imgui/backends)

add_executable(RaytracingWeekend
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
        external/stb_image.h
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
        glad.c
        ${IMGUI_SRC}
        viewport.h
        render_worker.h
        render_worker.cpp
        user_interface.h
        ui_components.h
        viewport.cpp
        hittable_list.cpp
        ui_components.cpp
)
target_link_libraries(RaytracingWeekend PRIVATE glfw)

