//
// Created by xinyuangui on 9/8/18.
//

#ifndef TOPHAT_RECODE_NEIGHBORHOOD_H
#define TOPHAT_RECODE_NEIGHBORHOOD_H


#include <cstddef>
#include <cstdlib>

#define NH_USE_ALL       0
#define NH_SKIP_TRAILING 1
#define NH_SKIP_LEADING  2
#define NH_SKIP_CENTER   4

#define NH_CENTER_MIDDLE_ROUNDUP   0
#define NH_CENTER_MIDDLE_ROUNDDOWN 2
#define NH_CENTER_UL               4
#define NH_CENTER_LR               8
#define NUM_DIMS                   2

typedef struct Neighborhood_tag {
    /**
     * array_coords is a num_of_neighbors-by-2 arrays containing relative offsets.
     */
    ptrdiff_t *array_coords;
    int num_neighbors;
} *Neighborhood_T;

typedef struct NeighborhoodWalker_tag {
    /**
     * array_coords is a num_of_neighbors-by-2 arrays containing relative offsets.
     */
    ptrdiff_t *array_coords;

    /**
     * an array containing linear neighbor offsets, computed with respect to given image size
     */
    ptrdiff_t *neighbor_offsets;

    int *image_size;

    /**
     * containing the array coordinates for the image pixel whose neighborhood about to walk
     */
    int *center_coords;

    /**
     * containing the cumulative product of the image size array.
     * used in the sub_to_ind and ind_to_sub calculations
     */
    int *cumprod;

    /**
     * linear index of the point we are about to walk
     */
    int pixel_offset;

    /**
     * used to filter out certain neighbors in a neighborhood walk
     */
    bool * use;

    /**
     * index of the next neighbor in the walk
     */
     ptrdiff_t next_neighbor_idx;
     int num_neighbors;

     /**
      * make sure walker isn't used until nhSetWalkerLocation is called
      */
     bool ready_for_use;
} *NeighborhoodWalker_T;

Neighborhood_T nhMakeNeighborhood(int D, int center_location);
NeighborhoodWalker_T nhMakeNeighborhoodWalker(Neighborhood_T nhood, const int *input_size, unsigned int flags);
Neighborhood_T nhMakeDefaultConnectivityNeighborhood();
void nhReflectNeighborhood(Neighborhood_T nhood);
void nhDestroyNeighborhoodWalker(NeighborhoodWalker_T walker);
void nhDestroyNeighborhood(Neighborhood_T nhood);
void nhSetWalkerLocation(NeighborhoodWalker_T walker, int p);
bool nhGetNextInboundsNeighbor(NeighborhoodWalker_T walker, int *p, int *idx);

//TODO: nhCheckDomain()
//TODO: nhCheckConnectivityDomain

int sub_to_ind(int *coords, int *cumprod);
ptrdiff_t sub_to_ind_signed(ptrdiff_t *coords, int *cumprod);
void ind_to_sub(int p, int *cumprod, int *coords);
void ind_to_sub(int p, int *cumprod, ptrdiff_t *coords);

ptrdiff_t *nhGetWalkerNeighborOffsets(NeighborhoodWalker_T walker);
Neighborhood_T allocate_neighborhood(int num_neighbors);
int ngGetNumNeighbors(NeighborhoodWalker_T walker);

/**
 * count nonzero elements of real part of 2-d array
 * @tparam _T
 * @param D
 * @param size
 * @return
 */
template <typename _T>
int num_nonzeros(_T *D, int *size) {
    int num_elements = size[0] * size[1];
    int count = 0;
    for (int i = 0; i < num_elements; ++i) {
        if (D[i]) ++count;
    }
    return count;
}

/**
 *
 * @tparam _T1
 * @param pr
 * @param size 1*2 array, {cols, rows}
 * @param center_location
 * @return
 */
template <typename _T1>
Neighborhood_T create_neighborhood_general_template(_T1 *pr, int* size, int center_location) {
    int num_neighbors = num_nonzeros(pr, size);
    int num_elements = size[0] * size[1];
    Neighborhood_T result = allocate_neighborhood(num_neighbors);
    int *cumprod;
    cumprod = (int *)malloc(NUM_DIMS * sizeof(*cumprod));
    cumprod[0] = 1;
    for (int i = 1; i < NUM_DIMS; ++i) {
        cumprod[i] = cumprod[i - 1] * size[i - 1];
    }

    int count = 0;
    for (int i = 0; i < num_elements; ++i) {
        if (*pr) {
            ptrdiff_t *coords = result->array_coords + count * NUM_DIMS;
            ind_to_sub(i, cumprod, coords);
            if (center_location == NH_CENTER_MIDDLE_ROUNDDOWN) {
                /**
                 * substract the location of the center pixel from the neighbor coordinates
                 */
                 for (int j = 0; j < NUM_DIMS; ++j) {
                     coords[j] -= (size[j] - 1) / 2;
                 }
            } else if (center_location == NH_CENTER_UL) {
                /**
                 * no change
                 */
            } else if (center_location == NH_CENTER_LR) {
                for (int j = 0; j < NUM_DIMS; ++j) {
                    coords[j] -= (size[j] - 1);
                }
            }

            if (center_location == NH_CENTER_MIDDLE_ROUNDUP) {
                /**
                 * substract the location of center pixel from the neighbor coordinates
                 */
                 for (int j = 0; j < NUM_DIMS; ++j) {
                     coords[j] -= (size[j] - 1) / 2 + ((size[j] - 1) % 2 ? 1 : 0);
                 }
            }
            ++count;
        }
        ++pr;
    }
    free(cumprod);
    return result;
}

#endif //TOPHAT_RECODE_NEIGHBORHOOD_H
