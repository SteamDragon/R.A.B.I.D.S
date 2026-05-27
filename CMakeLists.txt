cmake_minimum_required (VERSION 3.11)
project ("R.A.B.I.D.S" CXX C)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
include(FetchContent)
FetchContent_Declare(json
    GIT_REPOSITORY https://github.com/ArthurSonzogni/nlohmann_json_cmake_fetchcontent
    GIT_PROGRESS TRUE
    GIT_SHALLOW TRUE
    GIT_TAG v3.10.5
)

FetchContent_Declare(sleepydiscord
    GIT_REPOSITORY https://github.com/SteamDragon/sleepy-discord
    GIT_PROGRESS TRUE
    GIT_SHALLOW TRUE
    GIT_TAG origin/develop
)

FetchContent_Declare(fmt
    GIT_REPOSITORY https://github.com/fmtlib/fmt.git
    GIT_PROGRESS TRUE
    GIT_SHALLOW TRUE
    GIT_TAG 8.1.1
)

FetchContent_GetProperties(json)
if(NOT json_POPULATED)
    FetchContent_Populate(json)
    add_subdirectory(${json_SOURCE_DIR} ${json_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

FetchContent_GetProperties(sleepydiscord)
if(NOT sleepydiscord_POPULATED)
    FetchContent_Populate(sleepydiscord)

    # Patch sleepydiscord's CMakeLists.txt to pin ASIO to 1-22-0 instead of master.
    # Websocketpp is incompatible with ASIO ≥1.24 (io_service removed).
    file(READ "${sleepydiscord_SOURCE_DIR}/CMakeLists.txt" _sd_cmake)
    string(REGEX REPLACE
        "(PROJ[ \t]+asio[ \t]*\n[ \t]*GIT_REPOSITORY[ \t]+https://github.com/chriskohlhoff/asio.git\n[ \t]*GIT_TAG)[ \t]+master"
        "\\1        asio-1-22-0"
        _sd_cmake "${_sd_cmake}"
    )
    file(WRITE "${sleepydiscord_SOURCE_DIR}/CMakeLists.txt" "${_sd_cmake}")
    unset(_sd_cmake)

    add_subdirectory(${sleepydiscord_SOURCE_DIR} ${sleepydiscord_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

FetchContent_GetProperties(fmt)
if(NOT fmt_POPULATED)
    FetchContent_Populate(fmt)
    add_subdirectory(${fmt_SOURCE_DIR} ${fmt_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

SET(Boost_USE_STATIC_LIBS ON)     
find_package(Boost 1.65 COMPONENTS date_time log log_setup REQUIRED)
if (WIN32)
	find_path(BOOST_INCLUDE_DIR boost)
        include_directories(${BOOST_INCLUDE_DIR})
	set(BOOST_LIBRARIES ${Boost_LIBRARY_DIRS}/boost_system-vc140-mt.lib)
	if (CMAKE_BUILD_TYPE MATCHES "Debug")
		set(BOOST_LIBRARIES ${Boost_LIBRARY_DIRS}/../debug/lib/boost_system-vc140-mt-gd.lib)
	endif()
endif()

find_package(OpenSSL REQUIRED)
add_executable (${PROJECT_NAME} "src/main.cpp" "src/config.cpp" "src/config/messages.cpp" "src/downtimeTracker.cpp" "src/rabids.cpp" "src/utils.cpp" "src/commands.cpp" "src/process.cpp" "src/syncEngine.cpp" "src/sqlite3.c")
add_subdirectory(src)
target_link_libraries(${PROJECT_NAME} OpenSSL::SSL)
target_link_libraries(${PROJECT_NAME} sleepy-discord)
target_link_libraries(${PROJECT_NAME} fmt)
find_package(PkgConfig QUIET)
if(PkgConfig_FOUND)
    pkg_check_modules(CURL REQUIRED libcurl)
    target_link_libraries(${PROJECT_NAME} ${CURL_LIBRARIES})
    target_include_directories(${PROJECT_NAME} PRIVATE ${CURL_INCLUDE_DIRS})
else()
    find_package(CURL REQUIRED)
    target_link_libraries(${PROJECT_NAME} CURL::libcurl)
endif()
target_link_libraries(${PROJECT_NAME} dl)
target_link_libraries(${PROJECT_NAME} nlohmann_json::nlohmann_json)
if (WIN32)
	target_link_libraries(${PROJECT_NAME}  ${Boost_LIBRARIES})
else()
    target_link_libraries(R.A.B.I.D.S Boost::log_setup Boost::log)
endif()

# Tests are standalone: cmake -B build/tests -S tests && cmake --build build/tests
