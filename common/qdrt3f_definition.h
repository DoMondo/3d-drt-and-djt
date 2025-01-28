#pragma once

#include "Halide.h"

class qDRT3f {
 private:
   typedef uint32_t dir_type;

 public:
   Halide::Func fm[MAX_IT];
   void qdrt3f(Halide::Func &in, Halide::Func &out, Halide::Var &x,
               Halide::Var &y, Halide::Var &z) {
      using namespace Halide;
      // Fill upper part
      // fm[0](x, y, z) = in(x, y, cast<uint8_t>(clamp(z - 2 * VAL_N));
      fm[0](x, y, z) = in(x, y, cast<uint8_t>(z - 2 * VAL_N));
      for (dir_type m = 0; m < MAX_IT - 1; m++) {
         Expr sigma1{"sigma1"};
         Expr sigma2{"sigma2"};
         Expr v1{"v1"};
         Expr v2{"v2"};
         Expr s1_lsb{"s1_lsb"};
         Expr s2_lsb{"s2_lsb"};
         Expr dir_fm1{"dir_fm1"};
         Expr dir_fm2{"dir_fm2"};
         Expr f00{"f00"};
         Expr f01{"f01"};
         Expr f10{"f10"};
         Expr f11{"f11"};
         Expr aux{"aux"};
         aux = cast<dir_type>(1 << m);
         v1 = cast<dir_type>(x) >> (m + 1);
         v2 = cast<dir_type>(y) >> (m + 1);
         sigma1 = (cast<dir_type>(x) & ((1 << (m + 1)) - 1)) >> 1;
         sigma2 = (cast<dir_type>(y) & ((1 << (m + 1)) - 1)) >> 1;
         s1_lsb = cast<dir_type>(x) & 1;
         s2_lsb = cast<dir_type>(y) & 1;
         dir_fm1 = cast<dir_type>(sigma1 + (v1 << (m + 1)));
         dir_fm2 = cast<dir_type>(sigma2 + (v2 << (m + 1)));
         f00 = fm[m](clamp(cast<uint16_t>(dir_fm1), 0, VAL_N - 1),
                     clamp(cast<uint16_t>(dir_fm2), 0, VAL_N - 1),
                     clamp(z, 0, 3 * VAL_N - 1));
         f10 = select(z + sigma1 + s1_lsb >= VAL_N * 3, 0,
                      fm[m](clamp(cast<uint16_t>(dir_fm1 + aux), 0, VAL_N - 1),
                            clamp(cast<uint16_t>(dir_fm2), 0, VAL_N - 1),
                            clamp(cast<uint16_t>(z + sigma1 + s1_lsb), 0,
                                  3 * VAL_N - 1)));
         f01 = select(z + sigma2 + s2_lsb >= VAL_N * 3, 0,
                      fm[m](clamp(cast<uint16_t>(dir_fm1), 0, VAL_N - 1),
                            clamp(cast<uint16_t>(dir_fm2 + aux), 0, VAL_N - 1),
                            clamp(cast<uint16_t>(z + sigma2 + s2_lsb), 0,
                                  3 * VAL_N - 1)));
         f11 = select(
            z + sigma1 + s1_lsb + sigma2 + s2_lsb >= VAL_N * 3, 0,
            fm[m](clamp(cast<uint16_t>(dir_fm1 + aux), 0, VAL_N - 1),
                  clamp(cast<uint16_t>(dir_fm2 + aux), 0, VAL_N - 1),
                  clamp(cast<uint16_t>(z + sigma1 + s1_lsb + sigma2 + s2_lsb),
                        0, VAL_N * 3 - 1)));
         fm[m + 1](x, y, z) = Halide::cast<uint8_t>(f00 + f01 + f10 + f11);
      }
      out(x, y, z) = fm[MAX_IT - 1](x, y, z);
   }
   void sched() {
      for (int i = 0; i < MAX_IT; i++) {
         fm[i].compute_root();
      }
   }
};
