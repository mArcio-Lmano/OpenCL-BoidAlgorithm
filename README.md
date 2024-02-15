# Boid Simulation with OpenCL and SFML

This project demonstrates a boid simulation implemented using OpenCL for parallel computation and SFML for rendering. Boids are simple agents that follow a set of rules to simulate flocking behavior.

![Boid Simulation Example](https://github.com/mArcio-Lmano/OpenCL/raw/main/example.gif)



## Overview

The simulation consists of the following components:

- **Boid Kernel**: Implemented in OpenCL, the kernel updates the position and velocity of each boid based on simple rules such as cohesion, alignment, and separation.
- **SFML Renderer**: Renders the boids using SFML's graphics capabilities. It also displays the frames per second (FPS) of the simulation.
- **Main Program**: Orchestrates the simulation by integrating the OpenCL kernel with the SFML renderer. It also handles user input and manages the main loop of the simulation.

## Features

- Parallel computation of boid behavior using OpenCL, enabling efficient simulation of a large number of boids.
- Real-time rendering of boids using SFML, providing a visual representation of the flocking behavior.
- Wrap-around boundary handling, ensuring that boids wrap around the screen when reaching the window boundaries.
- Display of frames per second (FPS), allowing users to monitor the performance of the simulation.

## Dependencies

- OpenCL: The project requires an OpenCL-compatible GPU and drivers to run the kernel code.
- SFML: The SFML library is used for graphics rendering and window management. Make sure to have SFML installed or included in your project dependencies.

## Getting Started

1. **Clone the Repository**: Clone this repository to your local machine using `git clone`.
2. **Build the Project**: Compile the project using your preferred build system (e.g., CMake, Makefile).
3. **Run the Executable**: Execute the compiled binary to start the boid simulation.
4. **Interact with the Simulation**: Press the spacebar to start and pause the simulation. Close the window to stop the simulation. You can also adjust simulation parameters or customize the code to suit your needs.

## Contributing

Contributions are welcome! If you find any bugs or have suggestions for improvements, please open an issue or create a pull request on GitHub.

**Disclaimer**: The behavior of the algorithm may differ from the original behavior of boid simulations, but it aims to provide an interesting and visually appealing simulation.
