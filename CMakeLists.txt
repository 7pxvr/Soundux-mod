cmake_minimum_required(VERSION 3.10)
project(webview VERSION 0.2 DESCRIPTION "A cross-platform C++ webview library")
option(WINDOWS_8 "Replaces the call to SetProcessDpiAwarenessContext call with SetProcessDpiAwareness to support Windows 8.1" OFF)
option(USE_QT_WEBENGINE "Use Qt WebEngine instead of WebKitGTK on Linux when available" ON)

file(GLOB src
    "webview/src/*.cpp"
    "webview/src/*/*.cpp"
    "webview/src/*/*/*.cpp"
)

add_library(webview STATIC ${src})
target_sources(webview PRIVATE webview/include/core/linux/window.hpp)

if (WIN32)
    target_compile_options(webview PRIVATE /W4)
    
    find_program(NUGET_EXE NAMES nuget)
	if(NOT NUGET_EXE)
		message("NUGET.EXE not found.")
		message(FATAL_ERROR "Please install this executable, and run CMake again.")
	endif()

	exec_program(${NUGET_EXE}
    	ARGS install "Microsoft.Web.WebView2" -Version 1.0.865-prerelease -ExcludeVersion -OutputDirectory ${CMAKE_BINARY_DIR}/packages)
	exec_program(${NUGET_EXE}
    	ARGS install "Microsoft.Windows.ImplementationLibrary" -ExcludeVersion -OutputDirectory ${CMAKE_BINARY_DIR}/packages)

    if (WINDOWS_8)
        target_link_libraries(webview INTERFACE Shcore.lib)
        target_compile_definitions(webview PRIVATE WEBVIEWPP_WINDOWS_8=1)
    endif()

    target_link_libraries(webview INTERFACE Version.lib Shlwapi.lib ${CMAKE_BINARY_DIR}/packages/Microsoft.Web.WebView2/build/native/x64/WebView2LoaderStatic.lib ${CMAKE_BINARY_DIR}/packages/Microsoft.Windows.ImplementationLibrary/build/native/Microsoft.Windows.ImplementationLibrary.targets)
    target_include_directories(webview SYSTEM PUBLIC ${CMAKE_BINARY_DIR}/packages/Microsoft.Web.WebView2/build/native/include ${CMAKE_BINARY_DIR}/packages/Microsoft.Windows.ImplementationLibrary/include)
elseif(UNIX)
    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        target_compile_options(webview PRIVATE -Wall -Wextra -Werror -pedantic -Wno-unused-lambda-capture)
    endif()

    if (USE_QT_WEBENGINE)
        find_package(Qt6 COMPONENTS Widgets WebEngineWidgets WebChannel DBus QUIET)
    endif()

    if (Qt6_FOUND)
        set_target_properties(webview PROPERTIES AUTOMOC ON)
        target_compile_definitions(webview PUBLIC SOUNDUX_WEBVIEW_QT=1)
        target_link_libraries(webview PUBLIC Qt6::Widgets Qt6::WebEngineWidgets Qt6::WebChannel Qt6::DBus pthread)
        target_include_directories(webview SYSTEM PUBLIC ${PROJECT_SOURCE_DIR})
    else()
        find_package(PkgConfig REQUIRED)
	    pkg_check_modules(GTK3 REQUIRED gtk+-3.0)
	    pkg_check_modules(WEBKIT2 webkit2gtk-4.1)
	    if (NOT WEBKIT2_FOUND)
		    pkg_check_modules(WEBKIT2 REQUIRED webkit2gtk-4.0)
	    endif()

        target_link_libraries(webview INTERFACE ${GTK3_LIBRARIES} ${WEBKIT2_LIBRARIES} pthread)
        target_include_directories(webview SYSTEM PUBLIC ${GTK3_INCLUDE_DIRS} ${WEBKIT2_INCLUDE_DIRS} ${PROJECT_SOURCE_DIR})
    endif()
endif()

target_include_directories(webview SYSTEM PUBLIC "webview/include/")
target_include_directories(webview SYSTEM PUBLIC "lib/json/single_include/nlohmann")

target_compile_features(webview PRIVATE cxx_std_17)
set_target_properties(webview PROPERTIES 
                      CXX_STANDARD 17
                      CXX_EXTENSIONS OFF
                      CXX_STANDARD_REQUIRED ON)

set_target_properties(webview PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(webview PROPERTIES PROJECT_NAME ${PROJECT_NAME})
