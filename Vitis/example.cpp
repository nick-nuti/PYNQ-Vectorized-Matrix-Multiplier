#include "example.h"

//important example: https://discuss.pynq.io/t/using-m-axi-with-pynq-z1/2625

void transfer_kernel(hls::burst_maxi<float> matA, hls::burst_maxi<float> matB,hls::burst_maxi<float> matout, const int mat1r, const int mat1cmat2r, const int mat2c)
{
  #pragma HLS INTERFACE m_axi port=matA depth=1048576 offset=slave bundle=matA
  #pragma HLS INTERFACE s_axilite port=matA bundle=CTRL

  #pragma HLS INTERFACE m_axi port=matB depth=1048576 offset=slave bundle=matB
  #pragma HLS INTERFACE s_axilite port=matB bundle=CTRL

  #pragma HLS INTERFACE m_axi port=matout depth=1048576 offset=slave bundle=matout
  #pragma HLS INTERFACE s_axilite port=matout bundle=CTRL

  #pragma HLS INTERFACE s_axilite port=mat1r bundle=CTRL
  #pragma HLS INTERFACE s_axilite port=mat1cmat2r bundle=CTRL
  #pragma HLS INTERFACE s_axilite port=mat2c bundle=CTRL
  #pragma HLS INTERFACE s_axilite port=return bundle=CTRL


// matA (Rows x Columns) = mat1r x mat1cmat2r
// matB^T (Columns x Rows) = mat2c x mat1cmat2r
	// matB is inputed as a transposed matrix
	// matA columns and matB^T rows are the same; so they're combined into mat_sz[1]

	unsigned int matA_size = mat1r * mat1cmat2r;
	unsigned int matA_row = mat1r;
	unsigned int matA_col = mat1cmat2r;

	unsigned int matB_size = mat1cmat2r * mat2c;
	unsigned int matB_row = mat1cmat2r;
	unsigned int matB_col = mat2c;

	unsigned int matout_size = mat1r * mat2c;
	unsigned int matout_row = mat1r;
	unsigned int matout_col = mat2c;

	unsigned int matA_iter = matA_col; // How many matA units to gather on one burst (need to fix this to # columns right now)
	unsigned int matB_iter = matB_col; // How many matB columns or matB^T rows to gather on one burst (should be multiplicative of matB rows)
	unsigned int matout_iter = matout_col; // How many matout rows to write to memory at a time

	float buf1[32768] = {0};
	float buf2[32768] = {0};
	float buffout[32768] = {0};

	unsigned int outbuff_iter = 0;
	unsigned int outbuff_start = 0;

	float buffout_acc = 0;

	for(int matA_cnt=0; matA_cnt<matA_size; matA_cnt+=mat1cmat2r) // loop through matA one row at a time
	{
		matA.read_request(matA_cnt, mat1cmat2r); // load part of matA

		for(int aload = 0; aload < mat1cmat2r; aload++) // loading matA row into buf1
		{
#pragma HLS PIPELINE II=1
			buf1[aload] = matA.read(); // load in this way because we reuse buf1[0->matA_iter] (buffer for storing one row of matA); matA_cnt helps us manage where we are in the matA input array
		}

		for(int matB_cnt=0; matB_cnt<matB_size; matB_cnt+=mat1cmat2r)
		{
			matB.read_request(matB_cnt, mat1cmat2r); // load part of matB

			for(int bload = 0; bload < mat1cmat2r; bload++)
			{
	#pragma HLS PIPELINE II=1
				buf2[bload] = matB.read();
			}

			buffout_acc = 0;

			for(int A_iter_cnt = 0; A_iter_cnt < mat1cmat2r; A_iter_cnt++) // load matA with this variable
			{
#pragma HLS PIPELINE II=11 rewind
				buffout[A_iter_cnt] = buf1[A_iter_cnt] * buf2[A_iter_cnt];
			}

		//}
			for(int k = 0; k < mat1cmat2r; k++)
			{
#pragma HLS UNROLL
				buffout_acc += buffout[k];
			}


			matout.write_request(outbuff_iter, 1);
			matout.write(buffout_acc);
			matout.write_response();
			outbuff_iter++;

			/*for(int k = 0; k < matout_iter; k++)
			{
	#pragma HLS PIPELINE II=1
				float matout_buff = buf1[k]*buf2[k];
				matout.write(matout_buff);
			}

			matout.write_response();

			outbuff_iter++;*/
		}

		// if we've reached capacity of units stored in the output matrix
		/*
		if(outbuff_iter == matout_iter)
		{
			matout.write_request(outbuff_start, matout_iter);

			for(int k = outbuff_start; k < matout_iter; k++)
			{
	#pragma HLS PIPELINE II=1
				float matout_buff = buffout[k];
				matout.write(matout_buff);
			}

			matout.write_response();

			outbuff_start += outbuff_iter;
			outbuff_iter = 0;

			for(int i = 0; i < 32768; i++) buffout[i] = 0;
		}
		*/
	}
}
