#include "qdjt3d.h"

// A quad of forward discrete radon transform (1024x1024)
namespace {

void qDJT3d_generator::generate() {
   Func clamped_in = Halide::BoundaryConditions::constant_exterior(in, 0);
   q.qdjt3d(clamped_in, q_out, z, y, x);
   out(z, y, x) = q_out(z, y, x);
}

void qDJT3d_generator::schedule() {
   if (auto_schedule) {
      // for (int i = 0; i < MAX_IT; i++)
      // fm[i].estimate(x, 0, 1024).estimate(y, 0, 1024);
      in.dim(0).set_bounds_estimate(0, VAL_N);
      in.dim(1).set_bounds_estimate(0, VAL_N);
      in.dim(2).set_bounds_estimate(0, VAL_N);
      out.estimate(z, 0, 2 * VAL_N)
         .estimate(y, 0, 2 * VAL_N)
         .estimate(x, 0, VAL_N * VAL_N);
   } else {
      q.sched();
      out.compute_root();
   }
}

} // namespace

HALIDE_REGISTER_GENERATOR(qDJT3d_generator, qdjt3d)
