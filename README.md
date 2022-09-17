Pynq Z1 Matrix Multiplier created using VITIS HLS

***NOTICE*** this is a proof of concept... and this design needs optimization

Why is this important?:
 - Typically only small matrices can be processed on PL due to small BRAM sizes... so with this project you're limited by how much DRAM is available. With decent sizes    of DRAM (even 512 MB), you can do matrix multiplications on very large matrices.
 - This implementation is fast enough and has many places where it can be improved (speed currently: ~1 second to process [100,100] matrix * [100,100] matrix); this      was a proof of concept
 - Notice, this deign is very slow for 1024*1024 matrix multiplication

Useful implementation information:
 - numpy arrays can be easily inputted to this by using the ".flatten()" function
 - numpy arrays can be easily derived from the output by using: 
     mm_output_arr = np.array(output_arr)
     numpy.reshape(mm_output_arr, (#rows, #columns))

Info:
 - Current max size is 1024 x 1024 for matrix A and matrix B (this can be increased; view "Notes" below)
 - Matrix A should be inputted normally
 - Matrix B must be transposed (the program naturally thinks Matrix B is transposed and takes it's value "row by row"
 - mat1r and mat1cmat2r: should be set to the row size and column size of matrix A (first matrix)
 - mat1cmat2r and mat2c: should be set to the row size and column size of matrix B (second matrix) [NOT TRANSPOSED VALUES; original values]
 
Notes:
* - If you would like to increase the max size of the matrices then edit the following in "/Vitis/example.cpp":
    - Right now, "depth" is set for 1024x1024 = 1048576
    - Could change it to anything like 256x256 = 65536
    - IMPORTANT: Increasing the size of "depth" doesn't increase the BRAM requirement in PL. Increasing the max depth allows the m_axi to access a portion of DDR memory     equivalent to "depth".

    #pragma HLS INTERFACE m_axi port=matA depth=1048576 offset=slave bundle=matA
    #pragma HLS INTERFACE s_axilite port=matA bundle=CTRL

    #pragma HLS INTERFACE m_axi port=matB depth=1048576 offset=slave bundle=matB
    #pragma HLS INTERFACE s_axilite port=matB bundle=CTRL

    #pragma HLS INTERFACE m_axi port=matout depth=1048576 offset=slave bundle=matout
    #pragma HLS INTERFACE s_axilite port=matout bundle=CTRL
  
* - once you change the "depth" you need to adjust the size of the following in "/Vitis/example.cpp": 
    - IMPORTANT:
    - buf1 should be adjusted to the max size of a row for matrix A (first matrix)
    - buf1 should be adjusted to the max size of a column for matrix B (second matrix)
    - buffout should be adjusted to the max size of a row for matrix out (output matrix)
    - Note: modifying these arrays does change the BRAM requirement in PL.
  float buf1[32768] = {0};
	 float buf2[32768] = {0};
	 float buffout[32768] = {0};
  
Multiplication:
- The CPP is simple:
 1. Loop
   - 1A. burst read a row of matrix A
   2. Loop
     - 2A. burst read a row of transposed matrix B
     - 2B. Loop
       - 2B1. multiply values from row of matrix A with row of matrix B
       - 2B2. store the values individually in an output buffer array
     - 2C. finished multiplying, accumulate all values in an unrolled loop into a buffer
     - 2D. burst write out buffer to DDR memory

- Note: after 2D you loop back to "2.Loop" until you've exhausted all transposed rows (aka columns) of matrix B. After all transposed rows of matrix B are processed for the row of matrix A, then go back to "1.Loop" and burst read the next row of matrix A... and do it all over again.
