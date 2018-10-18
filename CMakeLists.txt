cmake_minimum_required(VERSION 3.5)
project(DictaWav)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_STANDARD 17)

# List of Header files (.h, .hh, .hpp)
set(HEADER_FILES
    include/dictawav.h
    include/wav_handler/WavHandler.h
    include/preprocessor/PreProcessor.h
    include/preprocessor/MFCC.h
    include/classificator/Kernel.h
    include/classificator/KernelCanvas.h
    include/classificator/Wisard.h
    include/classificator/Discriminator.h
    include/classificator/Ram.h
    include/preprocessor/FFTHandler.h
    include/preprocessor/DCTHandler.h)

# List of Source files (.c, .cc, .cpp)
set(SOURCE_FILES
        src/main.cpp
    )

# Include Projet cmake scripts (Mostly used to find dependencies libraries on the system)
set(CMAKE_MODULE_PATH
    ${CMAKE_MODULE_PATH}
    ${CMAKE_SOURCE_DIR}/cmake
    )

# Execute each dependency find_cmake scrip
find_package(LibSndFile REQUIRED)
find_package(FFTW REQUIRED)

# Include the dependencies header files to be compiled
if (LIBSNDFILE_FOUND AND FFTW_FOUND)
    include_directories(${LIBSNDFILE_INCLUDE_DIRS}
                        ${FFTW_INCLUDE_DIR}
                        )

    # Specify which project's files will be compiled
    add_executable(${PROJECT_NAME}
                   ${HEADER_FILES}
                   ${SOURCE_FILES}
                   )

    # Link the dependencies libs
    target_link_libraries(${PROJECT_NAME}
                          ${LIBSNDFILE_LIBRARIES}
                          ${FFTW_LIBRARIES}
                          -lstdc++fs
                          )

endif ()