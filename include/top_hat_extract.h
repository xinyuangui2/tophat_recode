//
// Created by Xinyuan Gui on 10/9/18.
//

#ifndef TOPHAT_RECODE_REORGANIZE_TOP_HAT_EXTRACT_H
#define TOPHAT_RECODE_REORGANIZE_TOP_HAT_EXTRACT_H


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

/**
 * return the tophat result
 * @param origin_img
 * @param y_input rows of the image
 * @param x_input cols of the image
 * @param mask mask for the erode neighbor
 * @param mask_y rows of the mask
 * @param mask_x cols of the mask
 * @return tophat_result
 */
float* top_hat_extract(float *origin_img, int y_input, int x_input,
        int *mask, int mask_y, int mask_x) {
    float *imer = im_erode(origin_img, y_input, x_input, mask, mask_y, mask_y);

    float *reconstruct_result = im_reconstruct(imer, origin_img, y_input, x_input);

    for (int i = 0; i < y_input; ++i) {
        for (int j = 0; j < x_input; ++j) {
            reconstruct_result[i * x_input + j] = origin_img[i * x_input + j] - reconstruct_result[i * x_input + j];
        }
    }

    free(imer);

    return reconstruct_result;
}

#endif //TOPHAT_RECODE_REORGANIZE_TOP_HAT_EXTRACT_H
