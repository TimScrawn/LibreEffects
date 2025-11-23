# Installing Qt6 for LibreEffects

## Windows

### Option 1: Qt Online Installer (Recommended)

1. **Download Qt Online Installer**:
   - Go to https://www.qt.io/download-qt-installer
   - Download the installer for Windows

2. **Run the Installer**:
   - Create a Qt account (free)
   - Select "Open Source" license
   - Choose installation path (e.g., `C:\Qt`)
   - Select components:
     - **Qt 6.7.x** (or latest stable)
     - **MSVC 2019 64-bit** (or MSVC 2022 64-bit)
     - **MinGW 11.2.0 64-bit** (optional, alternative compiler)
     - **Qt Creator** (optional IDE, but Cursor works fine)

3. **Add Qt to PATH** (Optional but helpful):
   - Add `C:\Qt\6.x.x\msvc2019_64\bin` to your system PATH
   - Or use Qt Creator which handles this automatically

4. **Configure CMake**:
   ```powershell
   mkdir build
   cd build
   cmake .. -DCMAKE_PREFIX_PATH="C:/Qt/6.x.x/msvc2019_64"
   ```

### Option 2: vcpkg (Package Manager)

```powershell
# Install vcpkg if you don't have it
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
.\bootstrap-vcpkg.bat

# Install Qt6
.\vcpkg install qt6-base[core,widgets,gui]:x64-windows

# Configure CMake
cmake .. -DCMAKE_TOOLCHAIN_FILE=[path to vcpkg]/scripts/buildsystems/vcpkg.cmake
```

### Option 3: Conan (Package Manager)

```powershell
pip install conan
conan install qt/6.7.0@ -g CMakeDeps -g CMakeToolchain
cmake .. -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake
```

## Linux (Ubuntu/Debian)

```bash
# Install Qt6 via package manager
sudo apt-get update
sudo apt-get install qt6-base-dev qt6-base-dev-tools qt6-tools-dev

# Verify installation
qmake6 --version

# Build (CMake should find Qt automatically)
mkdir build && cd build
cmake ..
make -j$(nproc)
```

## macOS

### Option 1: Homebrew (Recommended)

```bash
# Install Qt6
brew install qt6

# Configure CMake
mkdir build && cd build
cmake .. -DCMAKE_PREFIX_PATH=$(brew --prefix qt6)
make -j$(sysctl -n hw.ncpu)
```

### Option 2: Qt Online Installer

1. Download from https://www.qt.io/download-qt-installer
2. Run installer
3. Select Qt 6.7.x and macOS components
4. Configure CMake with the Qt path

## Verifying Installation

After installing, verify Qt is found:

```bash
# Windows (PowerShell)
& "C:\Qt\6.x.x\msvc2019_64\bin\qmake.exe" --version

# Linux/Mac
qmake6 --version
```

## Building LibreCanvas

Once Qt is installed:

```bash
# Create build directory
mkdir build
cd build

# Configure (adjust Qt path if needed)
# Windows:
cmake .. -DCMAKE_PREFIX_PATH="C:/Qt/6.x.x/msvc2019_64"

# Linux/Mac (usually auto-detected):
cmake ..

# Build
cmake --build . --config Release

# Run
# Windows:
.\apps\librecanvas\Release\LibreCanvas.exe

# Linux/Mac:
./apps/librecanvas/LibreCanvas
```

## Troubleshooting

### CMake can't find Qt

Set the `CMAKE_PREFIX_PATH` explicitly:
```bash
cmake .. -DCMAKE_PREFIX_PATH="/path/to/qt6"
```

### Missing Qt modules

Make sure you installed:
- Qt6::Core
- Qt6::Widgets  
- Qt6::Gui

### Compiler errors

- Windows: Install Visual Studio 2019/2022 with C++ support
- Linux: Install `build-essential` package
- Mac: Install Xcode Command Line Tools: `xcode-select --install`

