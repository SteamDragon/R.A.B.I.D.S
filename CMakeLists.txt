cmake_minimum_required (VERSION 3.11)
project ("R.A.B.I.D.S" CXX)
include(FetchContent)
FetchContent_Declare(json
    GIT_REPOSITORY https://github.com/ArthurSonzogni/nlohmann_json_cmake_fetchcontent
    GIT_PROGRESS TRUE
    GIT_SHALLOW TRUE
    GIT_TAG v3.10.5
)

FetchContent_Declare(sleepydiscord
    GIT_REPOSITORY https://github.com/yourWaifu/sleepy-discord.git
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
if(NOT sleepy-discord_POPULATED)
    FetchContent_Populate(sleepydiscord)
    add_subdirectory(${sleepydiscord_SOURCE_DIR} ${sleepydiscord_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

FetchContent_GetProperties(fmt)
if(NOT sleepy-discord_POPULATED)
    FetchContent_Populate(fmt)
    add_subdirectory(${fmt_SOURCE_DIR} ${fmt_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

set(Boost_USE_STATIC_LIBS ON)
find_package(BOOST 1.68.0 REQUIRED COMPONENTS date_time log log_setup)
find_path(BOOST_INCLUDE_DIR boost)
include_directories(${BOOST_INCLUDE_DIR})
if (WIN32)
	set(BOOST_LIBRARIES ${Boost_LIBRARY_DIRS}/boost_system-vc140-mt.lib)
	if (CMAKE_BUILD_TYPE MATCHES "Debug")
		set(BOOST_LIBRARIES ${Boost_LIBRARY_DIRS}/../debug/lib/boost_system-vc140-mt-gd.lib)
	endif()
endif()

find_package(OpenSSL REQUIRED)
add_executable (${PROJECT_NAME} "src/main.cpp" "src/config.cpp")
add_subdirectory(src)
target_link_libraries(${PROJECT_NAME}  PUBLIC ${Boost_LIBRARIES})
target_link_libraries(${PROJECT_NAME}  OpenSSL::SSL)
target_link_libraries(${PROJECT_NAME}  nlohmann_json::nlohmann_json)
target_link_libraries(${PROJECT_NAME}  sleepy-discord)
target_link_libraries(${PROJECT_NAME}  fmt)