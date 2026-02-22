set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_SYSTEM_VERSION 10.0.x)

set(LLVM_PATH "C:/Program Files/LLVM/bin")
set(CMAKE_CXX_COMPILER "${LLVM_PATH}/clang-cl.exe")
set(CMAKE_C_COMPILER "${LLVM_PATH}/clang-cl.exe")
set(CMAKE_LINKER "${LLVM_PATH}/lld-link.exe")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_COMPILER_ID "Clang")
set(CMAKE_CXX_SIMULATE_ID "MSVC")
set(CMAKE_CXX_COMPILER_FRONTEND_VARIANT "MSVC")


add_compile_options("-Wno-ignored-attributes")
