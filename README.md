# tophat_recode

## Code Structure

* The main API is in `./include/top_hat_extract.h`. You can directly call `top_hat_extract` function to extract the top-hat feature with `float` image and `int` mask
* The `test.c` has example of testing. It uses gdal to read dsm image.
* You can only use `/include` and `/src` folder in your project. It doesn't depend on any libraries.