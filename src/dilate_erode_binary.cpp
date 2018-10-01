//
// Created by xinyuangui on 9/19/18.
//
#include "morph.h"

inline ptrdiff_t my_abs(ptrdiff_t v) {
    v = v < 0 ? -v : v;
    return (int)v;
}

inline int my_min(ptrdiff_t offset, int dim) {
    offset = (ptrdiff_t)dim > offset ? (int)offset : dim;
    return offset;
}

inline int my_max(ptrdiff_t offset, int dim) {
    offset = (ptrdiff_t) dim < offset ? (int) offset : dim;
    return offset;
}

/*
 * dilate_logical
 * Perform binary dilation on a logical array.
 *
 * inputs
 * ======
 * in            - pointer to first element of input array
 * num_elements  - number of elements in input and output arrays
 * walker        - neighborhood walker representing the structuring element
 *
 * Output ====== Out - pointer to first element of output array
 */
 void dilate_logical(bool *in, bool *out, int num_elements, NeighborhoodWalker_T walker) {
     for (int p = 0; p < num_elements; ++p) {
         if (in[p]) {
             int q;
             nhSetWalkerLocation(walker, p);
             while (nhGetNextInboundsNeighbor(walker, &q, NULL)) {
                 out[q] = 1;
             }
         }
     }
 }

/*
 * dilate_logical_twod
 * Perform binary dilation on edge pixels in logical array.
 *
 * inputs
 * ======
 * in            - pointer to first element of input array
 * M             - number of rows in input and output arrays
 * N             - number of columns in input and output arrays
 * nhood         - neighborhood representing the structuring element
 * walker        - neighborhood walker traverses neighborhood in image
 *
 * Output ====== Out - pointer to first element of output array
 */
void dilate_logical_twod(bool *in, bool *out, int M, int N, Neighborhood_T nhood, NeighborhoodWalker_T walker) {
    // determine r, r endpoints of interior image using minimum and maximum offsets in nhood
    ptrdiff_t min_r_offset = 0;
    ptrdiff_t min_c_offset = 0;
    ptrdiff_t max_r_offset = 0;
    ptrdiff_t max_c_offset = 0;
    int c_interior_start = 0;
    int c_interior_end = 0;
    int r_interior_start = 0;
    int r_interior_end = 0;

    int num_elements = M * N;
    int idx, k;
    for (k = 0; k < nhood->num_neighbors; ++k) {
        idx = 2 * k;

        if (nhood->array_coords[idx] < min_r_offset)
        {
            min_r_offset = nhood->array_coords[idx];
        }
        if (nhood->array_coords[idx] > max_r_offset)
        {
            max_r_offset = nhood->array_coords[idx];
        }
        if (nhood->array_coords[idx+1] < min_c_offset)
        {
            min_c_offset = nhood->array_coords[idx+1];
        }
        if (nhood->array_coords[idx+1] > max_c_offset)
        {
            max_c_offset = nhood->array_coords[idx+1];
        }
    }

    c_interior_start = my_min(my_abs(min_c_offset), N);
    c_interior_end = my_max((ptrdiff_t) N - max_c_offset, 0);
    r_interior_start = my_min(my_abs(min_r_offset), M);
    r_interior_end = my_max((ptrdiff_t) M - max_r_offset, 0);

    int r,c;
    int idxn,q;

    //process interior pixels
    for (c = c_interior_start; c < c_interior_end; c++)
    {
        idx = M * c + r_interior_start;

        for (r = r_interior_start; r < r_interior_end; r++)
        {
            if(in[idx])
            {
                for(k = 0; k < walker->num_neighbors; k++)
                {
                    idxn = idx + (int) walker->neighbor_offsets[k];
                    out[idxn] = 1;
                }
            }
            idx++;
        }

    }

    //process left edge
    int end_idx = M * c_interior_start;

    for (idx = 0; idx < end_idx ; idx++)
    {
        if(in[idx])
        {
            nhSetWalkerLocation(walker, idx);
            while (nhGetNextInboundsNeighbor(walker, &q, NULL))
            {
                out[q] = 1;
            }
        }
    }

    //process right edge
    int starting_idx = M * c_interior_end;

    for (idx = starting_idx; idx < num_elements; idx++)
    {

        if(in[idx])
        {

            nhSetWalkerLocation(walker, idx);
            while (nhGetNextInboundsNeighbor(walker, &q, NULL))
            {
                out[q] = 1;
            }
        }
    }


    // process top and bottom edges
    int idx_top,idx_bottom;

    for (c = c_interior_start; c < c_interior_end; c++)
    {

        // top edge

        idx_top = M * c;

        for (r = 0; r < r_interior_start; r++)
        {

            if(in[idx_top])
            {

                nhSetWalkerLocation(walker, idx_top);
                while (nhGetNextInboundsNeighbor(walker, &q, NULL))
                {
                    out[q] = 1;
                }
            }
            idx_top++;
        }

        // bottom edge

        idx_bottom = M * c + r_interior_end;

        for (r = r_interior_end; r < M; r++)
        {

            if(in[idx_bottom])
            {

                nhSetWalkerLocation(walker, idx_bottom);
                while (nhGetNextInboundsNeighbor(walker, &q, NULL))
                {
                    out[q] = 1;
                }
            }
            idx_bottom++;
        }
    }
}

