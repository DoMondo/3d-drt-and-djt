#pragma once

#include "Halide.h"
#include "../qdjt3d_definition.h"

// A quad of forward discrete radon transform (1024x1024)
namespace {

class qDJT3d_generator : public Halide::Generator<qDJT3d_generator> {
 private:
   qDJT3d q;
   Func q_out;
   Func clamped_in;

 public:
   Var x{"x"}, y{"y"}, z{"z"};
   Input<Buffer<uint8_t>> in{"in", 3};
   Output<Buffer<uint8_t>> out{"out", 3};

   void generate();

   void schedule();
};

} // namespace
