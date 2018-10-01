//
// Created by xinyuangui on 9/24/18.
//
/**
 * Functions for nonflat grayscale dilation and erosion.  The actual dilation
 * and erosion algorithm code can be found in dilate_erode_gray_nonflat.h.
  * different numeric types by #defining TYPE, COMPARE_OP, INIT_VAL, DO_ROUND,
 * MIN_VAL, and MAX_VAL.
 *
 * Note that the dilation functions in this module all require reflected
 * neighborhoods.
 */

#include "morph.h"
#include <limits>

#define TYPE uint8_t
#define INIT_VAL 0
#define DO_ROUND
#define MIN_VAL 0
#define MAX_VAL UINT8_MAX
#define COMBINE_OP +
#define COMPARE_OP >
void dilate_gray_nonflat_uint8
#include "dilate_erode_gray_nonflat.h"

/*
 * dilate_gray_nonflat_uint16
 */
#define TYPE uint16_t
#define INIT_VAL 0
#define DO_ROUND
#define MIN_VAL 0
#define MAX_VAL UINT16_MAX
#define COMBINE_OP +
#define COMPARE_OP >
void dilate_gray_nonflat_uint16
#include "dilate_erode_gray_nonflat.h"

/*
 * dilate_gray_nonflat_uint32
 */
#define TYPE uint32_t
#define INIT_VAL 0
#define DO_ROUND
#define MIN_VAL 0
#define MAX_VAL UINT32_MAX
#define COMBINE_OP +
#define COMPARE_OP >
void dilate_gray_nonflat_uint32
#include "dilate_erode_gray_nonflat.h"

/*
 * dilate_gray_nonflat_int8
 */
#define TYPE int8_t
#define INIT_VAL INT8_MIN
#define DO_ROUND
#define MIN_VAL INT8_MIN
#define MAX_VAL INT8_MAX
#define COMBINE_OP +
#define COMPARE_OP >
void dilate_gray_nonflat_int8
#include "dilate_erode_gray_nonflat.h"

/*
 * dilate_gray_nonflat_int16
 */
#define TYPE int16_t
#define INIT_VAL INT16_MIN
#define DO_ROUND
#define MIN_VAL INT16_MIN
#define MAX_VAL INT16_MAX
#define COMBINE_OP +
#define COMPARE_OP >
void dilate_gray_nonflat_int16
#include "dilate_erode_gray_nonflat.h"

/*
 * dilate_gray_nonflat_int32
 */
#define TYPE int32_t
#define INIT_VAL INT32_MIN
#define DO_ROUND
#define MIN_VAL INT32_MIN
#define MAX_VAL INT32_MAX
#define COMBINE_OP +
#define COMPARE_OP >
void dilate_gray_nonflat_int32
#include "dilate_erode_gray_nonflat.h"

/*
 * dilate_gray_nonflat_single
 */
#define TYPE float
#define INIT_VAL ( std::numeric_limits<float>::min() )
#define COMBINE_OP +
#define COMPARE_OP >
void dilate_gray_nonflat_single
#include "dilate_erode_gray_nonflat.h"

/*
 * dilate_gray_nonflat_double
 */
#define TYPE double
#define INIT_VAL ( std::numeric_limits<double>::min() )
#define COMBINE_OP +
#define COMPARE_OP >
void dilate_gray_nonflat_double
#include "dilate_erode_gray_nonflat.h"

/*
 * erode_gray_nonflat_uint8
 * Perform nonflat grayscale erosion on a uint8 array.
 *
 * Inputs
 * ======
 * In             - pointer to first element of input array
 * num_elements   - number of elements in input and output arrays
 * walker         - neighborhood walker corresponding to structuring element
 * height         - pointer to array of heights; one height value
 *                  corresponding to each neighborhood element.
 *
 * Output
 * ======
 * Out            - pointer to first element of output array
 */
#define TYPE uint8_t
#define INIT_VAL UINT8_MAX
#define DO_ROUND
#define MIN_VAL 0
#define MAX_VAL UINT8_MAX
#define COMBINE_OP -
#define COMPARE_OP <
void erode_gray_nonflat_uint8
#include "dilate_erode_gray_nonflat.h"

/*
 * erode_gray_nonflat_uint16
 */
#define TYPE uint16_t
#define INIT_VAL UINT16_MAX
#define DO_ROUND
#define MIN_VAL 0
#define MAX_VAL UINT16_MAX
#define COMBINE_OP -
#define COMPARE_OP <
void erode_gray_nonflat_uint16
#include "dilate_erode_gray_nonflat.h"

/*
 * erode_gray_nonflat_uint32
 */
#define TYPE uint32_t
#define INIT_VAL UINT32_MAX
#define DO_ROUND
#define MIN_VAL 0
#define MAX_VAL UINT32_MAX
#define COMBINE_OP -
#define COMPARE_OP <
void erode_gray_nonflat_uint32
#include "dilate_erode_gray_nonflat.h"

/*
 * erode_gray_nonflat_int8
 */
#define TYPE int8_t
#define INIT_VAL INT8_MAX
#define DO_ROUND
#define MIN_VAL INT8_MIN
#define MAX_VAL INT8_MAX
#define COMBINE_OP -
#define COMPARE_OP <
void erode_gray_nonflat_int8
#include "dilate_erode_gray_nonflat.h"

/*
 * erode_gray_nonflat_int16
 */
#define TYPE int16_t
#define INIT_VAL INT16_MAX
#define DO_ROUND
#define MIN_VAL INT16_MIN
#define MAX_VAL INT16_MAX
#define COMBINE_OP -
#define COMPARE_OP <
void erode_gray_nonflat_int16
#include "dilate_erode_gray_nonflat.h"

/*
 * erode_gray_nonflat_int32
 */
#define TYPE int32_t
#define INIT_VAL INT32_MAX
#define DO_ROUND
#define MIN_VAL INT32_MIN
#define MAX_VAL INT32_MAX
#define COMBINE_OP -
#define COMPARE_OP <
void erode_gray_nonflat_int32
#include "dilate_erode_gray_nonflat.h"

/*
 * erode_gray_nonflat_single
 */
#define TYPE float
#define INIT_VAL ( std::numeric_limits<float>::max() )
#define COMBINE_OP -
#define COMPARE_OP <
void erode_gray_nonflat_single
#include "dilate_erode_gray_nonflat.h"

/*
 * erode_gray_nonflat_double
 */
#define TYPE double
#define INIT_VAL ( std::numeric_limits<double>::max() )
#define COMBINE_OP -
#define COMPARE_OP <
void erode_gray_nonflat_double
#include "dilate_erode_gray_nonflat.h"

