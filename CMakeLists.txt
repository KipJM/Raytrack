cmake_minimum_required(VERSION 4.0)
project(RaytracingWeekend)

set(CMAKE_CXX_STANDARD 23)


add_executable(RaytracingWeekend main.cpp
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
        primitives/geometry/geo_cube.h)
target_link_libraries(RaytracingWeekend)
