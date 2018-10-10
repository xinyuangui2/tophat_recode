//
// Created by xinyuangui on 9/8/18.
//

#include <stdexcept>
#include <cassert>
#include "neighborhood.h"


/**
 * nhMakeNeighborhood
 * Make neighborhood object.
 *
 * Input
 * =====
 * D - Connectivity array.  In its most general form, D can be
 *     an N-D array of 0s and 1s.  The 1s indicate members of the
 *     neighborhood, relative to the center of D (rounded to
 *     the upper left).  D can also be one of these scalars:
 *     4  - 2-D neighborhood, neighbors connected to center by an edge
 *     8  - 2-D neighborhood, neighbors connected to center by an edge
 *          or a vertex
 *     6  - 3-D neighborhood, neighbors connected to center by a face
 *     18 - 3-D neighborhood, neighbors connected to center by a face
 *          or an edge
 *     26 - 3-D neighborhood, neighbors connected to center by a face,
 *          edge, or vertex
 *
 *     center_location - The location of the center of the neighborhood
 *                   Possible values:
 *                   NH_CENTER_MIDDLE_ROUNDDOWN - center in the middle
 *                                                round even dimensions down
 *                   NH_CENTER_MIDDLE_ROUNDUP   - center in the middle
 *                                                round even dimensions up
 *                   NH_CENTER_UL               - center in the upper left
 *                   NH_CENTER_LR               - center in the lower right

 * Return
 * ======
 * new Neighborhood_T object.
 */
Neighborhood_T nhMakeNeighborhood(int D, int center_location) {
    // D is one scalar of 4/8
    if (D != 4 && D != 8) {
        throw std::invalid_argument("connective number can only be 4 or 8");
    }
    int max = D == 4 ? 1 : 2;
    int num_neighbors = D + 1;// center pixel
    Neighborhood_T result = allocate_neighborhood(num_neighbors);
    int index = 0;
    for (int col = -1; col <= 1; ++col) {
        for (int row = -1; row <= 1; ++row) {
            /**
             * sum == 1, edge-connected
             * sum == 2, edge or vertex-connnected
             */
            int sum = (row != 0) + (col != 0);
            if (sum <= max) {
                result->array_coords[index++] = row;
                result->array_coords[index++] = col;
            }
        }
    }
    return result;
}

/**
 * allocate_neighborhood_walker
 * Allocate space for NeighborhoodWalker_T object.
 * static function: only visible to this file
 *
 * Inputs
 * ======
 * num_neighbors - number of neighbors in the neighborhood
 * num_dims      - number of dimensions
 *
 * Return
 * ======
 * NeighborhoodWalker_T object.
 */
static NeighborhoodWalker_T allocate_neighborhood_walker(int num_neighbors, int num_dims) {
    NeighborhoodWalker_T walker = (NeighborhoodWalker_T) malloc(sizeof(*walker));
    walker->array_coords = (ptrdiff_t *)malloc(num_neighbors * num_dims * sizeof(ptrdiff_t));
    walker->neighbor_offsets = (ptrdiff_t *) malloc(num_neighbors *
                                                          sizeof(ptrdiff_t));
    walker->image_size = (int *) malloc(num_dims * sizeof(int));
    walker->cumprod = (int *) malloc((num_dims+1)* sizeof(int));
    walker->center_coords = (int *) malloc(num_dims * sizeof(int));
    walker->use = (bool *) malloc(num_neighbors * sizeof(bool));

    /**
     * walker can't be used until nhSetWalkerLocation is called.
     */
    walker->ready_for_use = false;

    return walker;
}


/**
 * sub_to_relative_ind
 * Compute linear relative offset when given relative array
 * offsets.
 *
 * Inputs
 * ======
 * coords   - array of relative array offsets
 * num_dims - number of dimensions
 *
 * Return
 * ======
 * relative linear offset
 */
static ptrdiff_t sub_to_relative_ind(ptrdiff_t *coords) {

    /**
     * Algorithm notes:
     * We want to compute sub_to_ind and see if ind > 0.  However,
     * the output of sub_to_ind is only valid if the specified neighbor
     * is guaranteed to be within the image, as defined by the size
     * used in the sub_to_ind computation.  Hence, we imagine we are
     * computing sub_to_ind on a neighbor of the center pixel of a
     * P-by-P-by- ... -by-P array, where P = 2*N+1, and N is the maximum
     * absolute neighbor offset.  This guarantees the validity of the
     * sub_to_ind computation.
     */

    ptrdiff_t N = 0;
    for (int k = 0; k < NUM_DIMS; ++k) {
        ptrdiff_t abs_coord = coords[k] > 0 ? coords[k] : -coords[k];
        if (abs_coord > N) {
            N = abs_coord;
        }
    }

    // perform sub_to_ind computation
    ptrdiff_t index = 0;
    ptrdiff_t cumprod = 1;
    ptrdiff_t P = 2 * N + 1;
    for (int k = 0; k < NUM_DIMS; ++k) {
        index += coords[k] * cumprod;
        cumprod *= P;
    }

    return index;
}

