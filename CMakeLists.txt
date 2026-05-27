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

    # Pre-populate ASIO 1.22.0 so sleepydiscord's download_project
    # (UPDATE_DISCONNECTED 1) skips download — websocketpp incompatible
    # with ASIO ≥1.24 (missing io_service).
    set(_asio_dir "${sleepydiscord_SOURCE_DIR}/deps/asio")
    if(NOT EXISTS "${_asio_dir}/asio/include/asio/io_service.hpp")
        message(STATUS "Downloading ASIO 1.22.0 for websocketpp compatibility")
        set(_asio_archive "${CMAKE_BINARY_DIR}/asio-1-22-0.tar.gz")
        file(DOWNLOAD
            "https://github.com/chriskohlhoff/asio/archive/refs/tags/asio-1-22-0.tar.gz"
            "${_asio_archive}"
            STATUS _asio_dl_status
        )
        list(GET _asio_dl_status 0 _asio_dl_rc)
        if(_asio_dl_rc EQUAL 0)
            file(MAKE_DIRECTORY "${sleepydiscord_SOURCE_DIR}/deps")
            if(CMAKE_VERSION VERSION_GREATER_EQUAL 3.18)
                file(ARCHIVE_EXTRACT INPUT "${_asio_archive}" DESTINATION "${sleepydiscord_SOURCE_DIR}/deps")
            else()
                find_program(_asio_tar tar)
                if(_asio_tar)
                    execute_process(COMMAND tar -xzf "${_asio_archive}" WORKING_DIRECTORY "${sleepydiscord_SOURCE_DIR}/deps")
                else()
                    message(FATAL_ERROR "tar not found - required to extract ASIO 1.22.0")
                endif()
            endif()
            # GitHub archive extracts as asio-asio-1-22-0 → rename to asio
            if(EXISTS "${sleepydiscord_SOURCE_DIR}/deps/asio-asio-1-22-0" AND NOT EXISTS "${_asio_dir}")
                file(RENAME "${sleepydiscord_SOURCE_DIR}/deps/asio-asio-1-22-0" "${_asio_dir}")
            endif()
        else()
            message(WARNING "Failed to download ASIO 1.22.0: ${_asio_dl_status}")
        endif()
    endif()

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
target_link_libraries(${PROJECT_NAME} dl)
target_link_libraries(${PROJECT_NAME} nlohmann_json::nlohmann_json)
if (WIN32)
	target_link_libraries(${PROJECT_NAME}  ${Boost_LIBRARIES})
else()
    target_link_libraries(R.A.B.I.D.S Boost::log_setup Boost::log)
endif()