/*
 * erode_logical
 * Perform binary erosion on a logical array.
 *
 * Inputs
 * ======
 * In            - pointer to first element of input array
 * num_elements  - number of elements in input and output arrays
 * walker        - neighborhood walker representing the structuring element
 *
 * Output
 * ======
 * Out           - pointer to first element of output array
 */
void erode_logical(bool *In, bool *Out, int num_elements,
                   NeighborhoodWalker_T walker)
{

    for (int p = 0; p < num_elements; p++)
    {
        int q;

        Out[p] = 1;
        nhSetWalkerLocation(walker, p);
        while (nhGetNextInboundsNeighbor(walker, &q, NULL))
        {
            if (In[q] == 0)
            {
                Out[p] = 0;
                break;
            }
        }
    }
}

/*
 * erode_logical_twod
 * Perform binary erosion on edge pixels in logical array.
 *
 * Inputs
 * ======
 * In            - pointer to first element of input array
 * M             - number of rows in input and output arrays
 * N             - number of columns in input and output arrays
 * nhood         - neighborhood representing the structuring element
 * walker        - neighborhood walker traverses neighborhood in image
 *
 * Output ====== Out - pointer to first element of output array
 */


void erode_logical_twod(bool *In, bool *Out, int M, int N,
                        Neighborhood_T nhood, NeighborhoodWalker_T walker)
{

    // determine r,c endpoints of interior image using minumum and maximum
    // offsets in nhood
    ptrdiff_t min_r_offset = 0;
    ptrdiff_t min_c_offset = 0;
    ptrdiff_t max_r_offset = 0;
    ptrdiff_t max_c_offset = 0;
    int c_interior_start    = 0;
    int c_interior_end      = 0;
    int r_interior_start    = 0;
    int r_interior_end      = 0;

    int k,idx;
    int num_elements = M * N;

    for (k = 0; k < nhood->num_neighbors; k++)
    {
        idx = 2*k;
        if (nhood->array_coords[idx] < min_r_offset)
        {
            min_r_offset = nhood->array_coords[idx];
        }
        if (nhood->array_coords[idx] > max_r_offset)
        {
            max_r_offset = nhood->array_coords[idx];
        }
        if (nhood->array_coords[idx+1] < min_c_offset)
        {
            min_c_offset = nhood->array_coords[idx+1];
        }
        if (nhood->array_coords[idx+1] > max_c_offset)
        {
            max_c_offset = nhood->array_coords[idx+1];
        }
    }

    c_interior_start = my_min(my_abs(min_c_offset), N);
    c_interior_end = my_max((ptrdiff_t) N - max_c_offset, 0);
    r_interior_start = my_min(my_abs(min_r_offset), M);
    r_interior_end = my_max((ptrdiff_t) M - max_r_offset, 0);

    int r,c;
    int idxn,q;

    //process interior pixels
    for (c = c_interior_start; c < c_interior_end; c++)
    {
        idx = M * c + r_interior_start;

        for (r = r_interior_start; r < r_interior_end; r++)
        {

            for(k = 0; k < walker->num_neighbors; k++)
            {
                idxn = idx + (int) walker->neighbor_offsets[k];

                if (!In[idxn])
                {
                    // if the neighbor is 0, then break out of the for
                    // loop. Out[idx] = 0 by default.
                    break;
                }
            }
            if (k == walker->num_neighbors)
            {
                // no zero-valued neigbhors
                Out[idx] = 1;
            }
            idx++;
        }

    }

    //process left edge
    int end_idx = M * c_interior_start;
    bool zero_found;

    for (idx = 0; idx < end_idx ; idx++)
    {
        zero_found = false;
        nhSetWalkerLocation(walker, idx);
        while (nhGetNextInboundsNeighbor(walker, &q, NULL))
        {
            if (!In[q])
            {
                // if the neighbor is 0, then break out of the while
                // loop. Out[idx] = 0 by default.
                zero_found = true;
                break;
            }
        }
        if (!zero_found)
        {
            // no zero-valued neighbors
            Out[idx] = 1;
        }
    }

    //process right edge
    int starting_idx = M * c_interior_end;

    for (idx = starting_idx; idx < num_elements; idx++)
    {
        zero_found = false;
        nhSetWalkerLocation(walker, idx);
        while (nhGetNextInboundsNeighbor(walker, &q, NULL))
        {
            if (!In[q])
            {
                // if the neighbor is 0, then break out of the while
                // loop. Out[idx] = 0 by default.
                zero_found = true;
                break;
            }
        }
        if (!zero_found)
        {
            //no zero-valued neighbors.
            Out[idx] = 1;
        }
    }


    // process top and bottom edges
    int idx_top, idx_bottom;

    for (c = c_interior_start; c < c_interior_end; c++)
    {

        // top edge

        idx_top = M * c;

        for (r = 0; r < r_interior_start; r++)
        {
            zero_found = false;
            nhSetWalkerLocation(walker, idx_top);
            while (nhGetNextInboundsNeighbor(walker, &q, NULL))
            {
                if (!In[q])
                {
                    // if the neighbor is 0, then break out of the while
                    // loop. Out[idx] = 0 by default.
                    zero_found = true;
                    break;
                }
            }
            if (!zero_found)
            {
                //no zero-valued neighbors.
                Out[idx_top] = 1;
            }
            idx_top++;
        }

        // bottom edge

        idx_bottom = M * c + r_interior_end;

        for (r = r_interior_end; r < M; r++)
        {
            zero_found = false;
            nhSetWalkerLocation(walker, idx_bottom);
            while (nhGetNextInboundsNeighbor(walker, &q, NULL))
            {
                if (!In[q])
                {

                    // if the neighbor is 0, then break out of the while
                    // loop. Out[idx_bottom] = 0 by default.
                    zero_found = true;
                    break;
                }
            }
            if (!zero_found)
            {
                //no zero-valued neighbors.
                Out[idx_bottom] = 1;
            }
            idx_bottom++;
        }
    }
}

