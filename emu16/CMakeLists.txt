file(GLOB SOURCE_FILES source/*.cpp)
file(GLOB HEADER_FILES source/*.h)

add_executable(emu16 ${SOURCE_FILES} ${HEADER_FILES})
