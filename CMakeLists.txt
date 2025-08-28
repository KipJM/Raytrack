cmake_minimum_required(VERSION 4.0)
project(RaytracingWeekend)

set(CMAKE_CXX_STANDARD 23)


add_executable(RaytracingWeekend main.cpp
        vec3.h color.h
        ray.h
        hittable.h
        sphere.h
        hittable_list.h
        rtweekend.h
        interval.h
        camera.h
        material.h
        disk.h)
target_link_libraries(RaytracingWeekend)
