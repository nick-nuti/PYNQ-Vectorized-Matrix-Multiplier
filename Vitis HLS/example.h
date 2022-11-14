#include "ap_int.h"
#include <cstdint>
#include "hls_burst_maxi.h"

void transfer_kernel(hls::burst_maxi<uint64_t> matA, hls::burst_maxi<uint64_t> matB,hls::burst_maxi<uint64_t> matout, const int mat1r, const int mat1cmat2r, const int mat2c);
