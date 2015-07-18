file(GLOB SOURCE_FILES source/*.cpp)
file(GLOB HEADER_FILES source/*.h)

find_package(SDL)

include_directories(${SDL_INCLUDE_DIR})
add_executable(emu16 ${SOURCE_FILES} ${HEADER_FILES} source/device_video.h source/device_video.cpp source/config.h source/font.h)
target_link_libraries(emu16 LINK_PUBLIC ${SDL_LIBRARY})
