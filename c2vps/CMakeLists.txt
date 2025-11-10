# CMakeList.txt : CMake project for c2vps, include source and define
# project specific logic here.
#

find_package(Drogon CONFIG REQUIRED)
find_package(unofficial-sodium CONFIG REQUIRED)
find_package(jwt-cpp CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(CURL REQUIRED)

# Add source to this project's executable.
add_executable (c2vps "src/c2vps.cpp" "src/db_manage.cpp" "src/hash.cpp" "src/mail.cpp")

target_link_libraries(c2vps PRIVATE 
    Drogon::Drogon
    unofficial-sodium::sodium
    jwt-cpp::jwt-cpp
    nlohmann_json::nlohmann_json
    CURL::libcurl
)

target_include_directories(c2vps
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET c2vps PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
