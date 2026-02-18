project(Core LANGUAGES CXX)

# Criar a biblioteca Core
# SHARED cria um .dll/.so, STATIC cria um .lib/.a
add_library(Core SHARED 
    geometria.h 
    geometria.cpp
    massadados.h
    geohl.h
)

# Permitir que outros projetos encontrem os headers desta pasta
target_include_directories(Core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Linkar dependências do subprojeto (ex: Qt ou IO)
target_link_libraries(Core PRIVATE Qt6::Core IO)
