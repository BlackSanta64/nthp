#pragma once
#include "global.hpp"

namespace nthp {
        namespace entity {
                struct cRect {
                        nthp::vectFixed x;
                        nthp::vectFixed y;
                        nthp::vectFixed w;
                        nthp::vectFixed h;
                };

                extern bool checkRectCollision(const cRect& a, const cRect& b);
        }
}