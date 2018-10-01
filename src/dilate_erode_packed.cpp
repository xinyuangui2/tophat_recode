//
// Created by xinyuangui on 9/19/18.
//
#include "morph.h"
#include <cstdlib>

#define BITS_PER_WORD 32
#define LEFT_SHIFT(x,shift) (shift == 0 ? x : (shift == BITS_PER_WORD ? 0 : x << shift))
#define RIGHT_SHIFT(x,shift) (shift == 0 ? x : (shift == BITS_PER_WORD ? 0 : x >> shift))

/*
 * dilate_packed_uint32
 * Packed binary dilation
 *
 * Inputs
 * ======
 * In            - pointer to first element of input array
 * M             - number of rows of packed input array
 * N             - number of columns of packed input array
 * rc_offsets    - Row-column offset locations corresponding to
 *                 each element of the structuring element; storage
 *                 order is that for a MATLAB array, num_neighbors-by-2
 * num_neighbors - number of neighbors in the structuring element
 *
 * Output
 * ======
 * Out           - pointer to first element of output array
 */
void dilate_packed_uint32(unsigned int *In, unsigned int *Out, int MM, int NN,
                          ptrdiff_t *rc_offsets, int num_neighbors)
{
    ptrdiff_t M = static_cast<ptrdiff_t>(MM);
    ptrdiff_t N = static_cast<ptrdiff_t>(NN);
    ptrdiff_t *column_offset;
    ptrdiff_t *row_offset1;
    ptrdiff_t *row_offset2;
    ptrdiff_t *bit_shift1;
    ptrdiff_t *bit_shift2;
    unsigned int val;
    unsigned int shifted_val;

    column_offset = (ptrdiff_t *) std::calloc(num_neighbors, sizeof(*column_offset));
    row_offset1 = (ptrdiff_t *) std::calloc(num_neighbors, sizeof(*row_offset1));
    row_offset2 = (ptrdiff_t *) std::calloc(num_neighbors, sizeof(*row_offset2));
    bit_shift1 = (ptrdiff_t *) std::calloc(num_neighbors, sizeof(*bit_shift1));
    bit_shift2 = (ptrdiff_t *) std::calloc(num_neighbors, sizeof(*bit_shift2));

    for (int k = 0; k < num_neighbors; k++)
    {
        column_offset[k] = rc_offsets[k + num_neighbors];
        ptrdiff_t r = rc_offsets[k];
        row_offset1[k] = (ptrdiff_t) floor((double) r / BITS_PER_WORD);
        row_offset2[k] = row_offset1[k] + 1;
        bit_shift1[k] = r - BITS_PER_WORD*row_offset1[k];
        bit_shift2[k] = BITS_PER_WORD - bit_shift1[k];
    }

    for (ptrdiff_t c = 0; c < N; c++)
    {
        for (ptrdiff_t r = 0; r < M; r++)
        {
            val = *In++;
            if (val != 0)
            {
                for (int k = 0; k < num_neighbors; k++)
                {
                    ptrdiff_t cc = c + column_offset[k];
                    if ((cc >= 0) && (cc < N))
                    {
                        ptrdiff_t rr = r + row_offset1[k];
                        if ((rr >= 0) && (rr < M))
                        {
                            shifted_val = LEFT_SHIFT(val,bit_shift1[k]);
                            Out[cc*M + rr] |= shifted_val;
                        }
                        rr = r + row_offset2[k];
                        if ((rr >= 0) && (rr < M))
                        {
                            shifted_val = RIGHT_SHIFT(val,bit_shift2[k]);
                            Out[cc*M + rr] |= shifted_val;
                        }
                    }
                }
            }
        }
    }

    free(column_offset);
    free(row_offset1);
    free(row_offset2);
    free(bit_shift1);
    free(bit_shift2);
}


