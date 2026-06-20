cmake_minimum_required(VERSION 3.5)

project(astra_developer_test_task VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Gui Widgets)

set(PROJECT_SOURCES
        src/main.cpp
        src/main_window/main_window.cpp
        src/main_window/main_window.h
		src/custom_system_model_delegate/custom_system_model_delegate.cpp
		src/custom_system_model_delegate/custom_system_model_delegate.h
        src/custom_system_model/custom_system_model.h
        src/custom_system_model/custom_system_model.cpp
		src/custom_sort_filter_proxy_model/custom_sort_filter_proxy_model.h
		src/custom_sort_filter_proxy_model/custom_sort_filter_proxy_model.cpp
)


add_executable(astra_developer_test_task
	${PROJECT_SOURCES}
)

target_link_libraries(astra_developer_test_task PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
