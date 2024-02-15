// Define the target OpenCL version
#define CL_TARGET_OPENCL_VERSION 300

// Include necessary headers
#include <iostream>
#include <CL/cl.h>
#include <cassert>
#include <cstring>
#include <fstream>
#include <streambuf>

int main() {
    // Define variables for buffer sizes
    const int num_elements = 256; // Define the number of elements

    // Declare an array to store platform IDs and a variable to store the platform count
    cl_platform_id platforms[64];
    unsigned int platform_count;

    // Get platform IDs
    cl_int platform_result = clGetPlatformIDs(64, platforms, &platform_count);
    assert(platform_result == CL_SUCCESS);

    // Declare a variable to store the selected device
    cl_device_id  device = nullptr;

    // Iterate over available platforms
    for (int i = 0; i < platform_count; ++i) {
        // Declare an array to store device IDs and a variable to store the device count
        cl_device_id devices[64];
        unsigned int devices_count;

        // Get device IDs for the current platform
        cl_int devices_result = clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_GPU, 64, devices, &devices_count);

        // Check if device IDs are successfully obtained
        if (devices_result == CL_SUCCESS) {
            // Iterate over available devices
            for (int j = 0; j < devices_count; ++j) {
                // Get vendor name of the device
                char vendor_name[256];
                size_t vendor_name_leng;
                cl_int device_info_result = clGetDeviceInfo(devices[j], CL_DEVICE_VENDOR, sizeof(vendor_name), vendor_name, &vendor_name_leng);

                // Check if vendor name retrieval is successful
                if (device_info_result == CL_SUCCESS) {
                    // Print vendor name
                    std::cout << "Vendor: " << vendor_name << std::endl;

                    // Get device type
                    cl_device_type device_type;
                    clGetDeviceInfo(devices[j], CL_DEVICE_TYPE, sizeof(device_type), &device_type, NULL);

                    // Print device type
                    if (device_type == CL_DEVICE_TYPE_GPU){
                        std::cout << "Device type: GPU" << std::endl;
                        device = devices[j]; // Assign the GPU device
                    }
                    else if (device_type == CL_DEVICE_TYPE_ACCELERATOR)
                        std::cout << "Device type: Accelerator (Possibly APU)" << std::endl;
                    else
                        std::cout << "Device type: Unknown" << std::endl;

                    // Get device name
                    char device_name[256];
                    size_t device_name_len;
                    clGetDeviceInfo(devices[j], CL_DEVICE_NAME, sizeof(device_name), device_name, &device_name_len);
                    // Print device name
                    std::cout << "Device name: " << device_name << std::endl;
                }
            }
        }
    }

    // Query maximum work-group size
    size_t max_work_group_size;
    clGetDeviceInfo(device, CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(size_t), &max_work_group_size, NULL);
    std::cout << "Max Work-Group Size: " << max_work_group_size << std::endl;

    // Query maximum memory allocation size
    cl_ulong max_mem_alloc_size;
    clGetDeviceInfo(device, CL_DEVICE_MAX_MEM_ALLOC_SIZE, sizeof(cl_ulong), &max_mem_alloc_size, NULL);
    std::cout << "Max Memory Allocation Size: " << max_mem_alloc_size << std::endl;

    // Create OpenCL context
    cl_int context_result;
    cl_context context = clCreateContext(nullptr, 1, &device, nullptr, nullptr, &context_result);
    assert(context_result == CL_SUCCESS);

    // Create command queue with properties
    cl_int command_queue_result;
    cl_queue_properties queue_properties[] = {0};
    cl_command_queue queue = clCreateCommandQueueWithProperties(context, device, queue_properties, &command_queue_result);

    
    // Read kernel source from file
    std::ifstream kernel_file("vector_sum.cl");
    std::string kernel_code((std::istreambuf_iterator<char>(kernel_file)), std::istreambuf_iterator<char>());
    const char* program_source = kernel_code.c_str();
    size_t length = kernel_code.length();

    // Create program with source
    cl_int program_result;
    cl_program program = clCreateProgramWithSource(context, 1, &program_source, &length, &program_result);
    assert(program_result == CL_SUCCESS);

    // Build OpenCL program
    cl_int program_build_result = clBuildProgram(program, 1, &device, "", nullptr, nullptr);
    if (program_build_result != CL_SUCCESS){
        // If build fails, get build log
        char log[256];
        size_t log_length;
        cl_int program_build_result_info = clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, 256, log, &log_length);
        assert(program_build_result_info == CL_SUCCESS);
    }

    // Create OpenCL kernel
    cl_int kernel_result;
    cl_kernel kernel = clCreateKernel(program, "vector_sum", &kernel_result);
    assert(kernel_result == CL_SUCCESS);

    // Initialize arrays
    float vec_a_data [256];
    float vec_b_data [256];
    float vec_c_data [256];
    for (int i = 0; i < num_elements; ++i) {
        vec_a_data[i] = i;
        vec_b_data[i] = i * i;
        vec_c_data[i] = i * i * i;
    }

    // Create OpenCL buffers for input and output data
    // IMPUT
    cl_int vec_a_result;
    cl_mem vec_a = clCreateBuffer(context, CL_MEM_READ_ONLY, num_elements * sizeof(float), nullptr, &vec_a_result);
    cl_int enqueue_vec_a_result = clEnqueueWriteBuffer(queue, vec_a, CL_TRUE, 0, num_elements * sizeof(float), vec_a_data,0, nullptr, nullptr);

    cl_int vec_b_result;
    cl_mem vec_b = clCreateBuffer(context, CL_MEM_READ_ONLY, num_elements * sizeof(float), nullptr, &vec_b_result);
    cl_int enqueue_vec_b_result = clEnqueueWriteBuffer(queue, vec_b, CL_TRUE, 0, num_elements * sizeof(float), vec_b_data,0, nullptr, nullptr);

    cl_int vec_c_result;
    cl_mem vec_c = clCreateBuffer(context, CL_MEM_READ_ONLY, num_elements * sizeof(float), nullptr, &vec_c_result);
    cl_int enqueue_vec_c_result = clEnqueueWriteBuffer(queue, vec_c, CL_TRUE, 0, num_elements * sizeof(float), vec_c_data,0, nullptr, nullptr);

    // OUTPUT
    cl_int vec_final_result;
    cl_mem vec_final = clCreateBuffer(context, CL_MEM_WRITE_ONLY, num_elements * sizeof(float), nullptr, &vec_final_result);
    float vec_final_data [256] ;
    cl_int enqueue_vec_final_result = clEnqueueWriteBuffer(queue, vec_final, CL_TRUE, 0, num_elements * sizeof(float), vec_final_data,0, nullptr, nullptr);



    // Set kernel arguments
    cl_int kernel_arg_a_result = clSetKernelArg(kernel, 0, sizeof(cl_mem), &vec_a);
    assert(kernel_arg_a_result == CL_SUCCESS);
    cl_int kernel_arg_b_result = clSetKernelArg(kernel, 1, sizeof(cl_mem), &vec_b);
    assert(kernel_arg_b_result == CL_SUCCESS);
    cl_int kernel_arg_c_result = clSetKernelArg(kernel, 2, sizeof(cl_mem), &vec_c);
    assert(kernel_arg_c_result == CL_SUCCESS);
    cl_int kernel_arg_final_result = clSetKernelArg(kernel, 3, sizeof(cl_mem), &vec_final);
    assert(kernel_arg_final_result == CL_SUCCESS);

    // Execute the kernel
    size_t global_work_size = num_elements;
    size_t local_work_size = 1;
    cl_int enqueue_kernel_result = clEnqueueNDRangeKernel(queue, kernel, 1, 0, &global_work_size, &local_work_size, 0, nullptr, nullptr);
    assert(enqueue_kernel_result == CL_SUCCESS);

    // Read the result from the OpenCL buffer
    // Read the result from the OpenCL buffer
    cl_int enqueue_read_buffer_result = clEnqueueReadBuffer(queue, vec_final, CL_TRUE, 0, num_elements * sizeof(float), vec_final_data, 0, nullptr, nullptr);
    assert(enqueue_read_buffer_result == CL_SUCCESS);

    // Wait for all queued OpenCL commands to finish
    clFinish(queue);

    // Print the result
    std::cout << "Vector A: ";
    for (int i=0; i<256; ++i) {
        std::cout << vec_a_data[i] << ",";
    }
    std::cout << std::endl;

    std::cout << "Vector B: ";
    for (int i=0; i<256; ++i) {
        std::cout << vec_b_data[i] << ",";
    }
    std::cout << std::endl;

    std::cout << "Vector C: ";
    for (int i=0; i<256; ++i) {
        std::cout << vec_c_data[i] << ",";
    }
    std::cout << std::endl;

    std::cout << "Result: ";
    for (int i=0; i<256; ++i) {
        std::cout << vec_final_data[i] << ",";
    }
    std::cout << std::endl;

    return 0;
}
