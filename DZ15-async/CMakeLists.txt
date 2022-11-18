project(3D_Cube C)

find_package(GLUT REQUIRED)
find_package(GLEW REQUIRED)
find_package(OpenGL REQUIRED)

add_executable(3D_Cube main.c)
target_link_libraries(3D_Cube PRIVATE GLUT::GLUT GLEW::GLEW OpenGL::GL)
