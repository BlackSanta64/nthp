#pragma once
#include "global.hpp"

namespace nthp {
        namespace entity {
                struct cRect {
                        nthp::fixed_t x;
                        nthp::fixed_t y;
                        nthp::fixed_t w;
                        nthp::fixed_t h;
                };

                extern int checkRectCollision(cRect a,cRect b);
        }
}
