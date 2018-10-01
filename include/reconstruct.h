//
// Created by xinyuangui on 9/9/18.
//

#ifndef TOPHAT_RECODE_RECONSTRUCT_H
#define TOPHAT_RECODE_RECONSTRUCT_H

#include "neighborhood.h"
#include <stdexcept>
#include <queue>


//////////////////////////////////////////////////////////////////////////////
//
// This file contains the function body for the image reconstruction
// algorithm.
//
// Algorithm reference: Luc Vincent, "Morphological Grayscale Reconstruction
// in Image Analysis: Applications and Efficient Algorithms," IEEE
// Transactions on Image Processing, vol. 2, no. 2, April 1993, pp. 176-201.
//
// The algorithm used here is "fast hybrid grayscale reconstruction,"
// described as follows on pp. 198-199:
//
// I: mask image (binary or grayscale)
// J: marker image, defined on domain D_I, J <= I.
//   Reconstruction is determined directly in J.
//
// Scan D_I in raster order:
// Let p be the current pixel;
// J(p) <- (max{J(q),q member_of N_G_plus(p) union {p}}) ^ I(p)
//      [Note that ^ here refers to "pointwise minimum.]
//
// Scan D_I in antiraster order:
//  Let p be the current pixel;
//  J(p) <- (max{J(q),q member_of N_G_minus(p) union {p}}) ^ I(p)
//      [Note that ^ here refers to "pointwise minimum.]
//  If there exists q member_of N_G_minus(p) such that J(q) < J(p) and
//      J(q) < I(q), then fifo_add(p)
//
// Propagation step:
//  While fifo_empty() is false
//  p <- fifo_first()
//  For every pixel q member_of N_G(p):
//    If J(q) < J(p) and I(q) ~= J(q), then
//      J(q) <- min{J(p),I(q)}
//      fifo_add(q)
//
//////////////////////////////////////////////////////////////////////////////
template <typename _T>
void compute_reconstruction(_T *J, _T *I, int num_elements,
        NeighborhoodWalker_T walker,
        NeighborhoodWalker_T trailingWalker,
        NeighborhoodWalker_T leadingWalker) {

    // enforce the requirement that J <= I. We need to check this here.
    // because if it isn't true, the algorithm might not terminate
    for (int k = 0; k < num_elements; ++k) {
        if (J[k] > I[k]) {
            throw std::invalid_argument("Images:imreconstruct:markerGreaterThanMas: "
                                        "MARKER pixels must be <= MASK pixels.");
        }
    }

    std::queue<int> Queue;

    // first pass, scan D_I in raster order (upper-left to lower-right,
    // along the columns)
    for (int p = 0; p < num_elements; ++p) {
        // "Let p be the current pixel"
        // "J(p) <- (max{J(q),q member_of N_G_plus(p) union {p}}) ^ I(p)"

        // Find the maximum value of the (y,x) pixel
        // plus all the pixels in the "plus" neighborhood
        // of (y,x).

        _T max_pixel = J[p];
        nhSetWalkerLocation(trailingWalker, p);
        int q;
        while (nhGetNextInboundsNeighbor(trailingWalker, &q, NULL)) {
            if (J[q] > max_pixel) {
                max_pixel = J[q];
            }
        }
        // Now set the (y, x) pixel of image J to the minimum
        // of max_pixel and the (y, x) pixel of image I
        J[p] = (max_pixel < I[p]) ? max_pixel : I[p];
    }

    // second pass, scan D_I in antiraster order (lower-right to upper-left,
    // along the columns
    for (int pp = 0; pp < num_elements; ++pp) {
        int p = num_elements - 1 - pp;

        // "Let p be the current pixel"
        // "J(p) <- (max{J(q),q member_of N_G_minus(p) union {p}}) ^ I(p)"

        // Find the maximum value of the (y,x) pixel
        // plus all the pixels in the "minus" neighborhood
        // of (y,x).
        _T max_pixel = J[p];
        nhSetWalkerLocation(leadingWalker, p);
        int q;
        while (nhGetNextInboundsNeighbor(leadingWalker, &q, NULL)) {
            if (J[q] > max_pixel) {
                max_pixel = J[q];
            }
        }

        // Now set the (y,x) pixel of image J to the minimum
        // of maxPixel and the (y,x) pixel of image I.

        J[p] = (max_pixel < I[p]) ? max_pixel : I[p];

        // If there exists q member_of N_G_minus(p)
        // such that J(q) < J(p) and J(q) < I(q), then fifo_add(p)
        nhSetWalkerLocation(leadingWalker, p);
        while (nhGetNextInboundsNeighbor(leadingWalker, &q, NULL)) {
            if (J[q] < J[p] && J[q] < I[q]) {
                Queue.push(p);
                break;
            }
        }
    }

    // Propagation step
    while (!Queue.empty()) {
        int p = Queue.front();
        Queue.pop();
        _T Jp = J[p];

        // for every pixel q member_of_N_g(p);
        nhSetWalkerLocation(walker, p);
        int q;
        while (nhGetNextInboundsNeighbor(walker, &q, NULL)) {

            // "If J(q) < J(p) and I(q) ~= J(q), then
            //  J(q) <- min{J(p),I(q)}
            //  fifo_add(q)"
            _T Jq = J[q];
            _T Iq = I[q];
            if (Jq < Jp && Iq != Jq) {
                J[q] = (Jp < Iq) ? Jp : Iq;
                Queue.push(q);
            }
        }
    }
}

#endif //TOPHAT_RECODE_RECONSTRUCT_H





























