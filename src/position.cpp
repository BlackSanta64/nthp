#include "position.hpp"



// Takes a worldPosition (virtual position) and returns a fixed-type length primitive integer vector, with both coordinates equal to the absolute position
// in the window pixel buffer of the worldPosition object. (i.e Virtual Position -> Render Position)
nthp::vector<FIXED_TYPE> nthp::generatePixelPosition(nthp::worldPosition pos, nthp::RenderRuleSet* ruleset) {
        return nthp::vector<FIXED_TYPE>(nthp::fixedToInt(nthp::f_fixedProduct(pos.x, ruleset->scaleFactor.x) + ruleset->cameraWorldPosition.x), nthp::fixedToInt(nthp::f_fixedProduct(pos.y, ruleset->scaleFactor.y)  + ruleset->cameraWorldPosition.y));
}

nthp::vector<nthp::fixed_t> nthp::generateWorldPosition(nthp::vector<FIXED_TYPE> pos, nthp::RenderRuleSet* ruleset) {
        return nthp::vector<nthp::fixed_t>((nthp::f_fixedQuotient(nthp::intToFixed(pos.x), ruleset->scaleFactor.x) - ruleset->cameraWorldPosition.x), (nthp::f_fixedQuotient(nthp::intToFixed(pos.y), ruleset->scaleFactor.y) - ruleset->cameraWorldPosition.y));
}
