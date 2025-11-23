# Building LibreEffects

## Quick Start

### Windows

1. Install Qt6 from [qt.io](https://www.qt.io/download)
2. Install CMake from [cmake.org](https://cmake.org/download/)
3. Install Visual Studio 2019 or later (with C++ support)

```powershell
# Clone repository
git clone https://github.com/TimScrawn/LibreEffects.git
cd LibreEffects

# Create build directory
mkdir build
cd build

# Configure (adjust Qt path if needed)
cmake .. -DCMAKE_PREFIX_PATH="C:/Qt/6.x.x/msvc2019_64"

# Build
cmake --build . --config Release

# Run LibreCanvas
.\apps\librecanvas\Release\LibreCanvas.exe
```

### Linux

```bash
# Install dependencies (Ubuntu/Debian)
sudo apt-get install build-essential cmake qt6-base-dev qt6-base-dev-tools

# Clone and build
git clone https://github.com/TimScrawn/LibreEffects.git
cd LibreEffects
mkdir build && cd build
cmake ..
make -j$(nproc)

# Run
./apps/librecanvas/LibreCanvas
```

### macOS

```bash
# Install dependencies via Homebrew
brew install cmake qt6

# Clone and build
git clone https://github.com/TimScrawn/LibreEffects.git
cd LibreEffects
mkdir build && cd build
cmake .. -DCMAKE_PREFIX_PATH=$(brew --prefix qt6)
make -j$(sysctl -n hw.ncpu)

# Run
./apps/librecanvas/LibreCanvas
```

## Project Structure

```
LibreEffects/
├── apps/
│   └── librecanvas/          # LibreCanvas application
│       ├── src/              # Source files
│       └── resources/        # Resources (icons, etc.)
├── libs/
│   ├── branding/             # Branding and theme system
│   └── core/                 # Core shared libraries
├── CMakeLists.txt            # Root CMake configuration
└── README.md
```

## Branding System

All LibreEffects applications use a unified branding system located in `libs/branding/`. This ensures consistent:
- Colors (primary, secondary, accents)
- Themes (dark/light mode)
- Typography
- Spacing and layout

The theme is automatically applied to all applications.

