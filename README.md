# TOC
- [What is Tiny Rasterizer](#what-is-tiny-rasterizer)
  - [Why Software Rendering](#why-software-rendering)
- [Philosophies](#philosophies)
- [Build System](#build-system)
  - [1. Prerequisites](#1-prerequisites)
  - [2. Environment Setup](#2-environment-setup)
  - [3. Building the Project](#3-building-the-project)
- [Showcase](#showcase)
  - [Code Hotreloading + Record & Rewind](#code-hotreloading--record--rewind)
  - [Mesh Hotreloading](#mesh-hotreloading)

## What is Tiny Rasterizer
Tiny Rasterizer is a software renderer, meaning it processes graphics entirely using the CPU rather than relying on specialized GPU hardware. Software rendering has a rich history in computer graphics and remains relevant for its flexibility, educational value, and independence from hardware constraints.

### Why Software Rendering

GPUs offer advanced, high-performance features, but their complexity can overshadow the elegance of basic rendering principles. By stripping away the complexities of GPU programming, the project focuses on the essentials, making it ideal for learning and experimentation

Not every device has a modern GPU or reliable driver support. CPU-based rendering ensures Tiny Rasterizer works on a wider range of systems, from low-end machines to virtualized environments without GPU access. By avoiding GPU-specific optimizations, Tiny Rasterizer can be more easily adapted to different platforms and architectures

## Philosophies
Coming soon...

## Build System
Note: Currently, only x64 Windows development is supported.


### 1. Prerequisites
Ensure you have the following installed:

```
Microsoft C/C++ Build Tools (version 19.41 or later) with the MSVC compiler and Windows SDK.
(Optional) Clang, if the Windows SDK is already installed.
```

### 2. Environment Setup
Before building, configure your terminal to use the MSVC compiler. This is done by running the vcvarsall.bat x64 script, which is included in the Microsoft C/C++ Build Tools.

Alternatively, you can use the x64 Native Tools Command Prompt for VS <year>, accessible from the Windows Start Menu.

To confirm the compiler is correctly set up, run:
```
cl
```

Example output:
```
Microsoft (R) C/C++ Optimizing Compiler Version 19.41.34120 for x64
Copyright (C) Microsoft Corporation.  All rights reserved.

usage: cl [ option... ] filename... [ /link linkoption... ]
```

### 3. Building the Project
To build the project:

1. Open your configured terminal.
2. Navigate to the root directory of the repository.
3. Run the build script:
```
build
```

You should see output similar to this:

```
[Debug mode enabled]
[Using MSVC compiler]
Building game DLL...
Building main application...
Build successful
```

If the build completes successfully, a `build` folder will be created at the root of the codebase, containing the newly compiled `TinyRasterizer.exe`.


## Showcase
### Code Hotreloading + Record & Rewind
![colorpicker](https://github.com/user-attachments/assets/76531869-d169-4175-ba31-fc842a8824f2)

### Mesh Hotreloading
![hot edit blend](https://github.com/user-attachments/assets/297bcffb-7ad5-4f69-b3a4-29f3ecfb0ce5)
