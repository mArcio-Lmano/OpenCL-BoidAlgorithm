#define CL_TARGET_OPENCL_VERSION 300

#include <iostream>
#include <CL/cl.h>
#include <cassert>

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

    return 0;
}
