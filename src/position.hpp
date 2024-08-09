#pragma once
#include "global.hpp"
#include "core.hpp"

namespace nthp {

        typedef nthp::vectFixed worldPosition;
        
        extern nthp::vector<FIXED_TYPE> generatePixelPosition(nthp::worldPosition pos, nthp::RenderRuleSet* ruleset);
        extern nthp::vector<nthp::fixed_t> generateWorldPosition(nthp::vector<FIXED_TYPE> pos, nthp::RenderRuleSet* ruleset);
}