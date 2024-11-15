file(
 GLOB_RECURSE CORE_INC CONFIGURE_DEPENDS
 RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
 *.h *.hpp
)

file(
 GLOB_RECURSE CORE_SRC CONFIGURE_DEPENDS
 RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
 *.c *.cpp
)

set(CORE_INC_DIR
 ${CMAKE_CURRENT_SOURCE_DIR}
 CACHE PATH "CORE INCLUDE SOURCE PATH"
)

add_library(core STATIC ${CORE_SRC} ${CORE_INC} "bl_library/shader.h" "bl_library/shader.cpp" "bl_library/texture.h" "bl_library/texture.cpp" "bl_library/mesh.h" "bl_library/mesh.cpp" "bl_library/model.h" "bl_library/model.cpp")

find_package(OpenGL REQUIRED)

target_link_libraries(core PUBLIC IMGUI glm assimp)

install (TARGETS core DESTINATION lib)
install (FILES ${CORE_INC} DESTINATION include/core)

