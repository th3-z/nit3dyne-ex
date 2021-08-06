cmake_minimum_required(VERSION 3.12.4)

add_executable(
    basic
    src/basic.cpp
)
target_link_libraries(basic PRIVATE nit3dyne)

add_executable(
    skinning
    src/skinning.cpp
)
target_link_libraries(skinning PRIVATE nit3dyne)

add_executable(
        colored
        src/colored.cpp
)
target_link_libraries(colored PRIVATE nit3dyne)

add_executable(
        billboard
        src/billboard.cpp
)
target_link_libraries(billboard PRIVATE nit3dyne)

add_executable(
    lines_transforms
    src/lines_transforms.cpp
)
target_link_libraries(lines_transforms PRIVATE nit3dyne)

add_executable(
    environment
    src/environment.cpp
)
target_link_libraries(environment PRIVATE nit3dyne)

add_executable(
        fps_scene
        src/fps_scene.cpp
)
target_link_libraries(fps_scene PRIVATE nit3dyne)

add_custom_target(nit3dyne-ex ALL)
# Copy application resources and shaders
add_custom_command(TARGET nit3dyne-ex POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/res
    ${CMAKE_CURRENT_BINARY_DIR}/res
)
add_custom_command(TARGET nit3dyne-ex POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/shaders
    ${CMAKE_CURRENT_BINARY_DIR}/shaders
)

# Copy built-in resources and shaders
add_custom_command(TARGET nit3dyne-ex POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    $<TARGET_FILE_DIR:nit3dyne>/res
    ${CMAKE_CURRENT_BINARY_DIR}/res
)
add_custom_command(TARGET nit3dyne-ex POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    $<TARGET_FILE_DIR:nit3dyne>/shaders
    ${CMAKE_CURRENT_BINARY_DIR}/shaders
)