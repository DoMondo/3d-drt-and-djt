#include "qdrt3f.h"

// A quad of forward discrete radon transform (1024x1024)
namespace {

void qDRT3f_generator::generate() {
   Func clamped_in = Halide::BoundaryConditions::constant_exterior(in, 0);
   q.qdrt3f(clamped_in, q_out, x, y, z);
   out(x, y, z) = q_out(x, y, z);
}

void qDRT3f_generator::schedule() {
   if (auto_schedule) {
      // for (int i = 0; i < MAX_IT; i++)
      // fm[i].estimate(x, 0, 1024).estimate(y, 0, 1024);
      in.dim(0).set_bounds_estimate(0, VAL_N);
      in.dim(1).set_bounds_estimate(0, VAL_N);
      in.dim(2).set_bounds_estimate(0, VAL_N);
      out.estimate(x, 0, VAL_N).estimate(y, 0, VAL_N).estimate(z, 0, 3 * VAL_N);
   } else {
      q.sched();
      out.compute_root();
   }
}

} // namespace

HALIDE_REGISTER_GENERATOR(qDRT3f_generator, qdrt3f)
