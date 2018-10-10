/**
 * This file is used to test the algorithms
 */
#include <iostream>
#include <fstream>
#include "dsm_handle.h"
#include "reconstruct.h"
#include "morph.h"


/**
 * duplicate the float pointer, should clear later
 * @param data
 * @param y_input
 * @param x_input
 * @return
 */
float* duplicate(float *data, int y_input, int x_input) {
    float *result = (float *)malloc(sizeof(float) * y_input * x_input);
    return (float*)memcpy(result, data, sizeof(float) * y_input * x_input);
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

float* im_erode(float *img, int y_input, int x_input, int *mask, int mask_y, int mask_x) {
    Neighborhood_T nhood;
    if (mask) {
        int mask_size[2] = {mask_x, mask_y};
        nhood = create_neighborhood_general_template(mask, mask_size, NH_CENTER_MIDDLE_ROUNDDOWN);
    } else {
        nhood = nhMakeDefaultConnectivityNeighborhood();
    }
    int input_size[2] = {x_input, y_input};
    NeighborhoodWalker_T walker = nhMakeNeighborhoodWalker(nhood, input_size, NH_USE_ALL);

    float *out_img = duplicate(img, y_input, x_input);

    erodeGrayFlat(img, out_img, y_input * x_input, walker);

    nhDestroyNeighborhood(nhood);
    nhDestroyNeighborhoodWalker(walker);

    return out_img;
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

void write_data_to_txt(float* data, int y_input, int x_input, std::string file_name) {
    std::ofstream myfile;
    myfile.open (file_name);
    int index = 0;
    for (int i = 0; i < y_input; ++i) {
        for (int j = 0; j < x_input; ++j) {
            if (j != 0) myfile << " ";
            myfile << data[i * x_input + j];
        }
        if (i != y_input - 1) {
            myfile << std::endl;
        }
    }
    myfile.close();
}

int main() {
    dsm_handle handler("dsm.tif");
    float *data = handler.get_dsm_data();


//    handler.write_data_to_file("new_data.tif", data, handler.get_y_size(), handler.get_x_size());

    int num_elements = handler.get_x_size() * handler.get_y_size();
    int y_inputs = handler.get_y_size();
    int x_inputs = handler.get_x_size();

    int mask[225];
    for (int i = 0; i < 225; ++i) mask[i] = 1;

    float *imer = im_erode(data, y_inputs, x_inputs, mask, 15, 15);

    float *reconstruct_result = im_reconstruct(imer, data, handler.get_y_size(), handler.get_x_size());

    float min = 1111;
    float max = -1111111;
    for (int i = 0; i < y_inputs; ++i) {
        for (int j = 0; j < x_inputs; ++j) {
            reconstruct_result[i * x_inputs + j] = data[i * x_inputs + j] - reconstruct_result[i * x_inputs + j];
            max = reconstruct_result[i * x_inputs + j] > max ? reconstruct_result[i * x_inputs + j] : max;
            min = reconstruct_result[i * x_inputs + j] < min ? reconstruct_result[i * x_inputs + j] : min;
        }
    }
    std::cout << max << ", " << min << std::endl;



//    print_data(imer, y_inputs, x_inputs);

    write_data_to_txt(data, y_inputs, x_inputs, "new_data.txt");
    write_data_to_txt(imer, y_inputs, x_inputs, "imer.txt");
    write_data_to_txt(reconstruct_result, y_inputs, x_inputs, "reconstruct.txt");
//    handler.write_data_to_file("new_data.tif", data, y_inputs, x_inputs);
//    handler.write_data_to_file("erode1.tif", reconstruct_result, y_inputs, x_inputs);
//
    free(imer);
    free(reconstruct_result);
}
