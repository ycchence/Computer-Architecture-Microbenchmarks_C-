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

    void *map = mmap(NULL, 12*1024*1024, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, 0, 0);
    int *data = (int*)map;
    for (int i = 0;i< 12*1024*1024/sizeof(int);i++) {
        data[i]++;
    }

    int steps[] = { 1*1024, 2*1024, 4*1024, 8*1024, 16*1024, 32*1024, 64*1024, 128*1024, 
                    256*1024, 512*1024, 1024*1024, 2*1024*1024, 3*1024*1024, 4*1024*1024, 5*1024*1024, 6*1024*1024 };
    for (int i = 0; i <= 15; i++) {
        double t_total, t_h, t_l;
        t_l = read_tsc();    
        for (int k = 0; k < times; k++) {
            int size = steps[i] / sizeof(int) - 1;
            for (int j = 0; j < repeats; j++) {
                ++data[ (j * 64) & size ];
                
            }
            
        }
        t_h = read_tsc();
        t_total = (t_h-t_l)/2000000000.0;
        FILE* f = fopen("results.txt", "w");
        fprintf(f, "%d, %f\n", steps[i] / KB, t_total);
        printf("%d time: %lf\n", steps[i] / KB, t_total);
    }
    munmap(map, (size_t)data_size);
    return 0;
}
