cmake_minimum_required(VERSION 3.0)

project(telegram_0_calories VERSION 0.0.0 LANGUAGES CXX)

set(TD_ENABLE_JNI OFF CACHE BOOL "turn off java api" FORCE)
set(TD_ENABLE_DOTNET OFF CACHE BOOL "turn off C# api" FORCE)
add_subdirectory(td)

option(TZC_USE_SYSTEM_WXWIDGETS "Use system wxWidgets" ON)
if(TZC_USE_SYSTEM_WXWIDGETS)
	set(wxWidgets_EXCLUDE_COMMON_LIBRARIES TRUE)
	find_package(wxWidgets COMPONENTS core base REQUIRED)
	if(NOT ${wxWidgets_FOUND})
    	message(FATAL_ERROR "wxWidgets are not found!")
	endif()
	message(STATUS "wxWidgets_CXX FLAGS: ${wxWidgets_CXX_FLAGS}")
	message(STATUS "wxWidgets_DEFINITIONS: ${wxWidgets_DEFINITIONS}")
	message(STATUS "wxWidgets_LIBRARIES: ${wxWidgets_LIBRARIES}")
else()
	add_subdirectory(wxWidgets)
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(HEADERS login_window.hpp
            pass_code_window.hpp
            password_window.hpp
            tg_io.hpp
            show_login_window_event.hpp
            show_password_window_event.hpp
            show_pass_code_window_event.hpp
            queued_executor.hpp
            main_window.hpp
            show_main_window_event.hpp)
set(SOURCES main.cpp
            login_window.cpp
            pass_code_window.cpp
            password_window.cpp
            tg_io.cpp
            show_login_window_event.cpp
            show_password_window_event.cpp
            show_pass_code_window_event.cpp
            queued_executor.cpp
            main_window.cpp
            show_main_window_event.cpp)

add_executable(${CMAKE_PROJECT_NAME} ${HEADERS} ${SOURCES})
set_property(TARGET ${CMAKE_PROJECT_NAME} PROPERTY CXX_STANDARD 20)
add_compile_definitions(${wxWidgets_DEFINITIONS})
include_directories(${wxWidgets_INCLUDE_DIRS})
target_link_directories(${CMAKE_PROJECT_NAME} PRIVATE ${wxWidgets_LIBRARY_DIRS})
target_link_libraries(${CMAKE_PROJECT_NAME} ${wxWidgets_LIBRARIES} tdclient)
