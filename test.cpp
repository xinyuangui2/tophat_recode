/**
 * This file is used to test the algorithms
 */
#include <iostream>
#include <fstream>
#include "dsm_handle.h"
#include "top_hat_extract.h"

void print_data(float *data, int y_input, int x_input) {
    int index = 0;
    for (int i = 0; i < y_input; ++i) {
        for (int j = 0; j < x_input; ++j) {
            std::cout << data[index++] << ", ";
        }
        std::cout << std::endl;
    }
}

void write_data_to_txt(float* data, int y_input, int x_input, std::string file_name) {
    std::ofstream myfile;
    myfile.open (file_name);
    int index = 0;
    for (int i = 0; i < y_input; ++i) {
        for (int j = 0; j < x_input; ++j) {
            if (j != 0) myfile << " ";
            myfile << data[i * x_input + j];
        }
        if (i != y_input - 1) {
            myfile << std::endl;
        }
    }
    myfile.close();
}

int main() {
    dsm_handle handler("dsm.tif");
    float *data = handler.get_dsm_data();


//    handler.write_data_to_file("new_data.tif", data, handler.get_y_size(), handler.get_x_size());

    int num_elements = handler.get_x_size() * handler.get_y_size();
    int y_inputs = handler.get_y_size();
    int x_inputs = handler.get_x_size();

    int mask[5 * 5];
    for (int i = 0; i < 25; ++i) mask[i] = 1;

    float *tophat = top_hat_extract(data, y_inputs, x_inputs, mask, 5, 5);



    write_data_to_txt(tophat, y_inputs, x_inputs, "reconstruct.txt");
//    handler.write_data_to_file("new_data.tif", data, y_inputs, x_inputs);
//    handler.write_data_to_file("erode1.tif", reconstruct_result, y_inputs, x_inputs);
//

    free(tophat);
}
