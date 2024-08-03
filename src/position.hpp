#pragma once
#include "global.hpp"
#include "core.hpp"

namespace nthp {

        struct worldPosition {
                nthp::fixed_t x;
                nthp::fixed_t y;
        };



        extern nthp::vector<FIXED_TYPE> generatePixelPosition(nthp::worldPosition pos, nthp::RenderRuleSet* ruleset);

}