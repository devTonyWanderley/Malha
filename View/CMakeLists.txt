# 1. Versão mínima (exigido pelo CMake)
cmake_minimum_required(VERSION 3.16)

# 2. Nome do projeto
project(View LANGUAGES CXX)

# 3. Configurações essenciais para o Qt
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 4. Localiza o Qt (Widgets contém o QGraphicsView)
find_package(Qt6 REQUIRED COMPONENTS Widgets)

# 5. Define a biblioteca e lista os arquivos (Adicione wcad.h/cpp e painelCad.h/cpp)
add_library(View SHARED
    wcad.h
    wcad.cpp
)

# 6. Vincula o Qt à biblioteca (Isso resolve o erro do QGraphicsView)
target_link_libraries(View PRIVATE Qt6::Widgets)

# 7. Inclui o próprio diretório para que os arquivos se achem
target_include_directories(View PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