/*
 * erodeones33_interior_pixels
 *
 * Erosion of interior pixels in a logical array using a ones(3)
 * neighborhood. No boundary checking.  We break out the common and popular
 * ones(3) case to get the best performance.
 *
 * Inputs
 * ======
 * In            - pointer to first element of input array
 * num_elements  - number of elements in input and output arrays
 * M             - number of rows in input array
 * N             - number of columns in input array
 *
 * Output ====== Out - pointer to first element of output array
 */
void erodeones33_interior_pixels(bool *In, bool *Out,
                                 ptrdiff_t M, ptrdiff_t N)
{
    ptrdiff_t r, c;
    ptrdiff_t idx;
    int k = 0;
    ptrdiff_t neighbor_idx;

    // removed 0 from the offsets list (this is the center pixel) because we
    // look it it when we query In[idx], which is the center pixel
    ptrdiff_t offsets[8] = {-M - 1,
                                -M,
                                -M + 1,
                                -1,
                                1,
                                M - 1,
                                M,
                                M + 1};

    for (c = 1; c < N - 1; c++)
    {
        idx = M * c;

        for (r = 1; r < M - 1; r++)
        {
            idx++;

            if(In[idx])
            {
                for (k = 0; k < 8; k++)
                {
                    neighbor_idx = idx + offsets[k];
                    if (!In[neighbor_idx])
                    {
                        break;
                    }
                }
                if (k == 8)
                {
                    Out[idx] = 1;
                }
            }
        }
    }
}

