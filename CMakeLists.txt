cmake_minimum_required(VERSION 3.19)
project(Malha LANGUAGES CXX)
# Adicione ou verifique estas linhas no seu CMakeLists.txt
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets LinguistTools)

qt_standard_project_setup()

qt_add_executable(Malha
    WIN32 MACOSX_BUNDLE
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    Core/geometria.h
)
# Verifica se está usando GCC ou Clang (MinGW se encaixa aqui)
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_compile_options(${PROJECT_NAME} PRIVATE -mbmi2)
endif()

# Se usar Visual Studio (MSVC) no futuro
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /arch:AVX2)
endif()

qt_add_translations(
    TARGETS Malha
    TS_FILES Malha_pt_BR.ts
)

target_link_libraries(Malha
    PRIVATE
        Qt::Core
        Qt::Widgets
)

include(GNUInstallDirs)

install(TARGETS Malha
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET Malha
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
add_subdirectory(Core)
add_subdirectory(IO)
