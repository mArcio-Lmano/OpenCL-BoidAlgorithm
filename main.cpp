// Define the target OpenCL version
#define CL_TARGET_OPENCL_VERSION 300
// Define the maximum number of characters in kernel source
#define MAX_SOURCE_SIZE (0x100000)

#include <iostream>
#include <fstream>
#include <vector>
#include <CL/cl.h>
#include <cassert>
#include <SFML/Graphics.hpp>
#include <SFML/Window/Event.hpp>

// Define the number of boids
const int NUM_BOIDS = 5000;
const float WIDTH = 1920.0f;
const float HEIGHT = 1080.0f;


// Structure to represent a boid
struct Boid {
    float x, y, vx, vy;
};

int main() {

    // Declare a boolean variable to track whether the simulation should run or not
    bool runSimulation = false;

    // Load kernel source from file
    std::ifstream kernelFile("boid.cl");
    if (!kernelFile.is_open()) {
        std::cerr << "Failed to open kernel file." << std::endl;
        return 1;
    }

    std::string kernelCode((std::istreambuf_iterator<char>(kernelFile)), std::istreambuf_iterator<char>());
    const char* kernelSource = kernelCode.c_str();
    size_t sourceSize = kernelCode.size();

    // Initialize OpenCL
    cl_platform_id platform;
    clGetPlatformIDs(1, &platform, NULL);

    cl_device_id device;
    clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, NULL);

    cl_context context = clCreateContext(NULL, 1, &device, NULL, NULL, NULL);
    cl_int command_queue_result;
    cl_queue_properties queue_properties[] = {0};
    cl_command_queue queue = clCreateCommandQueueWithProperties(context, device, queue_properties, &command_queue_result);
    assert(command_queue_result == CL_SUCCESS);

    // Create buffer for boids
    cl_mem boidBuffer = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(Boid) * NUM_BOIDS, NULL, NULL);

    // Initialize boids with random positions and velocities
    std::vector<Boid> boids(NUM_BOIDS);
    for (int i = 0; i < NUM_BOIDS; ++i) {
        boids[i].x = static_cast<float>(rand()) / RAND_MAX * WIDTH;
        boids[i].y = static_cast<float>(rand()) / RAND_MAX * HEIGHT;
        boids[i].vx = static_cast<float>(rand()) / RAND_MAX * 10.0f - 5.0f;
        boids[i].vy = static_cast<float>(rand()) / RAND_MAX * 10.0f - 5.0f;
    }

    // Create OpenCL program
    cl_program program = clCreateProgramWithSource(context, 1, &kernelSource, &sourceSize, NULL);
    clBuildProgram(program, 1, &device, NULL, NULL, NULL);

    // Create kernel
    cl_kernel kernel = clCreateKernel(program, "update_boids", NULL);

    // Set kernel arguments
    clSetKernelArg(kernel, 0, sizeof(cl_mem), &boidBuffer);
    clSetKernelArg(kernel, 1, sizeof(int), &NUM_BOIDS);
    float dt = 0.1f;
    clSetKernelArg(kernel, 2, sizeof(float), &dt);
    clSetKernelArg(kernel, 3, sizeof(float), &WIDTH);
    clSetKernelArg(kernel, 4, sizeof(float), &HEIGHT);

    // Create SFML window
    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Boid Simulation");

    // Declare Font
    sf::Font font;
    if (!font.loadFromFile("arial.ttf")) {
        std::cerr << "Failed to load font." << std::endl;
        return 1;
    }

    // Create text object for displaying FPS
    sf::Text fpsText;
    fpsText.setFont(font);
    fpsText.setCharacterSize(24);
    fpsText.setFillColor(sf::Color::White);
    fpsText.setPosition(10, 10);

    // Clock for measuring time
    sf::Clock clock;

    // Main loop
    while (window.isOpen()) {
        // Process events
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
            
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Space) {
                    // Toggle the value of runSimulation when the space key is pressed
                    runSimulation = !runSimulation;
                }
            }
        }

        // Only execute the simulation if runSimulation is true
        if (runSimulation) {
            // Execute OpenCL kernel
            clEnqueueWriteBuffer(queue, boidBuffer, CL_TRUE, 0, sizeof(Boid) * NUM_BOIDS, boids.data(), 0, NULL, NULL);
            size_t globalWorkSize = NUM_BOIDS;
            clEnqueueNDRangeKernel(queue, kernel, 1, NULL, &globalWorkSize, NULL, 0, NULL, NULL);
            clEnqueueReadBuffer(queue, boidBuffer, CL_TRUE, 0, sizeof(Boid) * NUM_BOIDS, boids.data(), 0, NULL, NULL);
        }

        // Measure elapsed time and calculate FPS
        sf::Time elapsedTime = clock.restart(); 
        float fps = 1.0f / elapsedTime.asSeconds();

        // Update FPS text
        fpsText.setString("FPS: " + std::to_string(static_cast<int>(fps)));
        // Clear window
        window.clear();

        // Render boids using SFML
        for (int i = 0; i < NUM_BOIDS; ++i) {
            sf::CircleShape shape(1); // Example: Render each boid as a circle
            shape.setPosition(boids[i].x, boids[i].y);
            window.draw(shape);
        }

        // Show Fps
        window.draw(fpsText);
        // Display window
        window.display();
    }
    // Clean up
    clReleaseKernel(kernel);
    clReleaseProgram(program);
    clReleaseCommandQueue(queue);
    clReleaseContext(context);
    clReleaseMemObject(boidBuffer);

    return 0;
}
