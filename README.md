# LibreEffects

**LibreEffects** is an open-source alternative to Adobe Creative Suite, providing professional-grade creative tools for image editing, video compositing, animation, and more.

## 🎯 Vision

Our goal is to create a comprehensive, free, and open-source suite of creative applications that rival Adobe's professional tools, making advanced creative software accessible to everyone.

## 📦 Applications

- **LibreCanvas** - Advanced image editing and manipulation (Photoshop equivalent) 🎨
- **LibrePremiere** - Video editing and production (Premiere Pro equivalent) 🎬
- **LibreMotion** - Motion graphics and visual effects compositing (After Effects equivalent) ✨
- **LibreVector** - Vector graphics and illustration (Illustrator equivalent) 📐
- **LibreLayout** - Desktop publishing and layout design (InDesign equivalent) 📄
- **LibreAudio** - Audio editing and mixing (Audition equivalent) 🎵

## 🚀 Getting Started

### Prerequisites

- **CMake** 3.20 or higher
- **Qt6** (Core, Widgets, Gui modules)
- **C++17** compatible compiler (GCC, Clang, or MSVC)
- **Git** for version control

### Building LibreCanvas

```bash
# Clone the repository
git clone https://github.com/TimScrawn/LibreEffects.git
cd LibreEffects

# Create build directory
mkdir build && cd build

# Configure with CMake
cmake ..

# Build
cmake --build .

# Run LibreCanvas
./apps/librecanvas/LibreCanvas  # Linux/Mac
# or
apps\librecanvas\Debug\LibreCanvas.exe  # Windows
```

## 🛠️ Tech Stack

- **C++17** - Core language for performance-critical operations
- **Qt6** - Cross-platform UI framework
- **CMake** - Build system
- **OpenGL/Vulkan** - Graphics rendering (planned)
- **Custom Branding System** - Unified theme across all applications

## 🤝 Contributing

We welcome contributions! Please see [CONTRIBUTING.md](CONTRIBUTING.md) for guidelines on how to contribute to this project.

## 📝 License

*License to be determined - likely GPL or similar open-source license*

## 👥 Authors

- [Your Name]
- [Friend's Name]

## 🙏 Acknowledgments

- Inspired by LibreOffice's mission to provide free, open-source alternatives
- Built with the goal of democratizing access to professional creative tools

## 📅 Roadmap

- [x] Project setup and architecture design
- [x] Branding system and theme framework
- [x] LibreCanvas initial structure
- [ ] Core rendering engine
- [ ] LibreCanvas MVP (basic image editing)
- [ ] LibrePremiere MVP
- [ ] Additional applications
- [ ] Documentation and tutorials

---

**Note:** This project is in early development. Contributions, suggestions, and feedback are highly welcome!

