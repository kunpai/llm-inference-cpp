cmake_minimum_required(VERSION 3.14)
project(llm-inference-cpp)

set(CMAKE_CXX_STANDARD 17)

# --- cpp-dotenv source files ---
file(GLOB_RECURSE DOTENV_SRC
    external/cpp-dotenv/src/*.cpp
    external/cpp-dotenv/common/antlr/parser/**/*.cpp
)


# Define the dotenv static library target
add_library(dotenv STATIC ${DOTENV_SRC})

# Add cpp-dotenv's bundled ANTLR4 runtime
file(GLOB_RECURSE ANTLR_SRC
    external/cpp-dotenv/common/libs/antlr4-cpp-runtime/runtime/src/*.cpp
)

add_library(antlr4-runtime STATIC ${ANTLR_SRC})

target_include_directories(antlr4-runtime PUBLIC
    external/cpp-dotenv/common/libs/antlr4-cpp-runtime/runtime/src
)

# Link cpp-dotenv to local antlr4-runtime
target_link_libraries(dotenv PUBLIC antlr4-runtime)


# Include cpp-dotenv headers
target_include_directories(dotenv PUBLIC
    external/cpp-dotenv/include
    external/cpp-dotenv/src
    external/cpp-dotenv/src/structures
    external/cpp-dotenv/src/common
    external/cpp-dotenv/src/listeners
    external/cpp-dotenv/src/listeners/dotenv
    external/cpp-dotenv/src/listeners/line/
    external/cpp-dotenv/common/antlr/parser/dotenv/
    external/cpp-dotenv/common/antlr/parser/line/
)

# --- main executable ---
add_executable(main src/main.cpp)
target_link_libraries(main PRIVATE dotenv)
target_include_directories(main PRIVATE external/cpp-dotenv/include)
