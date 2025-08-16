#include <iostream>
#include <hls_stream.h>
#include "sort.h"

int main() {
    const int size = 16777216;
    const int iteration = size/16;
    const int window = 2; 
    //const int array_size = size/16;

    //use one array with dn being pointers

    int input_array[size];

    for(int i = 0; i < size; i++){
        input_array[i] = size - i;
    }

    /*
    int d0[array_size];
    int d1[array_size];
    int d2[array_size];
    int d3[array_size];
    int d4[array_size];
    int d5[array_size];
    int d6[array_size];
    int d7[array_size];
    int d8[array_size];
    int d9[array_size];
    int d10[array_size];
    int d11[array_size];
    int d12[array_size];
    int d13[array_size];
    int d14[array_size];
    int d15[array_size];

    for (int i = 0; i < array_size; i++) {
        d0[i] = size - (0 * array_size + i);
        d1[i] = size - (1 * array_size + i);
        d2[i] = size - (2 * array_size + i);
        d3[i] = size - (3 * array_size + i);
        d4[i] = size - (4 * array_size + i);
        d5[i] = size - (5 * array_size + i);
        d6[i] = size - (6 * array_size + i);
        d7[i] = size - (7 * array_size + i);
        d8[i] = size - (8 * array_size + i);
        d9[i] = size - (9 * array_size + i);
        d10[i] = size - (10 * array_size + i);
        d11[i] = size - (11 * array_size + i);
        d12[i] = size - (12 * array_size + i);
        d13[i] = size - (13 * array_size + i);
        d14[i] = size - (14 * array_size + i);
        d15[i] = size - (15 * array_size + i);
    }
    */
    
    /*
    int d0[1] = {16};
    int d1[1] = {15};
    int d2[1] = {14};
    int d3[1] = {13};
    int d4[1] = {12};
    int d5[1] = {11};
    int d6[1] = {10};
    int d7[1] = {9};
    int d8[1] = {8};
    int d9[1] = {7};
    int d10[1] = {6};
    int d11[1] = {5};
    int d12[1] = {4};
    int d13[1] = {3};
    int d14[1] = {2};
    int d15[1] = {1};
    */
    /*
    int d0[2] = {32, 31};
    int d1[2] = {30, 29};
    int d2[2] = {28, 27};
    int d3[2] = {26, 25};
    int d4[2] = {24, 23};
    int d5[2] = {22, 21};
    int d6[2] = {20, 19};
    int d7[2] = {18, 17};
    int d8[2] = {16, 15};
    int d9[2] = {14, 13};
    int d10[2] = {12, 11};
    int d11[2] = {10, 9};
    int d12[2] = {8, 7};
    int d13[2] = {6, 5};
    int d14[2] = {4, 3};
    int d15[2] = {2, 1};
    */
    int output[size];
    
    for(int i = 0; i < size; i++) {
        output[i] = -1;
    }
      
    sort(size, iteration, window,
         input_array, input_array, input_array, input_array, input_array, input_array, input_array, input_array,
         input_array, input_array, input_array, input_array, input_array, input_array, input_array, input_array,
         output);

    sort(size, iteration/16, window*16,
         output, output, output, output, output, output, output, output,
         output, output, output, output, output, output, output, output,
         input_array);
    
    sort(size, iteration/16/16, window*16*16,
         input_array, input_array, input_array, input_array, input_array, input_array, input_array, input_array,
         input_array, input_array, input_array, input_array, input_array, input_array, input_array, input_array,
         output);

    sort(size, iteration/16/16/16, window*16*16*16,
         output, output, output, output, output, output, output, output,
         output, output, output, output, output, output, output, output,
         input_array);

    sort(size, iteration/16/16/16/16, window*16*16*16*16,
         input_array, input_array, input_array, input_array, input_array, input_array, input_array, input_array,
         input_array, input_array, input_array, input_array, input_array, input_array, input_array, input_array,
         output);

    sort(size, iteration/16/16/16/16/16, window*16*16*16*16*16,
         output, output, output, output, output, output, output, output,
         output, output, output, output, output, output, output, output,
         input_array);
 
    /*
    std::cout << "Sorted output:" << std::endl;
    for(int i = 0; i < size; i++) {
        std::cout << input_array[i] << " ";
    }
    std::cout << std::endl << std::endl;
    */
    bool is_sorted = true;
    for(int i = 1; i < size; i++) {
        if(input_array[i] < input_array[i-1]) {
            is_sorted = false;
            break;
        }
    }
    
    if(is_sorted) {
        std::cout << "SUCCESS: Array is sorted correctly!" << std::endl;
        
        bool has_all_elements = true;
        for(int expected = 1; expected <= size; expected++) {
            bool found = false;
            for(int i = 0; i < size; i++) {
                if(input_array[i] == expected) {
                    found = true;
                    break;
                }
            }
            if(!found) {
                has_all_elements = false;
                std::cout << "Missing element: " << expected << std::endl;
            }
        }
        
        if(has_all_elements) {
            std::cout << "All elements are present." << std::endl;
        } else {
            std::cout << "ERROR: Some elements are missing!" << std::endl;
        }
    } else {
        std::cout << "FAILURE: Array is NOT sorted correctly!" << std::endl;
    }
    
    return 0;
}