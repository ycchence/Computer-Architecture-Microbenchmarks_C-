#include <stdint.h>
#include <cstdio>
#include <time.h>
#include <sys/mman.h>


const int KB = 1024;
const int MB = 1024 * KB;
const int data_size = 32 * MB;
const int repeats = 64 * MB;
const int steps = 8 * MB;
const int times = 8;


int cacheflush(){
    void *map = mmap(NULL, 20*1024*1024, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, 0, 0);
    int *dummy = (int*)map;
    for (int i = 0;i< 12*1024*1024/sizeof(int);i++) {
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

   // int data[12*1024*1024];
    int* data = new int[12*1024*1024];
    for (int i = 0;i< 12*1024/sizeof(int);i++) {
        data[i]*=87;
    }

    int steps[] = { 1, 2, 4, 8, 16, 32, 64, 128, 256 };
    for (int i = 0; i <= 8; i++) {
        double t_total, t_h, t_l;
        cacheflush();
        t_l = read_tsc();    
        for (int k = 0; k < 10; k++) {
            for (int j = 0; j < MB; j++) {
                ++data[ k+steps[i] ];
                
            }
            
        }
        t_h = read_tsc();
        t_total = (t_h-t_l);
        printf("%d time: %lf\n", steps[i] / KB, t_total);
    }
    
    return 0;
}
