//
// Created by xinyuangui on 9/30/18.
//

#ifndef TOPHAT_RECODE_REORGANIZE_DSM_HANDLE_H
#define TOPHAT_RECODE_REORGANIZE_DSM_HANDLE_H

#include "gdal_priv.h"
#include "cpl_conv.h"

class dsm_handle {
public:
    dsm_handle(const char* file_name);
    float* get_dsm_data();
    int get_y_size();
    int get_x_size();
    ~dsm_handle();
private:
    GDALDataset *po_dataset;
    float *data;
    int x_size;
    int y_size;
};


#endif //TOPHAT_RECODE_REORGANIZE_DSM_HANDLE_H
