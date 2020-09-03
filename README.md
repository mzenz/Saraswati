# Saraswati: low-latency C++ library for music and audio applications

Saraswati supports:

- Basic audio synthesis
- Reading and writing of `.aiff` audio file format.
- [Music scale](include/Scale.h) generation utility.
- [Music note](include/Note.h) utilities with MIDI support.
- [Waveform utility functions](include/Util.h) for finding maximum sample values, normalizing, applying constant gain, inverting phase, panning, mixing two or more waveforms and more.

## Build instructions

```sh
cmake .
```
