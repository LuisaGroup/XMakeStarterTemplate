# XMake Starter Template
A starter template for using LuisaCompute as a submodule with the XMake build system.

# Use xmake?
- Download and install xmake: https://xmake.io/
- Install compiler, MSVC, GCC, LLVM, etc.
- Config:
    > xmake f -c
- Build:
    > xmake
- Run (BACKEND_NAME can be cuda, dx, metal, etc):
    > xmake run demo #BACKEND_NAME#

# Set custom options?
- All options defined in ext/LuisaCompute/xmake.lua
- Config example:
    > xmake f --toolchain=llvm --enable_gui=false -c
- Build after config:
    > xmake

# VSCode
- Install necessary plugins: C/C++(Microsoft), clangd(LLVM), CodeLLDB(Vadim Chugunov), XMake(tboox)
- LLVM-clangd must be well-installed, try:
    > clangd.exe --version
- Open project with vscode.