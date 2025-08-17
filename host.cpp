#include <xrt/xrt_device.h>
#include <experimental/xrt_xclbin.h>
#include <xrt/xrt_bo.h>
#include <xrt/xrt_kernel.h>
#include <cstdio>
#include <sys/time.h>
#include "utils.h"
#include <cstdlib>
#include <cmath>



int compare (const void * a, const void * b)
{
  return ( *(int*)a <  *(int*)b ) ? -1 : 1;
}


int main(int argc, char* argv[]){
        unsigned int dev_index = 0;
        auto device = xrt::device(dev_index);
        auto xclbin_uuid = device.load_xclbin("sort.xclbin");
        auto krnl = xrt::kernel(device, xclbin_uuid, "sort:{sort_1}",xrt::kernel::cu_access_mode::exclusive);

        int size = atoi(argv[1]);
        //int size = 16777216;
        int iter = size/16;
        int window = 2;
        int passes = std::log(size) / std::log(16);
        bool even_passes = ((passes % 2) == 0) ? true : false;

        printf("Group id: %d\n",krnl.group_id(3));
        xrt::bo input_buffer = xrt::bo(device,size*sizeof(int),krnl.group_id(3));
        xrt::bo output_buffer = xrt::bo(device,size*sizeof(int),krnl.group_id(3));


        int * inputs = new int[size];
        int * output = new int[size];

        double t0, t1, t2, t3, t4, t5;
        double timer_array[6] = {t0, t1, t2, t3, t4, t5};

        for(int i = 0; i < size; i++){
                inputs[i]  = size -i;
        }

        input_buffer.write(inputs);


#ifndef MANUAL_START
        xrt::run run(krnl);
        run.set_arg(0,size);

        auto start = mtick();
        for (int i = 0; i < passes; i++) {
                run.set_arg(1,iter);
                run.set_arg(2,window);
                for(int j = 0; j < 16; j++){
                        if(i%2 == 0){
                                run.set_arg(3 + j ,input_buffer);
                        }else{
                                run.set_arg(3 + j ,output_buffer);
                        }
                }
                if(i%2 == 0){
                        run.set_arg(19, output_buffer);
                }else{
                        run.set_arg(19, input_buffer);
                }
                start = mtick();
                run.start();
                run.wait();
                timer_array[i] = mtock(start);

                iter = iter/16;
                window = window *16;

        }

#else

        auto start = mtick();
        for (int i = 0; i < passes; i++) {
                writeR32(krnl,0,size);
                writeR32(krnl,1,iter);
                writeR32(krnl,2,window);
                for(int j = 0; j < 16; j++){
                        if(i%2 == 0){
                                writeR64(krnl,3 + j ,input_buffer.address());
                        }else{
                                writeR64(krnl,3 + j ,output_buffer.address());
                        }
                }
                if(i%2 == 0){
                        writeR64(krnl,19,output_buffer.address());
                }else{
                        writeR64(krnl,19,input_buffer.address());
                }
                start = mtick();
                manualRun(krnl);
                timer_array[i] = mtock(start);

                iter = iter/16;
                window = window *16;

        }

#endif

        /*
        start = mtick();
        qsort(inputs,size,sizeof(int),compare);
        double t6 = mtock(start);
        printf("CPU time: %lf ms\n",t6);
        */

        if(even_passes){
                input_buffer.read(output);
        }else{
                output_buffer.read(output);
        }

        printf("Total elements sorted: %d\n", size);

        bool sorted = true;
        for (int i = 1; i < size; i++){
                if(output[i] < output[i-1]){
                        sorted = false;
                        break;
                }
        }
        if(sorted){
                printf("Array sorted correctly!");
        }

        /*
        printf("Outputs:");
        for(int i = 0; i < 256; i++){
                printf("%d,",output[i]);
        }
        */
        printf("\n");

        printf("Time first pass %lf [ms], time second %lf, third %lf, forth %lf, fifth %lf, sixth %lf\n\n",timer_array[0],timer_array[1],timer_array[2],timer_array[3],timer_array[4],timer_array[5]);

        //printf("Time first pass %lf [ms], time second %lf, third %lf, forth %lf, fifth %lf\n\n",t0,t1,t2,t3,t4);

        delete [] inputs;
        delete [] output;
        return 0;
}