/*
 * erode_packed_uint32
 * Packed binary erosion
 *
 * Inputs
 * ======
 * In            - pointer to first element of input array
 * M             - number of rows of packed input array
 * N             - number of columns of packed input array
 * rc_offsets    - Row-column offset locations corresponding to
 *                 each element of the structuring element; storage
 *                 order is that for a MATLAB array, num_neighbors-by-2
 * num_neighbors - number of neighbors in the structuring element
 * unpacked_M    - number of rows of unpacked input array
 *
 * Output
 * ======
 * Out           - pointer to first element of output array
 */
void erode_packed_uint32(unsigned int *In, unsigned int *Out, int MM, int NN,
                         ptrdiff_t *rc_offsets, int num_neighbors,
                         int unpacked_M)
{
    ptrdiff_t M = static_cast<ptrdiff_t>(MM);
    ptrdiff_t N = static_cast<ptrdiff_t>(NN);

    ptrdiff_t *column_offset;
    ptrdiff_t *row_offset1;
    ptrdiff_t *row_offset2;
    ptrdiff_t *bit_shift1;
    ptrdiff_t *bit_shift2;
    unsigned int val;
    unsigned int shifted_val;
    unsigned int last_row_mask;
    int num_real_bits_in_last_row;

    column_offset = (ptrdiff_t *) std::calloc(num_neighbors, sizeof(*column_offset));
    row_offset1 = (ptrdiff_t *) std::calloc(num_neighbors, sizeof(*row_offset1));
    row_offset2 = (ptrdiff_t *) std::calloc(num_neighbors, sizeof(*row_offset2));
    bit_shift1 = (ptrdiff_t *) std::calloc(num_neighbors, sizeof(*bit_shift1));
    bit_shift2 = (ptrdiff_t *) std::calloc(num_neighbors, sizeof(*bit_shift2));

    for (int k = 0; k < 2*num_neighbors; k++)
    {
        rc_offsets[k] = -rc_offsets[k];
    }

    for (int k = 0; k < num_neighbors; k++)
    {
        column_offset[k] = (ptrdiff_t) rc_offsets[k + num_neighbors];
        ptrdiff_t r = (ptrdiff_t) rc_offsets[k];
        row_offset1[k] = (ptrdiff_t) floor((double) r / BITS_PER_WORD);
        row_offset2[k] = row_offset1[k] + 1;
        bit_shift1[k] = r - BITS_PER_WORD*row_offset1[k];
        bit_shift2[k] = BITS_PER_WORD - bit_shift1[k];
    }

    num_real_bits_in_last_row = unpacked_M % BITS_PER_WORD;
    if (num_real_bits_in_last_row == 0)
    {
        num_real_bits_in_last_row = BITS_PER_WORD;
    }

    last_row_mask = 0;
    for (int k = 0; k < num_real_bits_in_last_row; k++)
    {
        last_row_mask |= 1 << k;
    }

    for (ptrdiff_t c = 0; c < N; c++)
    {
        for (ptrdiff_t r = 0; r < M; r++)
        {
            val = ~(*In++);
            if (r == (M - 1))
            {
                val = val & last_row_mask;
            }
            if (val != 0)
            {
                for (int k = 0; k < num_neighbors; k++)
                {
                    ptrdiff_t cc = c + column_offset[k];
                    if ((cc >= 0) && (cc < N))
                    {
                        ptrdiff_t rr = r + row_offset1[k];
                        if ((rr >= 0) && (rr < M))
                        {
                            shifted_val = LEFT_SHIFT(val,bit_shift1[k]);
                            Out[cc*M + rr] |= shifted_val;
                        }
                        rr = r + row_offset2[k];
                        if ((rr >= 0) && (rr < M))
                        {
                            shifted_val = RIGHT_SHIFT(val,bit_shift2[k]);
                            Out[cc*M + rr] |= shifted_val;
                        }
                    }
                }
            }
        }
    }

    for (ptrdiff_t k = 0; k < M*N; k++)
    {
        Out[k] = ~Out[k];
    }

    /*
     * Mask out extraneous bits in the last row.
     */
    for (ptrdiff_t c = 0; c < N; c++)
    {
        Out[c*M + M - 1] &= last_row_mask;
    }

    free(column_offset);
    free(row_offset1);
    free(row_offset2);
    free(bit_shift1);
    free(bit_shift2);
}

