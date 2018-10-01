/*
 * Grayscale flat erosion using a linear structuring element oriented along
 * an array dimension.  This file contains a template used by morphmex.cpp.
 *
 * Algorithm reference: M. van Herk, "A fast algorithm for local minimum
 * and maximum filters on rectangular and octagonal kernels," Pattern Recognition
 * Letters, 13:517-521, 1992.  This implementation follows the description
 * in Pierre Soille, Morphological Image Analysis: Principles and Applications,
 * 2nd ed., 89-90.
 *
 * $Revision: 1.1.8.3 $
 */

#ifndef TOPHAT_RECODE_DILATE_LINEAR_H
#define TOPHAT_RECODE_DILATE_LINEAR_H

#include <cstddef>

#ifndef MIN
#define MIN(a, b) ( (a) < (b) ? (a) : (b))
#endif

/*
 * erodeWithLine performs gray scale flat erosion of a vector by a line segment.
 *
 * Input parameter f is the input vector.  It's allocated length (working_length)
 *     must be a multiple of lambda, the length of the line segment.  If
 *     working_length > length (the original unpadded vector length), then
 *     erodeWithLine() automatically fills in the end of f with pad values as
 *     necessary.  The calling routine only has to put length values into f.
 *
 * Input parameter g is temporary working space for the algorithm.  It's allocated
 *     length must be lambda.
 *
 * Input parameter h is temporary working space for the algorithm.  It's allocated
 *     length must be lambda.
 *
 * Output parameter r is the eroded vector.  It's allocated length must be lambda.
 *
 * Input parameter pad_value is the value used to fill in f.  It is also used
 *     for values that are "past the ends" of the vectors.  For erosion it should
 *     be a value that is guaranteed to be no smaller than any value in f.
 *
 * Input parameter lambda is the length (number of pixels) of the linear structuring
 *     element.
 *
 * Input parameter origin is the origin offset, relative to the left-most pixel
 *     of the linear structuring element.
 *
 * Input parameter length is the unpadded length of f.
 *
 * Input parameter working_length is the padded length of f.  It must be a multiple
 *     of lambda.
 *
 * Notes
 * -----
 * Variable names in this routine have been chosen to match the algorithm description
 * in the Soille book (see reference above).
 *
 */
template <typename T>
void erode_with_line(T *f, T *g, T *h, T *r, T pad_value, int lambda, ptrdiff_t origin,
        int length, int working_length) {
        /**
         * insert pad value into the end of the input vector
         */
         for (int x = length; x < working_length; ++x) {
             f[x] = pad_value;
         }

         for (int x = 0; x < working_length; ++x) {
             g[x] = ((x % lambda) == 0) ? f[x] : MIN(g[x - 1], f[x]);
         }

         for (ptrdiff_t x = working_length - 1; x >= 0; --x) {
             h[x] = (((x+1) % lambda) == 0) ? f[x] : MIN(h[x+1], f[x]);
         }

         ptrdiff_t g_offset = static_cast<ptrdiff_t>(lambda) - origin - 1;
         ptrdiff_t h_offset = -origin;
         for (int x = 0; x < working_length; ++x) {
             T v1;
             T v2;
             ptrdiff_t xg = static_cast<ptrdiff_t>(x) + g_offset;

             v1 = ((xg >= 0) && (xg < static_cast<ptrdiff_t >(working_length))) ?
                  g[xg] : pad_value;

             ptrdiff_t xh = static_cast<ptrdiff_t >(x) + h_offset;
             v2 = ((xh >= 0) && (xh < static_cast<ptrdiff_t >(working_length))) ?
                  h[xh] : pad_value;

             r[x] = MIN(v1, v2);
         }
}

#endif //TOPHAT_RECODE_DILATE_LINEAR_H
