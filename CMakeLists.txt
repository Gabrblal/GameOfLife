project(GameOfLife)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY bin)

# Setup compilers
# cmake -G "MinGW Makefiles" -D CMAKE_C_COMPILER=gcc CMAKE_CXX_COMPILER=g++ .

# Get SFML
set(SFML_STATIC_LIBRARIES TRUE)
set(SFML_DIR ${CMAKE_SOURCE_DIR}/lib/SFML/lib/cmake/SFML)
find_package(SFML 2 COMPONENTS system network graphics window audio REQUIRED)

file(GLOB SOURCE ${CMAKE_SOURCE_DIR}/src/*.cpp)
set(SOURCE_INCLUDE ${CMAKE_SOURCE_DIR}/src)
set(SFML_INCLUDE ${CMAKE_SOURCE_DIR}/lib/include)

add_executable(executable ${SOURCE})

target_link_libraries(
    executable 
    sfml-graphics
)

target_include_directories(
    executable 
    PUBLIC 
    ${SOURCE_INCLUDE} 
    ${SFML_INCLUDE}
)

target_compile_options(executable 
    PUBLIC -g -std=c++17 -Wall -Werror
)