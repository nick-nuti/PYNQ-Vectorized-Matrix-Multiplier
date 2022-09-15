#include "ap_int.h"
#include "hls_burst_maxi.h"

void transfer_kernel(hls::burst_maxi<float> matA, hls::burst_maxi<float> matB,hls::burst_maxi<float> matout, const int mat1r, const int mat1cmat2r, const int mat2c);
