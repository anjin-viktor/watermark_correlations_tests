# Watermarking Algorithms

This repository provides implementations of various watermarking algorithms using C++.

## Dependencies

To compile and run the code, the following libraries are required:

1. **OpenCV** - Used for reading and writing images.
2. **Boost** - Specifically the `program_options`, `system`, and `unit_test_framework` modules, used for command-line parsing and unit testing.

## Installation

Ensure the following tools and libraries are installed before compiling the project:

1. [**CMake**](https://cmake.org/) - Required for managing the build process.
2. [**Boost**](https://www.boost.org/) - For handling command-line options and running unit tests.
3. [**OpenCV**](https://opencv.org/) - For image processing.

## Compilation

Follow these steps to compile the project:

1. Create a build directory:
    ```bash
    mkdir project
    cd project
    ```
2. Run CMake, specifying the paths to your Boost and OpenCV installations:
    ```bash
    cmake ../ -DOpenCV_DIR=<path_to_opencv_build> -DBOOST_ROOT=<path_to_boost>
    ```
    - Replace `<path_to_opencv_build>` with the path to your OpenCV build directory, e.g., `D:/opencv/build`.
    - Replace `<path_to_boost>` with the path to your Boost installation directory, e.g., `D:/boost_1_85_0`.

3. Once CMake configuration is complete, compile the project.
