# Building ToneTrigger

This document provides detailed instructions for building the ToneTrigger application from source.

## Prerequisites

### Required Dependencies

1. **C++17 Compatible Compiler**
   - **Windows**: Visual Studio 2019 or later, or MinGW-w64
   - **macOS**: Xcode 10.0 or later, or Clang
   - **Linux**: GCC 7.0 or later, or Clang 5.0 or later

2. **CMake 3.15 or higher**
   - Download from [cmake.org](https://cmake.org/download/)
   - Or install via package manager:
     - **macOS**: `brew install cmake`
     - **Ubuntu/Debian**: `sudo apt install cmake`
     - **Windows**: Use the installer from cmake.org

3. **JUCE Framework 7.x**
   - Download from [juce.com](https://juce.com/get-juce/)
   - Or install via package manager:
     - **macOS**: `brew install juce`
     - **Linux**: Follow JUCE installation guide

### Optional Dependencies

- **Qt 5 or 6** (for enhanced UI features)
- **Essentia library** (for advanced audio analysis)
- **Audio interface** (for low-latency guitar input)

## Building Instructions

### Method 1: Using the Build Script (Recommended)

1. **Clone the repository**:
   ```bash
   git clone https://github.com/makalin/ToneTrigger.git
   cd ToneTrigger
   ```

2. **Run the build script**:
   ```bash
   ./build.sh
   ```

The build script will:
- Create a `build` directory
- Configure the project with CMake
- Build the application
- Create a macOS app bundle (on macOS)

### Method 2: Manual CMake Build

1. **Create build directory**:
   ```bash
   mkdir build
   cd build
   ```

2. **Configure with CMake**:
   ```bash
   cmake .. -DCMAKE_BUILD_TYPE=Release
   ```

3. **Build the project**:
   ```bash
   cmake --build . --config Release
   ```

### Platform-Specific Instructions

#### Windows

1. **Install Visual Studio 2019 or later** with C++ development tools
2. **Install CMake** from cmake.org
3. **Install JUCE** and set the `JUCE_DIR` environment variable
4. **Run the build script** or use CMake GUI

```cmd
# Set JUCE directory (adjust path as needed)
set JUCE_DIR=C:\JUCE

# Build
build.bat
```

#### macOS

1. **Install Xcode** from the App Store
2. **Install CMake**:
   ```bash
   brew install cmake
   ```

3. **Install JUCE**:
   ```bash
   brew install juce
   ```

4. **Build the project**:
   ```bash
   ./build.sh
   ```

The build will create both a command-line executable and a `.app` bundle.

#### Linux

1. **Install development tools**:
   ```bash
   # Ubuntu/Debian
   sudo apt update
   sudo apt install build-essential cmake git
   
   # Fedora
   sudo dnf install gcc-c++ cmake git
   ```

2. **Install JUCE** (follow the official guide)

3. **Build the project**:
   ```bash
   ./build.sh
   ```

## Troubleshooting

### Common Build Issues

#### CMake Cannot Find JUCE

**Error**: `Could not find JUCE`

**Solution**: Set the `JUCE_DIR` environment variable to point to your JUCE installation:

```bash
# macOS/Linux
export JUCE_DIR=/path/to/JUCE

# Windows
set JUCE_DIR=C:\path\to\JUCE
```

#### Compiler Not Found

**Error**: `No CMAKE_CXX_COMPILER could be found`

**Solution**: Install a C++17 compatible compiler:

- **Windows**: Install Visual Studio with C++ tools
- **macOS**: Install Xcode
- **Linux**: Install GCC 7+ or Clang 5+

#### Missing Audio Libraries

**Error**: Audio-related linking errors

**Solution**: Install platform-specific audio libraries:

```bash
# macOS
brew install portaudio

# Ubuntu/Debian
sudo apt install libasound2-dev

# Fedora
sudo dnf install alsa-lib-devel
```

### Build Configuration Options

You can customize the build with CMake options:

```bash
# Debug build
cmake .. -DCMAKE_BUILD_TYPE=Debug

# Enable specific features
cmake .. -DENABLE_VST3=ON -DENABLE_AU=ON

# Set custom install prefix
cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local
```

## Running the Application

### After Building

1. **Navigate to the build directory**:
   ```bash
   cd build
   ```

2. **Run the executable**:
   ```bash
   # Linux/macOS
   ./bin/ToneTrigger
   
   # Windows
   bin\ToneTrigger.exe
   
   # macOS app bundle
   open ToneTrigger.app
   ```

### Audio Setup

1. **Connect your guitar** to an audio interface
2. **Configure audio devices** in the Audio Settings tab
3. **Set appropriate buffer size** for low latency (256 samples or less)
4. **Test audio** using the test button

## Development

### Project Structure

```
ToneTrigger/
├── src/                    # Source code
│   ├── main.cpp           # Application entry point
│   ├── MainComponent.cpp  # Main UI component
│   ├── AudioProcessor.cpp # Audio processing engine
│   ├── Effects/           # Audio effects
│   ├── UI/               # User interface components
│   └── Utils/            # Utility functions
├── assets/               # Application assets
├── CMakeLists.txt        # CMake configuration
├── build.sh             # Build script
└── README.md            # Project documentation
```

### Adding New Effects

1. **Create effect class** in `src/Effects/`
2. **Inherit from BaseEffect**
3. **Implement required methods**
4. **Add to EffectProcessor**
5. **Update UI components**

### Adding New Triggers

1. **Extend TriggerManager** with new trigger types
2. **Update AudioAnalyzer** for detection
3. **Add UI controls** in TriggerPanel

## Contributing

1. **Fork the repository**
2. **Create a feature branch**
3. **Make your changes**
4. **Test thoroughly**
5. **Submit a pull request**

## Support

For build issues or questions:

- **GitHub Issues**: [Create an issue](https://github.com/makalin/ToneTrigger/issues)
- **Email**: makalin@gmail.com
- **Documentation**: Check the README.md file

## License

This project is licensed under the MIT License. See the LICENSE file for details. 