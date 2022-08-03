cmake_minimum_required (VERSION 3.11)
project ("R.A.B.I.D.S" CXX C)
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
    GIT_TAG origin/master
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
add_executable (${PROJECT_NAME} "src/main.cpp" "src/config.cpp" "src/downtimeTracker.cpp" "src/rabids.cpp" "src/sqlite3.c")
add_subdirectory(src)
target_link_libraries(${PROJECT_NAME} OpenSSL::SSL)
target_link_libraries(${PROJECT_NAME} sleepy-discord)
target_link_libraries(${PROJECT_NAME} fmt)
target_link_libraries(${PROJECT_NAME} dl)
target_link_libraries(${PROJECT_NAME} nlohmann_json::nlohmann_json)
if (WIN32)
	target_link_libraries(${PROJECT_NAME}  ${Boost_LIBRARIES})
else()
    target_link_libraries(R.A.B.I.D.S Boost::log_setup Boost::log)
endif()
