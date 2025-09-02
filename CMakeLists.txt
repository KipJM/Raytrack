cmake_minimum_required(VERSION 4.0)
project(RaytracingWeekend)

set(CMAKE_CXX_STANDARD 23)


add_executable(RaytracingWeekend main.cpp
        vec3.h color.h
        ray.h
        hittable.h
        primitives/geometry/sphere.h
        hittable_list.h
        misc.h
        interval.h
        camera.h
        material.h
        primitives/geometry/disk.h
        aabb.h
        bvh.h
        primitives/materials/basic_materials.h
        texture.h
        primitives/textures/tex_color.h
        primitives/textures/tex_checker.h
        scene.h)
target_link_libraries(RaytracingWeekend)
