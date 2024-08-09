#include "e_entity.hpp"


nthp::entity::gEntity::gEntity() {
        frameData = nullptr;
        frameSize = 0;
        frameDataNative = false;

        currentFrame = 0;
}


const nthp::RenderPacket nthp::entity::gEntity::getUpdateRenderPacket(nthp::RenderRuleSet* context) {
        auto pxlPos = nthp::generatePixelPosition(wPosition, context);
// TODO

        return nthp::generateRenderPacket(frameData[currentFrame].texture, &frameData[currentFrame].src, 
                {
                        (int)pxlPos.x, 
                        (int)pxlPos.y, 
                        (int)nthp::fixedToInt(nthp::f_fixedProduct(renderSize.x, context->scaleFactor.x)),
                        (int)nthp::fixedToInt(nthp::f_fixedProduct(renderSize.y, context->scaleFactor.y))
                });
}


// Gets the entity RenderPacket
const nthp::RenderPacket nthp::entity::gEntity::abs_getRenderPacket(nthp::RenderRuleSet* context) {
        auto pxlPos = nthp::generatePixelPosition(wPosition, context);
        

         return nthp::generateRenderPacket(frameData[currentFrame].texture, &frameData[currentFrame].src, 
                {
                        (int)pxlPos.x, 
                        (int)pxlPos.y,
                        (int)nthp::fixedToInt(renderSize.x),
                        (int)nthp::fixedToInt(renderSize.y)
                });
}



void nthp::entity::gEntity::setRenderSize(nthp::vectFixed size) {
        renderSize = size;
}


void nthp::entity::gEntity::importFrameData(nthp::texture::Frame* data, size_t size, bool native) {
        frameData = data;
        frameSize = size;

        frameDataNative = native;
}


void nthp::entity::gEntity::setPosition(nthp::vectFixed newPos) {
        wPosition = newPos;
}

void nthp::entity::gEntity::move(nthp::vectFixed offset) {
        wPosition += offset;
}

void nthp::entity::gEntity::setCurrentFrame(size_t cf) {
        currentFrame = cf;
}

nthp::entity::gEntity::~gEntity() {
        if(frameDataNative) {
                free(frameData);
        }
}