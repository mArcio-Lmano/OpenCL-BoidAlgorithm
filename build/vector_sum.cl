// vector_sum.cl
__kernel void vector_sum(__global const float* a, __global const float* b, __global const float* c, __global float* d) {
    int idx = get_global_id(0);
    float i = a[idx];  // Element from vector a
    float i_square = b[idx];  // Element from vector b
    float i_cube = c[idx];  // Element from vector c
    float sum = i + i_square + i_cube;  // Compute the sum
    d[idx] = sum;  // Store the result in the output vector
}