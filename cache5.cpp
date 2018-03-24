#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <cstdio>
#include <time.h>
#include <sys/mman.h>
#include <unistd.h>


const int KB = 1024;
const int MB = 1024 * KB;
const int data_size = 32 * MB;
const int repeats = 64 * MB;
const int steps = 8 * MB;
const int times = 18;



int cacheflush(){
    void *map = mmap(NULL, 64*1024*1024, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, 0, 0);
    int *dummy = (int*)map;
    for (int i = 0;i< 64*1024*1024/sizeof(int);i++) {
        dummy[i]++;
    }
   // munmap(map, (size_t)data_size);
}


uint64_t read_tsc(void) {
    uint64_t hi, lo;
    /* 
     * Embed the assembly instruction 'rdtsc', which should not be relocated (`volatile').
     *  The instruction will modify r_a_x and r_d_x, which the compiler should map to
     * lo and hi, respectively.
     * 
     * The format for GCC-style inline assembly generally is:
     * __asm__ ( ASSEMBLY CODE : OUTPUTS : INPUTS : OTHER THINGS CHANGED )
     *
     * Note that if you do not (correctly) specify the side effects of an assembly operation, the
     * compiler may assume that other registers and memory are not affected. This can easily
     * lead to cases where your program will produce difficult-to-debug wrong answers when
     * optimizations are enabled.
     */
    __asm__ volatile ( "rdtsc" : "=a"(lo), "=d"(hi));
    return lo | (hi << 32);
}


int main() {

    int* data = new int[64*1024*1024];
    for (int i = 0;i< 64*1024*1024/sizeof(int);i++) {
        data[i]*=87;
    }

    int steps[] = { 10, 20,40, 80, 160, 320, 630, 1280, 2560};
    int random;
    for (int i = 0; i <= 0; i++) {
        double t_total=0, t_h, t_l;
        
    //    t_l = read_tsc();    
        for (int k = 1; k < times; k++) {
            int size = sizeof(64*1024*1024)/sizeof(int) - 1;
            cacheflush();
            for (int j = 0; j < repeats; j++) {
                random =(rand() % (64*1024*1024-64+1)) +64;
                t_l = read_tsc();
                t_h = read_tsc();
                data[ (random*j*64) & size]=88;
                t_h = read_tsc();
                t_total = (t_h-t_l)+t_total;
            }
            
        }
     //   t_h = read_tsc();
    //    t_total = (t_h-t_l);
        printf("%d time: %lf\n", steps[i] / KB, t_total/2000000000);
    }
    
    return 0;
}
