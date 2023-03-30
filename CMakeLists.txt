cmake_minimum_required(VERSION 3.0)

project(app)

find_package(SDL2 REQUIRED) #Threads SDL2
find_package(Threads REQUIRED) #Threads lib
    
#set the source of all the cpp file
set(SOURCES src/util.cpp main.cpp) 

# Set up the executable.
add_executable(app ${SOURCES})

# Link the executable with the SDL2 and SDL2_ttf libraries.
target_link_libraries(app ${SDL2_LIBRARIES} Threads::Threads)

# Add the SDL2 include directory to the include paths.
include_directories(${SDL2_INCLUDE_DIRS} ${SDL2_TTF_INCLUDE_DIR} include)
