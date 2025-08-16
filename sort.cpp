#include <hls_stream.h>
#include "sort.h"

#define NUM_LEAVES 16
//#define MAX_WINDOW_SIZE 2097152

//merge unit for small window
void merge_sort_unit(
    int window,
    int iteration,
    hls::stream<int>& right_stream,
    hls::stream<int>& left_stream,
    hls::stream<int>& output_stream
){
    #pragma HLS STREAM variable=right_stream depth=64
    #pragma HLS STREAM variable=left_stream depth=64
    #pragma HLS STREAM variable=output_stream depth=128

    #pragma HLS INLINE OFF
    
    ITERATION_LOOP: // Each iteration processes a batch of elements
    for (int iter = 0; iter < iteration; iter++) {
        
        int left_val, right_val;
        bool left_valid = true, right_valid = true;
        int left_count = 0, right_count = 0;
        int half_size = window / 2;
        
        // Read first elements for this iteration
        if (half_size > 0) {
            left_val = left_stream.read();
            left_count = 1;
        } else {
            left_valid = false;
        }
            
        if (half_size > 0) {
            right_val = right_stream.read();
            right_count = 1;
        } else {
            right_valid = false;
        }
        
        MERGE_LOOP: // Each loop processes the elements in a window
        for (int i = 0; i < window; i++) {
            #pragma HLS PIPELINE II=1
            
            if (!left_valid) {
                // Only right stream has data
                output_stream.write(right_val);
                if (right_count < half_size) {
                    right_val = right_stream.read();
                    right_count++;
                } else {
                    right_valid = false;
                }
            }
            else if (!right_valid) {
                // Only left stream has data
                output_stream.write(left_val);
                if (left_count < half_size) {
                    left_val = left_stream.read();
                    left_count++;
                } else {
                    left_valid = false;
                }
            }
            else {
                // Both streams have data - compare and select smaller
                if (left_val <= right_val) {
                    output_stream.write(left_val);
                    if (left_count < half_size) {
                        left_val = left_stream.read();
                        left_count++;
                    } else {
                        left_valid = false;
                    }
                } else {
                    output_stream.write(right_val);
                    if (right_count < half_size) {
                        right_val = right_stream.read();
                        right_count++;
                    } else {
                        right_valid = false;
                    }
                }
            }
        }
    }
}

void sort_tree(
    int window,
    int iteration,
    hls::stream<int> input_stream[16],
    hls::stream<int>& output_stream
){

    #pragma HLS dataflow
    hls::stream<int> stage0[8];
	hls::stream<int> stage1[4];
	hls::stream<int> stage2[2];

	for(int i = 0; i < 8; i++){ // First sort windows with 8 2-leaf merge unit
    #pragma HLS unroll
		merge_sort_unit(window,iteration,input_stream[i*2],input_stream[i*2+1],
				stage0[i]);
	}

	for(int i = 0; i < 4; i++){ // the 8 merge unit feed into 4 merge units
    #pragma HLS unroll
		merge_sort_unit(window*2,iteration,stage0[i*2],stage0[i*2+1],
				stage1[i]);
	}

    for(int i = 0; i < 2; i++){
    #pragma HLS unroll
		merge_sort_unit(window*4,iteration,stage1[i*2],stage1[i*2+1],
				stage2[i]);
	}
    // final merge unit of the tree
	merge_sort_unit(window*8,iteration,stage2[0],stage2[1],output_stream);

}


void load_unit(
    const int offset,
	const int iter,
	const int window,
	const int * dram,
	hls::stream<int>& out_stream
){
    for(int j = 0; j < iter ; j++){
		int off = j*NUM_LEAVES*window/2+ offset;
		for(int i = 0; i < window/2; i++){
			#pragma HLS pipeline II=1
			const auto value = dram[off + i];
			out_stream.write(value);
		}
	}
}

void store_unit(
    int size,
	hls::stream<int>& out_stream,
	int * dram_out
){

    for(int i = 0; i < size/4; i++){	        
        for (int j = 0; j < 4; j++) {
            const auto v = out_stream.read();	
            dram_out[j + (i*4)] = v;
        }
	}
}

void sort(
    int size,
    int iter,
	int window,
	const int * d0,
	const int * d1,
	const int * d2,
	const int * d3,
	const int * d4,
	const int * d5,
	const int * d6,
	const int * d7,
    const int * d8,
	const int * d9,
	const int * d10,
	const int * d11,
	const int * d12,
	const int * d13,
	const int * d14,
	const int * d15,
	int * dram_out
){
    //16 input dram pointer to feed into each merge unit
    #pragma HLS interface m_axi port=d0 bundle=dram0 depth=256
    #pragma HLS interface m_axi port=d1 bundle=dram1 depth=256
    #pragma HLS interface m_axi port=d2 bundle=dram2 depth=256
    #pragma HLS interface m_axi port=d3 bundle=dram3 depth=256
    #pragma HLS interface m_axi port=d4 bundle=dram4 depth=256
    #pragma HLS interface m_axi port=d5 bundle=dram5 depth=256
    #pragma HLS interface m_axi port=d6 bundle=dram6 depth=256
    #pragma HLS interface m_axi port=d7 bundle=dram7 depth=256
    #pragma HLS interface m_axi port=d8 bundle=dram8 depth=256
    #pragma HLS interface m_axi port=d9 bundle=dram9 depth=256
    #pragma HLS interface m_axi port=d10 bundle=dram10 depth=256
    #pragma HLS interface m_axi port=d11 bundle=dram11 depth=256
    #pragma HLS interface m_axi port=d12 bundle=dram12 depth=256
    #pragma HLS interface m_axi port=d13 bundle=dram13 depth=256
    #pragma HLS interface m_axi port=d14 bundle=dram14 depth=256
    #pragma HLS interface m_axi port=d15 bundle=dram15 depth=256
    #pragma HLS interface m_axi port=dram_out bundle=dram16 depth=256

    #pragma HLS dataflow
	hls::stream<int> input_stream[16];
    hls::stream<int> out_stream;

    const int* input_arrays[16] = {d0,d1,d2,d3,d4,d5,d6,d7,d8,d9,d10,d11,d12,d13,d14,d15};

	for(int i = 0; i < 16; i++){
		#pragma HLS unroll
		int offset = (window/2)*i;
		load_unit(offset,iter,window,input_arrays[i],input_stream[i]);
	}
	sort_tree(window,iter,input_stream,out_stream);

	store_unit(size,out_stream,dram_out);

}