/**
 * is_leading_neighbor
 * Is a given neighbor reached before the center pixel in a linear
 * scan of image pixels?
 *
 * Inputs
 * ======
 * coords   - array of neighborhood offset coordinates
 * num_dims - number of dimensions
 *
 * Return
 * ======
 * true/false
 */
static bool is_leading_neighbor(ptrdiff_t *coords) {
    return (sub_to_relative_ind(coords) > 0);
}

/**
 * is_trailing_neighbor
 * Is a given neighbor reached after the center pixel in a linear
 * scan of image pixels?
 *
 * Inputs
 * ======
 * coords   - array of neighborhood offset coordinates
 * num_dims - number of dimensions
 *
 * Return
 * ======
 * true/false
 */
static bool is_trailing_neighbor(ptrdiff_t *coords) {
    return (sub_to_relative_ind(coords) < 0);
}

/**
 * is_neighborhood_center
 * Is a given neighbor the neighborhood center?
 *
 * Inputs
 * ======
 * coords   - array of neighborhood offset coordinates
 * num_dims - number of dimensions
 *
 * Return
 * ======
 * true if specified neighbor is the neighborhood center; false otherwise
 */
static bool is_neighborhood_center(ptrdiff_t *coords)
{
    bool result = true;

    if(coords == NULL) {
        throw std::invalid_argument("coords cannot be NULL");
    }

    for (int k = 0; k < NUM_DIMS; k++)
    {
        if (coords[k] != 0)
        {
            result = false;
            break;
        }
    }

    return result;
}
/**
 * process_walker_flags
 * Mark certain neighbors to be skipped according to the settings
 * of the options flags.
 *
 * Inputs
 * ======
 * walker   - NeighborhoodWalker_T object (modified)
 * flags    - Binary OR of any combination of
 *            NH_SKIP_LEADING, NH_SKIP_TRAILING, NH_SKIP_CENTER, or
 *            NH_USE_ALL
 */
static void process_walker_flags(NeighborhoodWalker_T walker, unsigned int flags) {
    if (walker == NULL) {
        throw std::invalid_argument("walker cannot be NULL");
    }
    bool skip_center = (flags & NH_SKIP_CENTER) != 0;
    bool skip_leading = (flags & NH_SKIP_LEADING) != 0;
    bool skip_tailing = (flags & NH_SKIP_TRAILING) != 0;

    for (int k = 0; k < walker->num_neighbors; ++k) {
        int num_dims = NUM_DIMS;
        ptrdiff_t *pr = walker->array_coords + k * NUM_DIMS;
        if ((skip_leading && is_leading_neighbor(pr)) ||
        (skip_tailing && is_trailing_neighbor(pr)) ||
        (skip_center && is_neighborhood_center(pr))) {
            walker->use[k] = false;
        }
    }
}

/**
 * nhMakeNeighborhoodWalker
 * Make neighborhood walker object.
 *
 * Inputs
 * ======
 * nhood       - Neighborhood_T object
 * input_size  - array of image dimensions
 * input_dims  - number of image dimensions
 * flags       - options flags; see below
 *
 * Return
 * ======
 * NeighborhoodWalker_T object
 *
 * Options values include:
 * NH_SKIP_CENTER     - walker will the center pixel itself
 * NH_SKIP_LEADING    - walker will skip neighbors that precede
 *                      the center pixel in a linear scan of pixels
 * NH_SKIP_TRAILING   - walker will skip neighbors that follow
 *                      the center pixel in a linear scan of pixels
 * NH_USE_ALL         - walk will go to every neighbor in a linear
                      - scan of pixels.
 * Options values can be OR'd together.  For example, if flags is
 * NH_SKIP_CENTER | NH_SKIP_LEADING, then both the center pixel
 * and leading neighbors will be skipped.
 */

