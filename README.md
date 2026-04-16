# gBoy

A cross-platform Game Boy emulator written in modern C++20.

## Development Environment Setup (Windows / CLion)

This project is configured to be developed seamlessly across macOS and Windows. On Windows, it is built specifically targeting the **MinGW** toolchain bundled with CLion to maintain cross-platform parity with Clang.

Because `vcpkg` defaults to Microsoft Visual C++ (MSVC) on Windows .

Follow these steps to get the project compiling on a fresh CLion installation:

### 1. Prerequisites
* **CLion:** Installed with the default bundled MinGW toolchain.
* **Vcpkg Plugin:** Ensure the official CLion `vcpkg` plugin is enabled.

### 2. Verify Toolchain
Before building, ensure CLion is actively using MinGW instead of Visual Studio:
1. Go to **Settings > Build, Execution, Deployment > CMake**.
2. Under the **Toolchain** dropdown, verify it is set explicitly to **MinGW**.

### 3. Configure CMake Options (Crucial Step)
To prevent `vcpkg` from searching for an MSVC installation and crashing, we must explicitly pass the MinGW target triplet, host triplet, and compiler paths directly into CMake.

1. Go to **Settings > Build, Execution, Deployment > CMake**.
2. Locate the **CMake options** text field.
3. Paste the following exact string into the field:
   `-DVCPKG_TARGET_TRIPLET=x64-mingw-dynamic -DVCPKG_HOST_TRIPLET=x64-mingw-dynamic -DCMAKE_C_COMPILER=gcc -DCMAKE_CXX_COMPILER=g++` 
4. Click **Apply** and **OK**.

### 4. Build and Run
1. Go to **Tools > CMake > Reset Cache and Reload Project** to clear out any old configurations.
2. CLion will automatically invoke `vcpkg` in the background.
3. `vcpkg` will read the `vcpkg.json` manifest .
4. Once the CMake configuration finishes, build and run the `gBoy` executable.