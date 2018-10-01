/**
 * This file is used to test the algorithms
 */
#include <iostream>
#include "dsm_handle.h"
#include "reconstruct.h"


/**
 * duplicate the float pointer, should clear later
 * @param data
 * @param y_input
 * @param x_input
 * @return
 */
float* duplicate(float *data, int y_input, int x_input) {
    float *result = (float *)malloc(sizeof(float) * y_input * x_input);
    (float*)memcpy(result, data, sizeof(float) * y_input * x_input);
}

float* im_reconstruct(float *imer, float *img, int y_input, int x_input) {
    Neighborhood_T nhood;
    NeighborhoodWalker_T trailing_walker;
    NeighborhoodWalker_T leading_walker;
    NeighborhoodWalker_T walker;

    // the reconstruction algorithm works in-place on a copy of the
    // input marker image. at the end, this copy will hold the result
    float *J = duplicate(imer, y_input, x_input);
    float *I = img;
    int num_elements = y_input * x_input;
    int input_size[2] = {x_input, y_input};
    nhood = nhMakeDefaultConnectivityNeighborhood();

    trailing_walker = nhMakeNeighborhoodWalker(nhood, input_size,
                                               NH_SKIP_CENTER | NH_SKIP_LEADING);
    leading_walker = nhMakeNeighborhoodWalker(nhood, input_size,
                                              NH_SKIP_CENTER | NH_SKIP_TRAILING);
    walker = nhMakeNeighborhoodWalker(nhood, input_size,
                                      NH_SKIP_CENTER);
    nhDestroyNeighborhood(nhood);

    compute_reconstruction(J, I, num_elements, walker, trailing_walker, leading_walker);

    nhDestroyNeighborhoodWalker(trailing_walker);
    nhDestroyNeighborhoodWalker(leading_walker);
    nhDestroyNeighborhoodWalker(walker);

    return J;
}

void print_data(float *data, int y_input, int x_input) {
    int index = 0;
    for (int i = 0; i < y_input; ++i) {
        for (int j = 0; j < x_input; ++j) {
            std::cout << data[index++] << ", ";
        }
        std::cout << std::endl;
    }
}

int main() {
    dsm_handle handler("dsm.tif");
    float *data = handler.get_dsm_data();
    float *imer = duplicate(data, handler.get_x_size(), handler.get_y_size());

    int num_elements = handler.get_x_size() * handler.get_y_size();
    int y_inputs = handler.get_y_size();
    int x_inputs = handler.get_x_size();

    float *reconstruct_result = im_reconstruct(imer, data, handler.get_y_size(), handler.get_x_size());

    float *diff = (float*)malloc(sizeof(float) * num_elements);
    for (int i = 0; i < num_elements; ++i) {
        diff[i] = reconstruct_result[i] - data[i];
    }

    print_data(diff, y_inputs, x_inputs);

    free(imer);
    free(reconstruct_result);
}