/*
 * erodeones33_edge_pixels
 *
 * Erosion of boundary pixels in the binary image using a ones(3)
 * neighborhood.  We break out the common and popular ones(3) case to get
 * the best performance.
 *
 * Inputs
 * ======
 * In            - pointer to first element of input array
 * num_elements  - number of elements in input and output arrays
 * walker        - neighborhood walker representing the structuring element
 * M             - number of rows in image
 * N             - number of columns in image
 *
 * Output ====== Out - pointer to first element of output array
 */
void erodeones33_edge_pixels(bool *In, bool *Out,
                             NeighborhoodWalker_T walker,
                             ptrdiff_t M, ptrdiff_t N,
                             int num_elements)
{
    ptrdiff_t p;
    int q;

    //Do first column
    for (p = 0; p < M; p++)
    {

        if (In[p])
        {
            bool zero_found = false;
            nhSetWalkerLocation(walker, p);
            while (nhGetNextInboundsNeighbor(walker, &q, NULL))
            {
                if (!In[q])
                {
                    zero_found = true;
                    break;
                }
            }
            if (!zero_found)
            {
                //didn't find any zero-valued neighbors
                Out[p] = 1;
            }
        }
    }


    //Do last column
    for (p = (ptrdiff_t) num_elements - M;p < (ptrdiff_t) num_elements; p++)
    {
        if (In[p])
        {
            bool zero_found = false;
            nhSetWalkerLocation(walker, p);
            while (nhGetNextInboundsNeighbor(walker, &q, NULL))
            {
                if (!In[q])
                {
                    zero_found = true;
                    break;
                }
            }
            if (!zero_found)
            {
                //didn't find any zero-valued neighbors
                Out[p] = 1;
            }
        }
    }

    //Do first and last row
    ptrdiff_t idx_top,idx_bottom;
    ptrdiff_t col;
    ptrdiff_t endingCol = N - 2;

    for (col = 0; col < endingCol; col++)
    {
        idx_top = M + col * M;
        idx_bottom = idx_top + M - 1;

        if (In[idx_top])
        {
            bool zero_found = false;
            nhSetWalkerLocation(walker, idx_top);
            while (nhGetNextInboundsNeighbor(walker, &q, NULL))
            {
                if (!In[q])
                {
                    zero_found = true;
                    break;
                }
            }
            if (!zero_found)
            {
                //didn't find any zero-valued neighbors
                Out[idx_top] = 1;
            }
        }

        if (In[idx_bottom])
        {
            bool zero_found = false;
            nhSetWalkerLocation(walker, idx_bottom);
            while (nhGetNextInboundsNeighbor(walker, &q, NULL))
            {
                if (!In[q])
                {
                    zero_found = true;
                    break;
                }
            }
            if (!zero_found)
            {
                //didn't find any zero-valued neighbors
                Out[idx_bottom] = 1;
            }
        }
    }
}

