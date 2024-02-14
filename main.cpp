#define CL_TARGET_OPENCL_VERSION 300

#include <iostream>
#include <CL/cl.h>
#include <cassert>
#include <cstring>

int main() {
    cl_platform_id platforms[64];
    unsigned int platform_count;

    cl_int platform_result = clGetPlatformIDs(64, platforms, &platform_count);
    assert(platform_result == CL_SUCCESS);

    cl_device_id  device = nullptr;

    for (int i = 0; i < platform_count; ++i) {
        cl_device_id devices[64];
        unsigned int devices_count;
        cl_int devices_result = clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_GPU, 64, devices, &devices_count);

        if (devices_result == CL_SUCCESS) {
            for (int j = 0; j < devices_count; ++j) {
                char vendor_name[256];
                size_t vendor_name_leng;
                cl_int device_info_result = clGetDeviceInfo(devices[j], CL_DEVICE_VENDOR, sizeof(vendor_name), vendor_name, &vendor_name_leng);

                if (device_info_result == CL_SUCCESS) {
                    std::cout << "Vendor: " << vendor_name << std::endl;

                    cl_device_type device_type;
                    clGetDeviceInfo(devices[j], CL_DEVICE_TYPE, sizeof(device_type), &device_type, NULL);

                    if (device_type == CL_DEVICE_TYPE_GPU){
                        std::cout << "Device type: GPU" << std::endl;
                        device = devices[j];
                    }
                    else if (device_type == CL_DEVICE_TYPE_ACCELERATOR)
                        std::cout << "Device type: Accelerator (Possibly APU)" << std::endl;
                    else
                        std::cout << "Device type: Unknown" << std::endl;

                    char device_name[256];
                    size_t device_name_len;
                    clGetDeviceInfo(devices[j], CL_DEVICE_NAME, sizeof(device_name), device_name, &device_name_len);
                    std::cout << "Device name: " << device_name << std::endl;
                }
            }
        }
    }
    cl_int context_result;
    cl_context context = clCreateContext(nullptr, 1, &device, nullptr, nullptr, &context_result);
    assert(context_result == CL_SUCCESS);

    cl_int command_queue_result;
    cl_command_queue queue = clCreateCommandQueue(context,device, 0, &command_queue_result);

    const char* program_source = "__kernel void vector_sum(__global const float* a, __global const float* b, __global float* c) { \
                                        int idx = get_global_id(0); \
                                        c[idx] = a[idx] + b[idx]; \
                                    }";
    size_t length = strlen(program_source);
    cl_int program_result;
    cl_program program = clCreateProgramWithSource(context, 1, &program_source, &length, &program_result);
    assert(program_result == CL_SUCCESS);

    cl_int program_build_result = clBuildProgram(program, 1, &device, "", nullptr, nullptr);
    if (program_build_result != CL_SUCCESS){
        char log[256];
        size_t log_length;
        cl_int program_build_result_info = clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, 256, log, &log_length);
        assert(program_build_result_info == CL_SUCCESS);
    }

    cl_int kernel_result;
    cl_kernel kernel = clCreateKernel(program, "vector_sum", &kernel_result);
    assert(kernel_result == CL_SUCCESS);


    cl_int vec_a_result;
    cl_mem vec_a = clCreateBuffer(context, CL_MEM_READ_ONLY, 2 * sizeof(float), nullptr, &vec_a_result);

    float vec_a_data [] {1.5f, 3.7f};
    cl_int enqueue_vec_a_result = clEnqueueWriteBuffer(queue, vec_a, CL_TRUE, 0, 2 * sizeof(float), vec_a_data,0, nullptr, nullptr);


    cl_int vec_b_result;
    cl_mem vec_b = clCreateBuffer(context, CL_MEM_READ_ONLY, 2 * sizeof(float), nullptr, &vec_b_result);

    float vec_b_data [] {3.5f, 1.7f};
    cl_int enqueue_vec_b_result = clEnqueueWriteBuffer(queue, vec_b, CL_TRUE, 0, 2 * sizeof(float), vec_b_data,0, nullptr, nullptr);


    cl_int vec_c_result;
    cl_mem vec_c = clCreateBuffer(context, CL_MEM_READ_ONLY, 2 * sizeof(float), nullptr, &vec_c_result);

    float vec_c_data [] {2.2f, 4.1f};
    cl_int enqueue_vec_c_result = clEnqueueWriteBuffer(queue, vec_c, CL_TRUE, 0, 2 * sizeof(float), vec_c_data,0, nullptr, nullptr);


    cl_int kernel_arg_a_result = clSetKernelArg(kernel, 0, sizeof(cl_mem), &vec_a);
    assert(kernel_arg_a_result == CL_SUCCESS);
    cl_int kernel_arg_b_result = clSetKernelArg(kernel, 1, sizeof(cl_mem), &vec_b);
    assert(kernel_arg_b_result == CL_SUCCESS);
    cl_int kernel_arg_c_result = clSetKernelArg(kernel, 2, sizeof(cl_mem), &vec_c);
    assert(kernel_arg_c_result == CL_SUCCESS);


    size_t global_work_size = 2;
    size_t local_work_size = 2;
    cl_int enqueue_kernel_result = clEnqueueNDRangeKernel(queue, kernel, 1, 0, &global_work_size, &local_work_size, 0, nullptr, nullptr);
    assert(enqueue_kernel_result == CL_SUCCESS);

    float vec_data[2];
    cl_int enqueue_read_buffer_result = clEnqueueReadBuffer(queue, vec_c, CL_TRUE, 0, 2 * sizeof(float), vec_data, 0, nullptr, nullptr);
    assert(enqueue_read_buffer_result == CL_SUCCESS);


    clFinish(queue);

    std::cout << "Result: ";
    for (int i=0; i<2; ++i) {
        std::cout << vec_data[i] << ",";
    }
    std::cout << std::endl;

    return 0;
}
