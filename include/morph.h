//
// Created by xinyuangui on 9/19/18.
//

#ifndef TOPHAT_RECODE_MORPH_H
#define TOPHAT_RECODE_MORPH_H

#include <math.h>
#include <cstdint>
#include "neighborhood.h"

#define BITS_PER_WORD 32
#define LEFT_SHIFT(x,shift) (shift == 0 ? x : (shift == BITS_PER_WORD ? 0 : x << shift))
#define RIGHT_SHIFT(x,shift) (shift == 0 ? x : (shift == BITS_PER_WORD ? 0 : x >> shift))

void dilate_logical(bool *in, bool *out, int num_elements, NeighborhoodWalker_T walker);
void dilate_logical_twod(bool *in, bool *out, int M, int N, Neighborhood_T nhood, NeighborhoodWalker_T walker);

void erode_logical(bool *In, bool *Out, int num_elements,
                   NeighborhoodWalker_T walker);
void erode_logical_twod(bool *In, bool *Out, int M, int N,
                        Neighborhood_T nhood, NeighborhoodWalker_T walker);
void dilateones33_edge_pixels(bool *In, bool *Out,
                              NeighborhoodWalker_T walker,
                              ptrdiff_t M, ptrdiff_t N,
                              int num_elements);
void dilateones33_interior_pixels(bool *input_data, bool *out_data,
                                  ptrdiff_t M, ptrdiff_t N);
void erodeones33_edge_pixels(bool *In, bool *Out,
                             NeighborhoodWalker_T walker,
                             ptrdiff_t M, ptrdiff_t N,
                             int num_elements);
void erodeones33_interior_pixels(bool *input_data, bool *out_data,
                                 ptrdiff_t M, ptrdiff_t N);

//////////////////////////////////////////////////////////////////////////////
// Perform flat grayscale dilation on a uint8 array.
//
// Inputs
// ======
// In             - pointer to first element of input array
// num_elements   - number of elements in input and output arrays
// walker         - neighborhood walker corresponding to reflected structuring
//                  element
//
// Output
// ======
// Out            - pointer to first element of output array
//
// Note that implementing dilation properly with this function requires
// passing in a neighborhood walker constructed from a reflected neighborhood.
//////////////////////////////////////////////////////////////////////////////
template<typename _t>
void dilateGrayFlat(_t *In, _t *Out, int num_elements,
                    NeighborhoodWalker_T walker)
{
    for (int p = 0; p < num_elements; p++)
    {
        _t val;
        _t new_val;
        int q;

        setMin(&val);
        nhSetWalkerLocation(walker, p);
        while (nhGetNextInboundsNeighbor(walker, &q, NULL))
        {
            new_val = In[q];
            if (new_val > val)
            {
                val = new_val;
            }
        }
        Out[p] = val;
    }
}

//////////////////////////////////////////////////////////////////////////////
// Perform flat grayscale dilation on input array.
//
// Inputs
// ======
// In             - pointer to first element of input array
// num_elements   - number of elements in input and output arrays
// walker         - neighborhood walker corresponding to reflected structuring
//                  element
//
// Output
// ======
// Out            - pointer to first element of output array
//////////////////////////////////////////////////////////////////////////////
template<typename _t>
void erodeGrayFlat(_t *In, _t *Out, int num_elements,
                   NeighborhoodWalker_T walker)
{
    for (int p = 0; p < num_elements; p++)
    {
        int q;
        _t val;
        _t new_val;

        bool val_set = false;
        nhSetWalkerLocation(walker, p);
        while (nhGetNextInboundsNeighbor(walker, &q, NULL))
        {
            new_val = In[q];
            if (!val_set || new_val < val)
            {
                val_set = true;
                val = new_val;
            }
        }
        Out[p] = val;
    }
}


void dilate_gray_nonflat_uint8(uint8_t *In, uint8_t *Out, int num_elements,
                               NeighborhoodWalker_T walker, double *heights);

void dilate_gray_nonflat_uint16(uint16_t *In, uint16_t *Out, int num_elements,
                                NeighborhoodWalker_T walker, double *heights);

void dilate_gray_nonflat_uint32(uint32_t *In, uint32_t *Out, int num_elements,
                                NeighborhoodWalker_T walker, double *heights);

void dilate_gray_nonflat_int8(int8_t *In, int8_t *Out, int num_elements,
                              NeighborhoodWalker_T walker, double *heights);

void dilate_gray_nonflat_int16(int16_t *In, int16_t *Out, int num_elements,
                               NeighborhoodWalker_T walker, double *heights);

void dilate_gray_nonflat_int32(int32_t *In, int32_t *Out, int num_elements,
                               NeighborhoodWalker_T walker, double *heights);

void dilate_gray_nonflat_single(float *In, float *Out, int num_elements,
                                NeighborhoodWalker_T walker, double *heights);

void dilate_gray_nonflat_double(double *In, double *Out, int num_elements,
                                NeighborhoodWalker_T walker, double *heights);

void erode_gray_nonflat_uint8(uint8_t *In, uint8_t *Out, int num_elements,
                              NeighborhoodWalker_T walker, double *heights);

void erode_gray_nonflat_uint16(uint16_t *In, uint16_t *Out, int num_elements,
                               NeighborhoodWalker_T walker, double *heights);

void erode_gray_nonflat_uint32(uint32_t *In, uint32_t *Out, int num_elements,
                               NeighborhoodWalker_T walker, double *heights);

void erode_gray_nonflat_int8(int8_t *In, int8_t *Out, int num_elements,
                             NeighborhoodWalker_T walker, double *heights);

void erode_gray_nonflat_int16(int16_t *In, int16_t *Out, int num_elements,
                              NeighborhoodWalker_T walker, double *heights);

void erode_gray_nonflat_int32(int32_t *In, int32_t *Out, int num_elements,
                              NeighborhoodWalker_T walker, double *heights);

void erode_gray_nonflat_single(float *In, float *Out, int num_elements,
                               NeighborhoodWalker_T walker, double *heights);

void erode_gray_nonflat_double(double *In, double *Out, int num_elements,
                               NeighborhoodWalker_T walker, double *heights);

void dilate_packed_uint32(unsigned int *In, unsigned int *Out, int M, int N,
                          ptrdiff_t *rc_offsets, int num_neighbors);

void erode_packed_uint32(unsigned int *In, unsigned int *Out, int M, int N,
                         ptrdiff_t *rc_offsets, int num_neighbors,
                         int unpacked_M);


#endif //TOPHAT_RECODE_MORPH_H