NeighborhoodWalker_T nhMakeNeighborhoodWalker(Neighborhood_T nhood,
        const int *input_size,
        unsigned int flags) {
    NeighborhoodWalker_T walker;
    int num_neighbors = nhood->num_neighbors;

    /**
     * the dimensionality of the walker is max(image dimension, neighborhood dimension)
     */
     int num_dims = NUM_DIMS;
     walker = allocate_neighborhood_walker(num_neighbors, num_dims);
     walker->num_neighbors = num_neighbors;

     for (int k = 0; k < num_neighbors; ++k) {
         ptrdiff_t *pr_in = nhood->array_coords + k * NUM_DIMS;
         ptrdiff_t *pr_out = walker->array_coords + k * NUM_DIMS;
         for (int p = 0; p < NUM_DIMS; ++p) {
             pr_out[p] = pr_in[p];
         }
     }

     for (int k = 0; k < NUM_DIMS; ++k) {
         walker->image_size[k] = input_size[k];
     }

     walker->cumprod[0] = 1;
     for (int k = 1; k < NUM_DIMS; ++k) {
         walker->cumprod[k] = walker->cumprod[k - 1] * walker->image_size[k - 1];
     }

     for (int k = 0; k < num_neighbors; ++k) {
         ptrdiff_t *pr_in = walker->array_coords + k * NUM_DIMS;
         walker->neighbor_offsets[k] = sub_to_ind_signed(pr_in, walker->cumprod);
     }

     for (int k = 0; k < num_neighbors; ++k) {
         walker->use[k] = true;
     }

     process_walker_flags(walker, flags);
     return walker;
}


/**
 * nhMakeDefaultConnectivityNeighborhood
 * Make neighborhood corresponding to DOMAIN = ones(repmat(3,1,num_dims)).
 *
 * Inputs
 * ======
 * num_dims      - number of dimensions
 *
 * Return
 * ======
 * Neighborhood_T object.
 */
Neighborhood_T nhMakeDefaultConnectivityNeighborhood() {
    int num_neighbors = 1;
    int *size = (int *)malloc(NUM_DIMS * sizeof(int));
    int *cumprod = (int *)malloc(NUM_DIMS * sizeof(int));
    int *unsigned_coords = (int *)malloc(NUM_DIMS * sizeof(int));

    for (int k = 0; k < NUM_DIMS; ++k) {
        size[k] = 3;
        cumprod[k] = num_neighbors;
        num_neighbors *= 3;
    }

    Neighborhood_T result = allocate_neighborhood(num_neighbors);
    for (int k = 0; k < num_neighbors; ++k) {
        ptrdiff_t *coords = result->array_coords + k * NUM_DIMS;
        ind_to_sub(k, cumprod, unsigned_coords);
        for (int q = 0; q < NUM_DIMS; ++q) {
            coords[q] = static_cast<ptrdiff_t>(unsigned_coords[q]) - 1;
        }
    }

    free(size);
    free(cumprod);
    free(unsigned_coords);

    return result;
}


/**
 * nhReflectNeighborhood
 * Negate all neighborhood offset values
 *
 * Input
 * =====
 * nhood - neighborhood object (modified)
 */
void nhReflectNeighborhood(Neighborhood_T nhood) {
    ptrdiff_t *coords = nhood->array_coords;
    int num_values = nhood->num_neighbors * NUM_DIMS;
    for (int k = 0; k < num_values; ++k) {
        coords[k] = -coords[k];
    }
}

/**
 * free space associated with neighborhood object
 */
void nhDestroyNeighborhood(Neighborhood_T nhood) {
    free(nhood->array_coords);
    free(nhood);
}

/**
 * nhDestroyNeighborhoodWalker
 * Free space allocated by walker object.
 *
 * Input
 * =====
 * walker - NeighborhoodWalker_T object (modified)
 *
 */
void nhDestroyNeighborhoodWalker(NeighborhoodWalker_T walker)
{
    if (walker == NULL) {
        throw std::invalid_argument("walker cannot be NULL");
    }

    free(walker->array_coords);
    free(walker->neighbor_offsets);
    free(walker->image_size);
    free(walker->cumprod);
    free(walker->center_coords);
    free(walker->use);

    free(walker);
}


/**
 * nhSetWalkerLocation
 * Prepare to walk the neighborhood surrounding a specified pixel.
 *
 * Inputs
 * ======
 * walker   - neighborhood walker; this input is modified
 * p        - pixel location, specified as a linear offset from
 *            the beginning of the image array
 */
void nhSetWalkerLocation(NeighborhoodWalker_T walker, int p) {
    if (walker == NULL) {
        throw std::invalid_argument("walker cannot be NULL");
    }

    walker->pixel_offset = p;
    ind_to_sub(p, walker->cumprod, walker->center_coords);
    walker->next_neighbor_idx = 0;

    walker->ready_for_use = true;
}


/**
 * is_inbounds_neighbor
 * Given pixel coordinates, offset coordinates for a neighbor, and
 * image size, is the neighbor inside the image or not?
 *
 * Inputs
 * ======
 * offset_coords - array of neighbor offset coordinates
 * array_coords  - array of pixel coordinates
 * input_size    - array of image dimensions
 * num_dims      - length of the above arrays
 *
 * Return
 * ======
 * true if specified neighbor of specified pixel is within the bounds
 * of the image; false otherwise.
 */
