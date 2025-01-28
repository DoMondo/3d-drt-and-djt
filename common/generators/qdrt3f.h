#pragma once

#include "Halide.h"
#include "../qdrt3f_definition.h"

// A quad of forward discrete radon transform (1024x1024)
namespace {

class qDRT3f_generator : public Halide::Generator<qDRT3f_generator> {
 private:
   qDRT3f q;
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
