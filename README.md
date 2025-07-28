# ToneTrigger

## Overview

ToneTrigger is an innovative live guitar processing application that dynamically switches audio effects based on specific notes, chords, or melodies played by the user. By leveraging real-time audio analysis, ToneTrigger detects predefined musical triggers to seamlessly apply effects like distortion, reverb, or delay, offering a unique performance experience for guitarists. Whether you're a live performer or a studio musician, ToneTrigger enhances your creativity by automating effect changes in sync with your playing.

## Features

- **Real-Time Audio Processing**: Low-latency guitar input and effect application using JUCE.
- **Note/Melody Detection**: Automatically detects specific notes, chords, or melody patterns to trigger effect changes.
- **Customizable Triggers**: Configure effects to switch based on user-defined musical events (e.g., a Cmaj chord or a melody sequence).
- **Cross-Platform**: Supports Windows, macOS, iOS, and Android for broad accessibility.
- **Intuitive UI**: User-friendly interface for setting triggers and tweaking effect parameters.
- **Extensible Effects**: Supports a range of effects like distortion, reverb, delay, and more, with easy integration for custom DSP.

## Tech Stack

- **JUCE (C++)**: Core framework for low-latency audio processing and effect implementation.
- **Essentia**: Real-time audio analysis for note and melody detection.
- **Qt**: Cross-platform UI for configuring triggers and effects.
- **CMake**: Build system for managing dependencies and cross-platform compilation.
- **Optional ML Integration**: ONNX Runtime for advanced melody detection using pre-trained models.

## Installation

### Prerequisites
- C++17-compatible compiler (e.g., GCC, Clang, MSVC)
- CMake 3.15 or higher
- JUCE 7.x
- Qt 5 or 6
- Essentia library
- Audio interface for low-latency guitar input (e.g., Focusrite Scarlett, iRig)

### Build Instructions
1. Clone the repository:
   ```
   git clone https://github.com/makalin/ToneTrigger.git
   cd ToneTrigger
   ```
2. Install dependencies:
   - Install JUCE: Follow instructions at [JUCE Documentation](https://juce.com/).
   - Install Qt: Download from [Qt Downloads](https://www.qt.io/download).
   - Install Essentia: Follow [Essentia Installation Guide](https://essentia.upf.edu/installing.html).
3. Configure the project:
   ```
   mkdir build
   cd build
   cmake ..
   ```
4. Build the project:
   ```
   cmake --build .
   ```
5. Run ToneTrigger:
   - On Windows: `./ToneTrigger.exe`
   - On macOS: `./ToneTrigger.app`
   - On Linux: `./ToneTrigger`

## Usage

1. **Connect Your Guitar**:
   - Plug your guitar into an audio interface connected to your device.
   - Configure the audio input/output in ToneTrigger’s settings.

2. **Set Triggers**:
   - Open the UI and navigate to the "Triggers" tab.
   - Define note/chord triggers (e.g., "C4" or "Cmaj") or upload a MIDI file for melody-based triggers.
   - Assign effects (e.g., distortion, reverb) to each trigger.

3. **Play and Experiment**:
   - Start playing your guitar. ToneTrigger will detect your predefined triggers and switch effects in real time.
   - Adjust effect parameters (e.g., reverb depth, delay time) via the UI.

4. **Save Presets**:
   - Save your trigger and effect configurations for future performances.

## Roadmap

- Add support for cloud-based preset sharing via Firebase.
- Implement advanced melody detection using machine learning models.
- Support for VST/AU plugin export for use in DAWs.
- Add WebSocket integration for real-time remote control.
- Optimize latency for high-performance live settings.

## Contributing

We welcome contributions! To contribute:
1. Fork the repository.
2. Create a feature branch (`git checkout -b feature/your-feature`).
3. Commit your changes (`git commit -m "Add your feature"`).
4. Push to the branch (`git push origin feature/your-feature`).
5. Open a pull request.

Please follow our [Code of Conduct](CODE_OF_CONDUCT.md) and submit issues for bugs or feature requests.

## License

This project is licensed under the MIT License. See [LICENSE](LICENSE) for details.

## Contact

For questions or feedback, reach out via [GitHub Issues](https://github.com/makalin/ToneTrigger/issues) or send e-mail to makalin@gmail.com.