static bool is_inbounds_neighbor(ptrdiff_t *offset_coords, int *array_coords,
        const int *input_size) {
    assert(offset_coords != NULL);
    assert(array_coords != NULL);
    assert(input_size != NULL);

    for (int k = 0; k < NUM_DIMS; ++k) {
        ptrdiff_t coordinate = static_cast<ptrdiff_t>(array_coords[k]) + offset_coords[k];
        if (coordinate < 0 || coordinate >= static_cast<ptrdiff_t>(input_size[k])) {
            return false;
        }
    }

    return true;
}

/** nhGetNextInboundsNeighbor
 *
 * Gets the next neighbor that isn't outside the image.  Caller must
 * call nhSetWalkerLocation first.
 *
 * Inputs:
 * - walker  NeighborhoodWalker_T; this input is modified
 *
 * Outputs:
 * - p       Neigbhbor location, expressed as a zero-based offset into
 *           the image array
 * - idx     Index of the neighbor, 0 to P-1, where P is the number of
 *           neighbors in the neighborhood.  idx can be NULL, in which
 *           case this value is not output.
 *
 * Return value:
 * true if successful; false if there were no more neighbors.  If false is
 * returned then p and idx are not set.
 */
bool nhGetNextInboundsNeighbor(NeighborhoodWalker_T walker, int *p, int *idx) {
    bool found = false;

    if (walker == NULL) {
        throw std::invalid_argument("walker cannot be NULL");
    }
    if (p == NULL) {
        throw std::invalid_argument("p cannot be NULL");
    }
    for (int k = walker->next_neighbor_idx; k < walker->num_neighbors; ++k) {
        if (walker->use[k] &&
        is_inbounds_neighbor(walker->array_coords + k * NUM_DIMS, walker->center_coords, walker->image_size)) {
            found = true;
            *p = walker->pixel_offset + walker->neighbor_offsets[k];
            if (idx != NULL) {
                *idx = k;
            }
            walker->next_neighbor_idx = k + 1;
            break;
        }
    }

    if (!found) {
        /**
         * make sure this walker cannot be used again util
         * nhSetWalkerLocation is called
         */
         walker->ready_for_use = false;
    }

    return found;
}


/**
 * nhGetWalkerNeighborOffsets
 * Get an array containing linear neighbor offsets, computed
 * with respect to a given image size.
 *
 * Input
 * =====
 * walker - NeighborhoodWalker_T object
 *
 * Return
 * ======
 * offsets - array of array coordinates
 */
ptrdiff_t *nhGetWalkerNeighborOffsets(NeighborhoodWalker_T walker)
{
    assert(walker != NULL);

    return(walker->neighbor_offsets);
}

/**
 * allocate space for new neighborhood object
 * @param num
 * @return
 */
Neighborhood_T allocate_neighborhood(int num_neighbors) {
    Neighborhood_T result;

    result = (Neighborhood_T) calloc(1, sizeof(*result));
    result->array_coords = (ptrdiff_t *)std::malloc(num_neighbors * NUM_DIMS * sizeof(*(result->array_coords)));
    result->num_neighbors = num_neighbors;
    return result;
}

/**
 * convert from array index to linear index
 * @param coords
 * @param cumprod
 * @return
 */
int sub_to_ind(int *coords, int *cumprod) {
    int index = 0;
    for (int i = 0; i < NUM_DIMS; ++i) {
        index += coords[i] * cumprod[i];
    }
    return index;
}


ptrdiff_t sub_to_ind_signed(ptrdiff_t *coords, int *cumprod) {
    ptrdiff_t index = 0;
    for (int i = 0; i < NUM_DIMS; ++i) {
        index += coords[i] * cumprod[i];
    }
    return index;
}

/**
 * convert from linear index to array coordinates
 * @param p
 * @param cumprod
 * @param coords
 */
void ind_to_sub(int p, int *cumprod, int *coords) {
    for (int j_up = 0; j_up < NUM_DIMS; ++j_up) {
        int j = NUM_DIMS - 1 - j_up;
        coords[j] = p / cumprod[j];
        p = p % cumprod[j];
    }
}

void ind_to_sub(int p, int *cumprod, ptrdiff_t *coords) {
    for (int j_up = 0; j_up < NUM_DIMS; ++j_up) {
        int j = NUM_DIMS - 1 - j_up;

        coords[j] = p / cumprod[j];
        p = p % cumprod[j];
    }
}


