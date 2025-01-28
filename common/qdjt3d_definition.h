#pragma once

#include "Halide.h"

//     _ _                   _             _       _       _       _____     _
//  __| (_)___  ___ _ __ ___| |_ ___      (_) ___ (_)_ __ | |_    |___ /  __| |
// / _` | / __|/ __| '__/ _ \ __/ _ \_____| |/ _ \| | '_ \| __|____ |_ \ / _` |
//| (_| | \__ \ (__| | |  __/ ||  __/_____| | (_) | | | | | ||_____|__) | (_| |
// \__,_|_|___/\___|_|  \___|\__\___|    _/ |\___/|_|_| |_|\__|   |____/ \__,_|
//                                      |__/

class qDJT3d {
 private:
   typedef uint32_t dir_type;

 public:
   Halide::Func fm[MAX_IT];
   void qdjt3d(Halide::Func &in, Halide::Func &out, Halide::Var &x,
               Halide::Var &y, Halide::Var &z) {
      using namespace Halide;
      // Fill upper part
      const int n = MAX_IT - 1;
      fm[0](z, y, x) =
         in(cast<uint8_t>(z - VAL_N), cast<uint8_t>(y - VAL_N), x);
      for (int m = 0; m < MAX_IT - 1; m++) {
         Expr d1{"d1"};
         Expr d2{"d2"};
         Expr sigma1{"sigma1"};
         Expr sigma2{"sigma2"};
         Expr v{"v"};
         Expr s1_lsb{"s1_lsb"};
         Expr s2_lsb{"s2_lsb"};
         Expr dir_fm0{"dir_fm0"};
         Expr dir_fm1{"dir_fm1"};
         Expr f0{"f0"};
         Expr f1{"f1"};
         d1 = cast<dir_type>(y);
         d2 = cast<dir_type>(z);
         v = cast<dir_type>(x) & ((1 << (n - m - 1)) - 1);
         s1_lsb = (cast<dir_type>(x) >> (n - m - 1)) & 1;
         sigma1 = (cast<dir_type>(x) >> (n - m)) & ((1 << (m)) - 1);
         s2_lsb = (cast<dir_type>(x) >> n) & 1;
         sigma2 = (cast<dir_type>(x) >> (n + 1)) & ((1 << (m)) - 1);
         dir_fm0 = (sigma2 << n) + (sigma1 << (n - m)) + (v << 1);
         dir_fm1 = dir_fm0 + 1;
         //if (m == 0) {
            //sigma2 = print(sigma2, ", ", s2_lsb, ", ", sigma1, ", ", s1_lsb,
                           //", ", v, " : (", z, ",", y, ",", x, ")");
         //}
         f0 = fm[m](clamp(cast<uint16_t>(d2), 0, 2 * VAL_N - 1),
                    clamp(cast<uint16_t>(d1), 0, 2 * VAL_N - 1),
                    clamp(cast<uint16_t>(dir_fm0), 0, 1 << (n + m + 1)));
         f1 =
            fm[m](clamp(cast<uint16_t>(d2 + sigma2 + s2_lsb), 0, 2 * VAL_N - 1),
                  clamp(cast<uint16_t>(d1 + sigma1 + s1_lsb), 0, 2 * VAL_N - 1),
                  clamp(cast<uint16_t>(dir_fm1), 0, 1 << (n + m + 1)));
         fm[m + 1](z, y, x) = Halide::cast<uint8_t>(f0 + f1);
      }
      out(z, y, x) = fm[MAX_IT - 1](z, y, x);
   }

   void sched() {
      for (int i = 0; i < MAX_IT; i++) {
         fm[i].compute_root();
      }
   }
};