/*
 * dilateones33_edge_pixels
 *
 * Perform dilation of boundary pixels in the binary image using a ones(3)
 * neighborhood.  We break out the common and popular ones(3) case to get
 * the best performance.
 *
 * Inputs
 * ======
 * In            - pointer to first element of input array
 * num_elements  - number of elements in input and output arrays
 * walker        - neighborhood walker representing the structuring element
 * M             - number of rows in image
 * N             - number of columns in image
 *
 * Output ====== Out - pointer to first element of output array
 */
void dilateones33_edge_pixels(bool *In, bool *Out,
                              NeighborhoodWalker_T walker,
                              ptrdiff_t M, ptrdiff_t N,
                              int num_elements)
{
    ptrdiff_t p;
    int q;

    //Do first column
    for (p = 0; p < M; p++)
    {
        if (In[p])
        {
            nhSetWalkerLocation(walker, p);
            while (nhGetNextInboundsNeighbor(walker, &q, NULL))
            {
                Out[q] = 1;
            }
        }
    }


    //Do last column
    for (p = (ptrdiff_t) num_elements - M;
         p < (ptrdiff_t) num_elements; p++)
    {
        if (In[p])
        {
            int q;

            nhSetWalkerLocation(walker, p);
            while (nhGetNextInboundsNeighbor(walker, &q, NULL))
            {
                Out[q] = 1;
            }
        }
    }

    //Do first and last row
    ptrdiff_t idx_top, idx_bottom;
    ptrdiff_t col;
    ptrdiff_t endingCol = N -2;

    for (col = 0; col < endingCol;col++)
    {
        idx_top = M + col * M;
        idx_bottom = idx_top + M - 1;

        if (In[idx_top])
        {
            int q;

            nhSetWalkerLocation(walker, idx_top);
            while (nhGetNextInboundsNeighbor(walker, &q, NULL))
            {
                Out[q] = 1;
            }
        }

        if (In[idx_bottom])
        {
            int q;

            nhSetWalkerLocation(walker, idx_bottom);
            while (nhGetNextInboundsNeighbor(walker, &q, NULL))
            {
                Out[q] = 1;
            }
        }
    }
}

/*
 * dilateones33_interior_pixels
 *
 * Dilation of interior pixels in a logical array using a ones(3)
 * neighborhood. No boundary checking. We break out the common and
 * popular ones(3) case to get the best performance.
 *
 * Inputs
 * ======
 * In            - pointer to first element of input array
 * num_elements  - number of elements in input and output arrays
 * M             - number of rows in input array
 * N             - number of columns in input array
 *
 * Output ====== Out - pointer to first element of output array
 */

void dilateones33_interior_pixels(bool *In, bool *Out,
                                  ptrdiff_t M, ptrdiff_t N)
{
    ptrdiff_t r, c;
    ptrdiff_t idx;
    ptrdiff_t idx1, idx2, idx3, idx4, idx5, idx6, idx7, idx8;

    // don't include offset 0 (center pixel) because we query In[idx], which is
    // the center pixel
    ptrdiff_t offset1 = -M - 1;
    ptrdiff_t offset2 = -M;
    ptrdiff_t offset3 = -M + 1;
    ptrdiff_t offset4 = -1;
    ptrdiff_t offset5 = 1;
    ptrdiff_t offset6 = M - 1;
    ptrdiff_t offset7 = M;
    ptrdiff_t offset8 = M + 1;

    for (c = 1; c < N - 1; c++)
    {
        idx = M * c;

        for (r = 1; r < M - 1; r++)
        {
            idx++;

            if(In[idx])
            {
                idx1 = idx + offset1;

                idx2 = idx + offset2;
                idx3 = idx + offset3;
                idx4 = idx + offset4;
                idx5 = idx + offset5;
                idx6 = idx + offset6;
                idx7 = idx + offset7;
                idx8 = idx + offset8;

                Out[idx] = 1;
                Out[idx1] = 1;
                Out[idx2] = 1;
                Out[idx3] = 1;
                Out[idx4] = 1;
                Out[idx5] = 1;
                Out[idx6] = 1;
                Out[idx7] = 1;
                Out[idx8] = 1;
            }
        }
    }
}
