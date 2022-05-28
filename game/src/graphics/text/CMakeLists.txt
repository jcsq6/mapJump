add_library(text INTERFACE "text.h")

find_package(Freetype REQUIRED)

target_include_directories(text INTERFACE ./)
target_link_libraries(text INTERFACE Freetype::Freetype)