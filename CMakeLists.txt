cmake_minimum_required(VERSION 3.16)
project(Malha LANGUAGES CXX)

# Configurações Globais C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mbmi2")
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Encontrar o Qt
find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

# Adicionar os Subdiretórios (Onde o CMake buscará outros CMakeLists.txt)
add_subdirectory(Core)
add_subdirectory(IO)

# Criar o Executável Principal
add_executable(Malha main.cpp)

# Linkar com as bibliotecas que você criou nos subprojetos
target_link_libraries(Malha PRIVATE Core IO Qt6::Widgets)
