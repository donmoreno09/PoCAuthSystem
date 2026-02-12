cmake_minimum_required(VERSION 3.16)

project(PoCAuthSystem VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt6 6.8 REQUIRED COMPONENTS Quick Network)

qt_standard_project_setup(REQUIRES 6.8)

set(cpp_sources
    # Entities
    entities/IPersistable.h
    entities/AuthTokens.h
    entities/UserSession.h

    # Networking
    networking/ApiTypes.h
    networking/HttpClient.h
    networking/HttpClient.cpp
    networking/BaseApi.h
    networking/AuthApi.h
    networking/AuthApi.cpp
    networking/ApiEndpoints.h
    networking/ApiEndpoints.cpp

    # Auth
    auth/AuthState.h
    auth/AuthManager.h
    auth/AuthManager.cpp
    auth/SecureTokenStorage.h
    auth/SecureTokenStorage.cpp
    auth/PermissionManager.h
    auth/PermissionManager.cpp
)

set(qml_files
    qml/Main.qml
    qml/LoginPage.qml
)

qt_add_executable(PoCAuthSystem main.cpp)

qt_add_qml_module(PoCAuthSystem
    URI PoCAuthSystem
    VERSION 1.0
    QML_FILES ${qml_files}
    SOURCES ${cpp_sources}
    RESOURCE_PREFIX "/"
)

target_include_directories(PoCAuthSystem
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/networking
        ${CMAKE_CURRENT_SOURCE_DIR}/auth
)

target_link_libraries(PoCAuthSystem
    PRIVATE
        Qt6::Quick
        Qt6::Network
)

set_target_properties(PoCAuthSystem PROPERTIES
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)
