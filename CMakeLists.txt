cmake_minimum_required(VERSION 3.0)

project(telegram_0_calories VERSION 0.0.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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


add_executable(${CMAKE_PROJECT_NAME} main.cpp login_window.hpp login_window.cpp pass_code_window.hpp pass_code_window.cpp password_window.hpp password_window.cpp)
add_compile_definitions(${wxWidgets_DEFINITIONS})
include_directories(${wxWidgets_INCLUDE_DIRS})
target_link_directories(${CMAKE_PROJECT_NAME} PRIVATE ${wxWidgets_LIBRARY_DIRS})
target_link_libraries(${CMAKE_PROJECT_NAME} ${wxWidgets_LIBRARIES})