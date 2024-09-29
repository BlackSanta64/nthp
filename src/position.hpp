#pragma once
#include "global.hpp"

namespace nthp {

        typedef nthp::vectFixed worldPosition;
        
        extern nthp::vectGen generatePixelPosition(nthp::worldPosition pos, nthp::RenderRuleSet* ruleset);
        extern nthp::vectFixed generateWorldPosition(vectGen pos, nthp::RenderRuleSet* ruleset);
}
