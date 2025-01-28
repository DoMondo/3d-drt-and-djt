#include "HalideBuffer.h"
#include "HalideRuntime.h"
#include "halide_benchmark.h"
#include "qdjt3d.h"
#include "qdrt3f.h"
#include <cmath>
#include <string>

#ifdef _WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#endif

bool have_opencl_or_metal() {
#ifdef _WIN32
   return LoadLibrary("OpenCL.dll") != NULL;
#elif __APPLE__
   return dlopen("/System/Library/Frameworks/Metal.framework/Versions/Current/"
                 "Metal",
                 RTLD_LAZY) != NULL;
#else
   return dlopen("libOpenCL.so", RTLD_LAZY) != NULL;
#endif
}

void test_radon_drt3f() {
   Halide::Runtime::Buffer<uint8_t> input(VAL_N, VAL_N, VAL_N);
   for (int x = 0; x < VAL_N; x++)
      for (int y = 0; y < VAL_N; y++)
         for (int z = 0; z < VAL_N; z++) {
            if ((x == y) && (x == z))
               input(x, y, z) = y + 1;
            else
               input(x, y, z) = 0;
         }
   // printf("input = \n");
   // for (int c = 0; c < VAL_N; c++)
   // for (int j = 0; j < input.height(); j++)
   // for (int i = 0; i < input.width(); i++)
   // printf("f(%d,%d,%d) = %d \n", i, j, c, input(i, j, c));
   Halide::Runtime::Buffer<uint8_t> output(VAL_N, VAL_N, VAL_N * 3);
   double time =
      Halide::Tools::benchmark(2, 10, [&]() { qdrt3f(input, output); });
   printf("qDRT3f time:\t  %g ms\n", time * 1e3);
   output.copy_to_host();
   // printf("output = \n");
   // for (int c = 0; c < VAL_N*3; c++)
   // for (int j = 0; j < output.height(); j++)
   // for (int i = 0; i < output.width(); i++)
   // printf("f(%d,%d,%d) = %d \n", i, j, c, output(i, j, c));
}

void test_radon_djt3f() {
   Halide::Runtime::Buffer<uint8_t> input(VAL_N, VAL_N, VAL_N);
   for (int x = 0; x < VAL_N; x++)
      for (int y = 0; y < VAL_N; y++)
         for (int z = 0; z < VAL_N; z++) {
            if (z == 0 && y == 0)
               //input(z, y, x) = 1;
            //else
               input(z, y, x) = 0;
         }
   input(0,0,0) = 1;
   input(0,0,1) = 1;
   input(1,1,2) = 1;
   input(1,1,3) = 1;
   //printf("input = \n");
   //for (int c = 0; c < VAL_N; c++)
      //for (int j = 0; j < input.height(); j++)
         //for (int i = 0; i < input.width(); i++)
            //printf("f(%d,%d,%d) = %d \n", i, j, c, input(i, j, c));
   Halide::Runtime::Buffer<uint8_t> output(VAL_N * 2, VAL_N * 2, VAL_N * VAL_N);
   double time =
      Halide::Tools::benchmark(2, 10, [&]() { qdjt3d(input, output); });
   printf("qDJT3f time:\t  %g ms\n", time * 1e3);
   output.copy_to_host();
   //printf("output = \n");
   //for (int c = 0; c < VAL_N * VAL_N; c++)
      //for (int j = 0; j < output.height(); j++)
         //for (int i = 0; i < output.width(); i++)
            //printf("f(%d,%d,%d) = %d \n", i, j, c, output(i, j, c));
}

int main(int argc, char *argv[]) {
   printf("N = %d\n", VAL_N);
   test_radon_drt3f();
   test_radon_djt3f();
   return 0;
}
