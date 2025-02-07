# Godot CaptureTexture

Implements a TextureImage that streams its image data from a shared memory RGBA source via the [shm_ringbuffers](https://github.com/directrix1/shm_ringbuffers) library on platforms supported by the **shm_ringbuffers** library. This can be used to, for example, stream a scene from OBS into your running Godot project, via the [obs-shmem-ringbuffer](https://github.com/directrix1/obs-shmem-buffer). This project only supports Godot 4.x.

## Building Prerequisites

This software requires the [SCons build system](https://scons.org/). Additionally, you must have [shm_ringbuffers](https://github.com/directrix1/shm_ringbuffers) installed at a location available searched by pkg-config (so you probably want it installed to /usr prefix) .

## Building

To build:

    git clone --recurse-submodules https://github.com/directrix1/godot-capture-texture.git
    cd godot-capture-texture
    scons

## Using in your project

To use this in your project after building. Go into the demo/ folder, and copy the "bin" subfolder into your project root. CaptureTexture should now be available after loading the project.

## Demo project

In the demo folder there is an example project that uses the CaptureTexture component.


## License

This software is [Copyright (c) 2025 Edward Andrew Flick](AUTHORS.md), and released under the [MIT license](LICENSE.md).
