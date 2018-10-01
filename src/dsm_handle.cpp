//
// Created by xinyuangui on 9/30/18.
//

#include "dsm_handle.h"
#include <iostream>

/**
 * read dsm data, store it to the data and x_size, y_size
 * @param file_name
 */
dsm_handle::dsm_handle(const char *file_name) {
    GDALAllRegister();
    po_dataset = (GDALDataset *) GDALOpen(file_name, GA_ReadOnly);

    if (po_dataset == NULL) {
        std::cout << "cannot read dsm" << std::endl;
        return;
    }

    GDALRasterBand *po_band = po_dataset->GetRasterBand(1);

    float *paf_scan_line;
    x_size = po_band->GetXSize();
    y_size = po_band->GetYSize();

    data = (float *)malloc(sizeof(float) * x_size * y_size);

    paf_scan_line = (float *)CPLMalloc(sizeof(float) * x_size);
    int index = 0;
    for (int i = 0; i < y_size; ++i) {
        po_band->RasterIO( GF_Read, 0, 0, x_size, 1,
                          paf_scan_line, x_size, 1, GDT_Float32,
                          0, 0 );
        for (int j = 0; j < x_size; ++j) {
            data[index++] = paf_scan_line[j];
        }
    }
    CPLFree(paf_scan_line);
    GDALClose(po_dataset);
}

float* dsm_handle::get_dsm_data() {
    return data;
}

int dsm_handle::get_x_size() {
    return x_size;
}

int dsm_handle::get_y_size() {
    return y_size;
}

dsm_handle::~dsm_handle() {
    if (data != NULL) {
        free(data);
    }
}