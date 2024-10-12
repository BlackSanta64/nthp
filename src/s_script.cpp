
#include "s_script.hpp"
using namespace nthp::script::instructions;
#define DEFINE_EXECUTION_BEHAVIOUR(instruction) const int instruction (nthp::script::Script::ScriptDataSet* data)

uint8_t nthp::script::stageMemory[UINT8_MAX];
nthp::texture::Palette nthp::script::activePalette;

#define EVAL_STDREF(ref)   if(PR_METADATA_GET(ref, nthp::script::flagBits::IS_REFERENCE)) {\
                                if(PR_METADATA_GET(ref, nthp::script::flagBits::IS_GLOBAL)) {\
                                        ref.value = data->globalVarSet[nthp::fixedToInt(ref.value)];\
                                }\
                                else {\
                                        ref.value = (*data->currentLocalMemory)[nthp::fixedToInt(ref.value)];\
                                }\
                        }




DEFINE_EXECUTION_BEHAVIOUR(EXIT) {
        return 0;
}

DEFINE_EXECUTION_BEHAVIOUR(HEADER) {
        return 0;
}

DEFINE_EXECUTION_BEHAVIOUR(LABEL) {
        return 0;
}

// Fastest instruction jumping.
DEFINE_EXECUTION_BEHAVIOUR(GOTO) {
        data->currentNode = (*(uint32_t*)data->nodeSet[data->currentNode].access.data);
        --data->currentNode;

        return 0;
}

DEFINE_EXECUTION_BEHAVIOUR(JUMP) {
        stdRef label = *(stdRef*)data->nodeSet[data->currentNode].access.data;
        uint32_t static_label = nthp::fixedToInt(label.value);

        EVAL_STDREF(label);

        for(size_t i = 0; i < data->currentLabelBlockSize; ++i) {
                if(data->currentLabelBlock[i + i] == static_label) {
                        data->currentNode = data->currentLabelBlock[i + i + 1];
                        break;
                }
        }
        return 0;
}

DEFINE_EXECUTION_BEHAVIOUR(SUSPEND) {
        data->isSuspended = true;

        return 0;
}

DEFINE_EXECUTION_BEHAVIOUR(RETURN) {
        stdRef index = *(stdRef*)data->nodeSet[data->currentNode].access.data;

        EVAL_STDREF(index);
        data->currentNode = index.value;

        return 0;
}

DEFINE_EXECUTION_BEHAVIOUR(GETINDEX) {
        indRef var = *(indRef*)data->nodeSet[data->currentNode].access.data;

        if(PR_METADATA_GET(var, nthp::script::flagBits::IS_GLOBAL)) {
                data->globalVarSet[var.value] = nthp::intToFixed(data->currentNode);
        }
        else {
                (*data->currentLocalMemory)[var.value] = nthp::intToFixed(data->currentNode);
        }
        return 0;
}

DEFINE_EXECUTION_BEHAVIOUR(INC) {
        indRef var = *(indRef*)data->nodeSet[data->currentNode].access.data;

        if(PR_METADATA_GET(var, nthp::script::flagBits::IS_GLOBAL)) {
                data->globalVarSet[var.value] += nthp::intToFixed(1);
        }
        else {
                (*data->currentLocalMemory)[var.value] += nthp::intToFixed(1);
        }

        return 0;
}

DEFINE_EXECUTION_BEHAVIOUR(DEC) {
        indRef var = *(indRef*)data->nodeSet[data->currentNode].access.data;

        if(PR_METADATA_GET(var, nthp::script::flagBits::IS_GLOBAL)) {
                data->globalVarSet[var.value] -= nthp::intToFixed(1);
        }
        else {
                (*data->currentLocalMemory)[var.value] -= nthp::intToFixed(1);
        }

        return 0;
}

DEFINE_EXECUTION_BEHAVIOUR(LSHIFT) {
        indRef var =  *(indRef*)data->nodeSet[data->currentNode].access.data;
        stdRef count = *(stdRef*)(data->nodeSet[data->currentNode].access.data + sizeof(indRef));

        EVAL_STDREF(count);
        
        if(PR_METADATA_GET(var, nthp::script::flagBits::IS_GLOBAL)) {
                data->globalVarSet[var.value] = ((data->globalVarSet[var.value]) << nthp::fixedToInt(count.value));
        }
        else {                
                (*data->currentLocalMemory)[var.value] = (((*data->currentLocalMemory)[var.value]) << nthp::fixedToInt(count.value));
        }

        return 0;
}

DEFINE_EXECUTION_BEHAVIOUR(RSHIFT) {
        indRef var =  *(indRef*)data->nodeSet[data->currentNode].access.data;
        stdRef count = *(stdRef*)(data->nodeSet[data->currentNode].access.data + sizeof(indRef));

        EVAL_STDREF(count);
        
        if(PR_METADATA_GET(var, nthp::script::flagBits::IS_GLOBAL)) {
                data->globalVarSet[var.value] = ((data->globalVarSet[var.value]) >> nthp::fixedToInt(count.value));
        }
        else {                
                (*data->currentLocalMemory)[var.value] = (((*data->currentLocalMemory)[var.value]) >> nthp::fixedToInt(count.value));
        }

        return 0;
}


DEFINE_EXECUTION_BEHAVIOUR(ADD) {
        stdRef a = *(stdRef*)data->nodeSet[data->currentNode].access.data;
        stdRef b = *(stdRef*)(data->nodeSet[data->currentNode].access.data + sizeof(stdRef));
        indRef output = *(indRef*)(data->nodeSet[data->currentNode].access.data + sizeof(stdRef) + sizeof(stdRef));


        EVAL_STDREF(a);
        EVAL_STDREF(b);

        if(PR_METADATA_GET(output, nthp::script::flagBits::IS_GLOBAL)) {
                data->globalVarSet[output.value] = (a.value + b.value);
        } else {
                (*data->currentLocalMemory)[output.value] = (a.value + b.value);
        }

        return 0;
}

DEFINE_EXECUTION_BEHAVIOUR(SUB) {
        stdRef a = *(stdRef*)data->nodeSet[data->currentNode].access.data;
        stdRef b = *(stdRef*)(data->nodeSet[data->currentNode].access.data + sizeof(stdRef));
        indRef output = *(indRef*)(data->nodeSet[data->currentNode].access.data + sizeof(stdRef) + sizeof(stdRef));


        EVAL_STDREF(a);
        EVAL_STDREF(b);

        if(PR_METADATA_GET(output, nthp::script::flagBits::IS_GLOBAL)) {
                data->globalVarSet[output.value] = (a.value - b.value);
        } else {
                (*data->currentLocalMemory)[output.value] = (a.value - b.value);
        }
        return 0;
}

DEFINE_EXECUTION_BEHAVIOUR(MUL) {
        stdRef a = *(stdRef*)data->nodeSet[data->currentNode].access.data;
        stdRef b = *(stdRef*)(data->nodeSet[data->currentNode].access.data + sizeof(stdRef));
        indRef output = *(indRef*)(data->nodeSet[data->currentNode].access.data + sizeof(stdRef) + sizeof(stdRef));


        EVAL_STDREF(a);
        EVAL_STDREF(b);

        if(PR_METADATA_GET(output, nthp::script::flagBits::IS_GLOBAL)) {
                data->globalVarSet[output.value] = nthp::f_fixedProduct(a.value, b.value);
        } else {
                (*data->currentLocalMemory)[output.value] = nthp::f_fixedProduct(a.value, b.value);
        }
        return 0;
}

DEFINE_EXECUTION_BEHAVIOUR(DIV) {
        stdRef a = *(stdRef*)data->nodeSet[data->currentNode].access.data;
        stdRef b = *(stdRef*)(data->nodeSet[data->currentNode].access.data + sizeof(stdRef));
        indRef output = *(indRef*)(data->nodeSet[data->currentNode].access.data + sizeof(stdRef) + sizeof(stdRef));


        EVAL_STDREF(a);
        EVAL_STDREF(b);

        if(PR_METADATA_GET(output, nthp::script::flagBits::IS_GLOBAL)) {
                data->globalVarSet[output.value] = nthp::f_fixedQuotient(a.value, b.value);
        } else {
                (*data->currentLocalMemory)[output.value] = nthp::f_fixedQuotient(a.value, b.value);
        }
        return 0;
}

DEFINE_EXECUTION_BEHAVIOUR(SQRT) {
        stdRef base = *(stdRef*)(data->nodeSet[data->currentNode].access.data);
        indRef pointer = *(indRef*)(data->nodeSet[data->currentNode].access.data + sizeof(stdRef));

        EVAL_STDREF(base);

        if(PR_METADATA_GET(pointer, nthp::script::flagBits::IS_GLOBAL)) {
                data->globalVarSet[pointer.value] = nthp::f_sqrt(base.value);
        }
        else {
                (*data->currentLocalMemory)[pointer.value] = nthp::f_sqrt(base.value);
        }
       
        return 0;
}



DEFINE_EXECUTION_BEHAVIOUR(END) {
        return 0;
}

DEFINE_EXECUTION_BEHAVIOUR(LOGIC_EQU) {
        stdRef opA = *(stdRef*)(data->nodeSet[data->currentNode].access.data);
        stdRef opB = *(stdRef*)(data->nodeSet[data->currentNode].access.data + sizeof(stdRef));
        uint32_t endIndex = *(uint32_t*)(data->nodeSet[data->currentNode].access.data + sizeof(stdRef) + sizeof(stdRef));

        EVAL_STDREF(opA);
        EVAL_STDREF(opB);

        if(opA.value == opB.value)
                return 0;
        else 
                data->currentNode = endIndex;

        return 0;
}

DEFINE_EXECUTION_BEHAVIOUR(LOGIC_NOT) {
        stdRef opA = *(stdRef*)(data->nodeSet[data->currentNode].access.data);
        stdRef opB = *(stdRef*)(data->nodeSet[data->currentNode].access.data + sizeof(stdRef));
        uint32_t endIndex = *(uint32_t*)(data->nodeSet[data->currentNode].access.data + sizeof(stdRef) + sizeof(stdRef));

        EVAL_STDREF(opA);
        EVAL_STDREF(opB);
        
        if(opA.value != opB.value)
                return 0;
        else 
                data->currentNode = endIndex;

        return 0;
}

DEFINE_EXECUTION_BEHAVIOUR(LOGIC_GRT) {
        stdRef opA = *(stdRef*)(data->nodeSet[data->currentNode].access.data);
        stdRef opB = *(stdRef*)(data->nodeSet[data->currentNode].access.data + sizeof(stdRef));
        uint32_t endIndex = *(uint32_t*)(data->nodeSet[data->currentNode].access.data + sizeof(stdRef) + sizeof(stdRef));

        EVAL_STDREF(opA);
        EVAL_STDREF(opB);

        if(opA.value > opB.value)
                return 0;
        else 
                data->currentNode = endIndex;

        return 0;
}

DEFINE_EXECUTION_BEHAVIOUR(LOGIC_LST) {
        stdRef opA = *(stdRef*)(data->nodeSet[data->currentNode].access.data);
        stdRef opB = *(stdRef*)(data->nodeSet[data->currentNode].access.data + sizeof(stdRef));
        uint32_t endIndex = *(uint32_t*)(data->nodeSet[data->currentNode].access.data + sizeof(stdRef) + sizeof(stdRef));

        EVAL_STDREF(opA);
        EVAL_STDREF(opB);

        if(opA.value < opB.value)
                return 0;
        else 
                data->currentNode = endIndex;

        return 0;
}

DEFINE_EXECUTION_BEHAVIOUR(LOGIC_GRTE) {
        stdRef opA = *(stdRef*)(data->nodeSet[data->currentNode].access.data);
        stdRef opB = *(stdRef*)(data->nodeSet[data->currentNode].access.data + sizeof(stdRef));
        uint32_t endIndex = *(uint32_t*)(data->nodeSet[data->currentNode].access.data + sizeof(stdRef) + sizeof(stdRef));

        EVAL_STDREF(opA);
        EVAL_STDREF(opB);

        if(opA.value >= opB.value)
                return 0;
        else 
                data->currentNode = endIndex;

        return 0;
}


DEFINE_EXECUTION_BEHAVIOUR(LOGIC_LSTE) {
        stdRef opA = *(stdRef*)(data->nodeSet[data->currentNode].access.data);
        stdRef opB = *(stdRef*)(data->nodeSet[data->currentNode].access.data + sizeof(stdRef));
        uint32_t endIndex = *(uint32_t*)(data->nodeSet[data->currentNode].access.data + sizeof(stdRef) + sizeof(stdRef));

        EVAL_STDREF(opA);
        EVAL_STDREF(opB);

        if(opA.value <= opB.value)
                return 0;
        else 
                data->currentNode = endIndex;

        return 0;
}

DEFINE_EXECUTION_BEHAVIOUR(SET) {
        indRef pointer = *(indRef*)(data->nodeSet[data->currentNode].access.data);
        nthp::script::stdVarWidth value = *(nthp::script::stdVarWidth*)(data->nodeSet[data->currentNode].access.data + sizeof(indRef));

        if(PR_METADATA_GET(pointer, nthp::script::flagBits::IS_GLOBAL)) {
                data->globalVarSet[pointer.value] = value;
        }
        else {
                (*data->currentLocalMemory)[pointer.value] = value;
        }
       

        return 0;
}

DEFINE_EXECUTION_BEHAVIOUR(SET_BINARY) {
        indRef pointer = *(indRef*)(data->nodeSet[data->currentNode].access.data);
        nthp::script::stdVarWidth value = *(nthp::script::stdVarWidth*)(data->nodeSet[data->currentNode].access.data + sizeof(indRef));

        if(PR_METADATA_GET(pointer, nthp::script::flagBits::IS_GLOBAL)) {
                data->globalVarSet[pointer.value] = value;
        }
        else {
                (*data->currentLocalMemory)[pointer.value] = value;
        }
       

        return 0;
}

DEFINE_EXECUTION_BEHAVIOUR(CLEAR) {
        delete[] data->currentLocalMemory;
        data->varSetSize = 0;

        return 0;
}

DEFINE_EXECUTION_BEHAVIOUR(DEFINE) {
        uint32_t size = *(uint32_t*)(data->nodeSet[data->currentNode].access.data);
        (*data->currentLocalMemory) = new (std::nothrow) nthp::script::stdVarWidth[size];
        
        if((*data->currentLocalMemory) == nullptr) return 1;


        return 0;
}

DEFINE_EXECUTION_BEHAVIOUR(COPY) {
        indRef from = *(indRef*)(data->nodeSet[data->currentNode].access.data);
        indRef to = *(indRef*)(data->nodeSet[data->currentNode].access.data + sizeof(indRef));
        nthp::script::stdVarWidth* from_target;
        nthp::script::stdVarWidth* to_target;

        if(PR_METADATA_GET(from, nthp::script::flagBits::IS_GLOBAL)) {
                from_target = data->globalVarSet;
        }
        else {
                from_target = (*data->currentLocalMemory);
        }

        if(PR_METADATA_GET(to, nthp::script::flagBits::IS_GLOBAL)) {
                to_target = data->globalVarSet;
        }
        else {
                to_target = (*data->currentLocalMemory);
        }


        to_target[to.value] = from_target[from.value];
        return 0;
}

DEFINE_EXECUTION_BEHAVIOUR(TEXTURE_DEFINE) {
	stdRef size = *(stdRef*)(data->nodeSet[data->currentNode].access.data);
	
	EVAL_STDREF(size);
	
	data->textureBlock = new nthp::texture::gTexture[nthp::fixedToInt(size.value)];
	data->textureBlockSize = nthp::fixedToInt(size.value);

	return 0;
}

DEFINE_EXECUTION_BEHAVIOUR(TEXTURE_CLEAR) {
	if(data->textureBlockSize > 0) 
		delete[] data->textureBlock;	

	return 0;
}

DEFINE_EXECUTION_BEHAVIOUR(TEXTURE_LOAD) {
	stdRef output = *(stdRef*)(data->nodeSet[data->currentNode].access.data);
	const char* file = (data->nodeSet[data->currentNode].access.data + sizeof(stdRef));

        EVAL_STDREF(output);

	if(nthp::fixedToInt(output.value) > data->textureBlockSize) {
		PRINT_DEBUG_ERROR("Output index of LOAD_TEXTURE instuction out of bounds.\n");		
		return 1;
	}
	
	data->textureBlock[nthp::fixedToInt(output.value)].autoLoadTextureFile(file, &nthp::script::activePalette, nthp::core.getRenderer());

	return 0;
}

DEFINE_EXECUTION_BEHAVIOUR(SET_ACTIVE_PALETTE) {

        nthp::script::activePalette.importPaletteFromFile(data->nodeSet[data->currentNode].access.data);

        return 0;
}

DEFINE_EXECUTION_BEHAVIOUR(CORE_INIT) {
        stdRef px = *(stdRef*)(data->nodeSet[data->currentNode].access.data);
        stdRef py = *(stdRef*)(data->nodeSet[data->currentNode].access.data + (sizeof(stdRef) * 1));
        stdRef tx = *(stdRef*)(data->nodeSet[data->currentNode].access.data + (sizeof(stdRef) * 2));
        stdRef ty = *(stdRef*)(data->nodeSet[data->currentNode].access.data + (sizeof(stdRef) * 3));
        stdRef cx = *(stdRef*)(data->nodeSet[data->currentNode].access.data + (sizeof(stdRef) * 4));
        stdRef cy = *(stdRef*)(data->nodeSet[data->currentNode].access.data + (sizeof(stdRef) * 5));
        uint8_t flags = *(uint8_t*)(data->nodeSet[data->currentNode].access.data + (sizeof(stdRef) * 6));

        EVAL_STDREF(px);
        EVAL_STDREF(py);
        EVAL_STDREF(tx);
        EVAL_STDREF(ty);
        EVAL_STDREF(cx);
        EVAL_STDREF(cy);

        const char* title = (char*)(data->nodeSet[data->currentNode].access.data + (sizeof(stdRef) * 6) + sizeof(uint8_t));

        nthp::core.init(nthp::RenderRuleSet(nthp::fixedToInt(px.value), nthp::fixedToInt(py.value), tx.value, ty.value, nthp::vectFixed(cx.value, cy.value)), title, (flags >> NTHP_CORE_INIT_FULLSCREEN) & 1, (flags >> NTHP_CORE_INIT_SOFTWARE_RENDERING) & 1);


        return 0;
}

DEFINE_EXECUTION_BEHAVIOUR(FRAME_DEFINE) {
        stdRef size = *(stdRef*)(data->nodeSet[data->currentNode].access.data);
        
        EVAL_STDREF(size);
        data->frameBlock = new nthp::texture::Frame[nthp::fixedToInt(size.value)];
        data->frameBlockSize = nthp::fixedToInt(size.value);


        return 0;
}

DEFINE_EXECUTION_BEHAVIOUR(FRAME_CLEAR) {
        if(data->frameBlockSize > 0) {
                delete[] data->frameBlock;
                data->frameBlockSize = 0;
        }

        return 0;
}

DEFINE_EXECUTION_BEHAVIOUR(FRAME_SET) {
        stdRef frameIndex = *(stdRef*)(data->nodeSet[data->currentNode].access.data);
        stdRef x = *(stdRef*)(data->nodeSet[data->currentNode].access.data + sizeof(stdRef));
        stdRef y = *(stdRef*)(data->nodeSet[data->currentNode].access.data + (sizeof(stdRef) * 2));
        stdRef w = *(stdRef*)(data->nodeSet[data->currentNode].access.data + (sizeof(stdRef) * 3));
        stdRef h = *(stdRef*)(data->nodeSet[data->currentNode].access.data + (sizeof(stdRef) * 4));
        stdRef textureIndex = *(stdRef*)(data->nodeSet[data->currentNode].access.data + (sizeof(stdRef) * 5));

        EVAL_STDREF(frameIndex);
        EVAL_STDREF(x);
        EVAL_STDREF(y);
        EVAL_STDREF(w);
        EVAL_STDREF(h);
        EVAL_STDREF(textureIndex);

        SDL_Rect rect;
        rect.x = nthp::fixedToInt(x.value);
        rect.y = nthp::fixedToInt(y.value);
        rect.w = nthp::fixedToInt(w.value);
        rect.h = nthp::fixedToInt(h.value);
        
        data->frameBlock[nthp::fixedToInt(frameIndex.value)].src = rect;
        data->frameBlock[nthp::fixedToInt(frameIndex.value)].texture = data->textureBlock[nthp::fixedToInt(textureIndex.value)].getTextureData().texture;

        return 0;
}



DEFINE_EXECUTION_BEHAVIOUR(GETGPR) {
        indRef into = *(indRef*)(data->nodeSet[data->currentNode].access.data);


        if(PR_METADATA_GET(into, nthp::script::flagBits::IS_GLOBAL)) {
                data->globalVarSet[into.value] = nthp::intToFixed(data->currentTriggerConfig.GPR);
        }
        else {
                (*data->currentLocalMemory)[into.value] = nthp::intToFixed(data->currentTriggerConfig.GPR);
        }

        return 0;
}

DEFINE_EXECUTION_BEHAVIOUR(SM_WRITE) {
        stdRef to = *(stdRef*)(data->nodeSet[data->currentNode].access.data);
        stdRef from = *(stdRef*)(data->nodeSet[data->currentNode].access.data + sizeof(stdRef));

        EVAL_STDREF(to);
        EVAL_STDREF(from);

        nthp::script::stageMemory[nthp::fixedToInt(to.value)] = (uint8_t)nthp::fixedToInt(from.value);

        return 0;
}

DEFINE_EXECUTION_BEHAVIOUR(SM_READ) {
        stdRef location = *(stdRef*)(data->nodeSet[data->currentNode].access.data);
        indRef output = *(indRef*)(data->nodeSet[data->currentNode].access.data + sizeof(indRef));

        EVAL_STDREF(location);

        if(PR_METADATA_GET(output, nthp::script::flagBits::IS_GLOBAL)) {
                data->globalVarSet[output.value] = nthp::intToFixed(nthp::script::stageMemory[nthp::fixedToInt(location.value)]);
        }
        else {
                (*data->currentLocalMemory)[output.value] = nthp::intToFixed(nthp::script::stageMemory[nthp::fixedToInt(location.value)]);
        }

        return 0;
}

DEFINE_EXECUTION_BEHAVIOUR(ENT_DEFINE) {
        stdRef size = *(stdRef*)(data->nodeSet[data->currentNode].access.data);

        EVAL_STDREF(size);
        if(data->entityBlockSize > 0) delete[] data->entityBlock;

        data->entityBlock = new nthp::entity::gEntity[nthp::fixedToInt(size.value)];
        data->entityBlockSize = nthp::fixedToInt(size.value);


        return 0;
}

DEFINE_EXECUTION_BEHAVIOUR(ENT_CLEAR) {
        delete[] data->entityBlock;
        data->entityBlockSize = 0;

        return 0;
}

DEFINE_EXECUTION_BEHAVIOUR(ENT_SETCURRENTFRAME) {
        stdRef target = *(stdRef*)(data->nodeSet[data->currentNode].access.data);
        stdRef frame = *(stdRef*)(data->nodeSet[data->currentNode].access.data + sizeof(stdRef));

        EVAL_STDREF(target);
        EVAL_STDREF(frame);

        data->entityBlock[nthp::fixedToInt(target.value)].setCurrentFrame(nthp::fixedToInt(frame.value));
        return 0;
}

DEFINE_EXECUTION_BEHAVIOUR(ENT_SETPOS) {
        stdRef target = *(stdRef*)(data->nodeSet[data->currentNode].access.data);
        stdRef x = *(stdRef*)(data->nodeSet[data->currentNode].access.data + sizeof(stdRef));
        stdRef y = *(stdRef*)(data->nodeSet[data->currentNode].access.data + sizeof(stdRef) + sizeof(stdRef));

        EVAL_STDREF(target);
        EVAL_STDREF(x);
        EVAL_STDREF(y);

        data->entityBlock[nthp::fixedToInt(target.value)].setPosition(nthp::vectFixed(x.value, y.value));
        return 0;
}

DEFINE_EXECUTION_BEHAVIOUR(ENT_MOVE) {
        stdRef target = *(stdRef*)(data->nodeSet[data->currentNode].access.data);
        stdRef x = *(stdRef*)(data->nodeSet[data->currentNode].access.data + sizeof(stdRef));
        stdRef y = *(stdRef*)(data->nodeSet[data->currentNode].access.data + sizeof(stdRef) + sizeof(stdRef));

        EVAL_STDREF(target);
        EVAL_STDREF(x);
        EVAL_STDREF(y);

        data->entityBlock[nthp::fixedToInt(target.value)].move(nthp::vectFixed(x.value, y.value));
        return 0;
}


DEFINE_EXECUTION_BEHAVIOUR(ENT_SETFRAMERANGE) {
        stdRef target = *(stdRef*)(data->nodeSet[data->currentNode].access.data);
        stdRef start = *(stdRef*)(data->nodeSet[data->currentNode].access.data + sizeof(stdRef));
        stdRef size = *(stdRef*)(data->nodeSet[data->currentNode].access.data + sizeof(stdRef) + sizeof(stdRef));

        EVAL_STDREF(target);
        EVAL_STDREF(start);
        EVAL_STDREF(size);

        data->entityBlock[nthp::fixedToInt(target.value)].importFrameData(data->frameBlock + nthp::fixedToInt(start.value), nthp::fixedToInt(size.value), false);
        return 0;
}

DEFINE_EXECUTION_BEHAVIOUR(ENT_SETHITBOXSIZE) {
        stdRef target = *(stdRef*)(data->nodeSet[data->currentNode].access.data);
        stdRef x = *(stdRef*)(data->nodeSet[data->currentNode].access.data + sizeof(stdRef));
        stdRef y = *(stdRef*)(data->nodeSet[data->currentNode].access.data + sizeof(stdRef) + sizeof(stdRef));

        EVAL_STDREF(target);
        EVAL_STDREF(x);
        EVAL_STDREF(y);

        data->entityBlock[nthp::fixedToInt(target.value)].setHtiboxSize(nthp::vectFixed(x.value, y.value));
        return 0;
}


DEFINE_EXECUTION_BEHAVIOUR(ENT_SETHITBOXOFFSET) {
        stdRef target = *(stdRef*)(data->nodeSet[data->currentNode].access.data);
        stdRef x = *(stdRef*)(data->nodeSet[data->currentNode].access.data + sizeof(stdRef));
        stdRef y = *(stdRef*)(data->nodeSet[data->currentNode].access.data + sizeof(stdRef) + sizeof(stdRef));

        EVAL_STDREF(target);
        EVAL_STDREF(x);
        EVAL_STDREF(y);

        data->entityBlock[nthp::fixedToInt(target.value)].setHitboxOffset(nthp::vectFixed(x.value, y.value));
        return 0;
}

DEFINE_EXECUTION_BEHAVIOUR(ENT_SETRENDERSIZE) {
        stdRef target = *(stdRef*)(data->nodeSet[data->currentNode].access.data);
        stdRef x = *(stdRef*)(data->nodeSet[data->currentNode].access.data + sizeof(stdRef));
        stdRef y = *(stdRef*)(data->nodeSet[data->currentNode].access.data + sizeof(stdRef) + sizeof(stdRef));

        EVAL_STDREF(target);
        EVAL_STDREF(x);
        EVAL_STDREF(y);

        data->entityBlock[nthp::fixedToInt(target.value)].setRenderSize(nthp::vectFixed(x.value, y.value));
        return 0;
}

DEFINE_EXECUTION_BEHAVIOUR(CORE_QRENDER) {
        stdRef entity = *(stdRef*)(data->nodeSet[data->currentNode].access.data);

        EVAL_STDREF(entity);

        nthp::core.render(data->entityBlock[nthp::fixedToInt(entity.value)].getUpdateRenderPacket(&nthp::core.p_coreDisplay));
        return 0;
}


DEFINE_EXECUTION_BEHAVIOUR(CORE_ABS_QRENDER) {
        stdRef entity = *(stdRef*)(data->nodeSet[data->currentNode].access.data);

        EVAL_STDREF(entity);

        nthp::core.render(data->entityBlock[nthp::fixedToInt(entity.value)].abs_getRenderPacket(&nthp::core.p_coreDisplay));
        return 0;
}

DEFINE_EXECUTION_BEHAVIOUR(CORE_CLEAR) {
        nthp::core.clear();
        return 0;
}

DEFINE_EXECUTION_BEHAVIOUR(CORE_DISPLAY) {
        nthp::core.display();
        return 0;
}

DEFINE_EXECUTION_BEHAVIOUR(CORE_SETMAXFPS) {
        stdRef fps = *(stdRef*)(data->nodeSet[data->currentNode].access.data);

        EVAL_STDREF(fps);

        nthp::setMaxFPS(nthp::fixedToInt(fps.value));
        return 0;
}

DEFINE_EXECUTION_BEHAVIOUR(CORE_SETWINDOWRES) {
        stdRef x = *(stdRef*)(data->nodeSet[data->currentNode].access.data);
        stdRef y = *(stdRef*)(data->nodeSet[data->currentNode].access.data + sizeof(stdRef));

        EVAL_STDREF(x);
        EVAL_STDREF(y);

        nthp::core.setWindowRenderSize(nthp::fixedToInt(x.value), nthp::fixedToInt(y.value));
        return 0;
}

DEFINE_EXECUTION_BEHAVIOUR(CORE_SETCAMERARES) {
        stdRef x = *(stdRef*)(data->nodeSet[data->currentNode].access.data);
        stdRef y = *(stdRef*)(data->nodeSet[data->currentNode].access.data + sizeof(stdRef));

        EVAL_STDREF(x);
        EVAL_STDREF(y);

        nthp::core.setVirtualRenderScale(x.value, y.value);
        return 0;
}

DEFINE_EXECUTION_BEHAVIOUR(CORE_SETCAMERAPOSITION) {
        stdRef x = *(stdRef*)(data->nodeSet[data->currentNode].access.data);
        stdRef y = *(stdRef*)(data->nodeSet[data->currentNode].access.data + sizeof(stdRef));

        EVAL_STDREF(x);
        EVAL_STDREF(y);

        nthp::core.p_coreDisplay.cameraWorldPosition.x = x.value;
        nthp::core.p_coreDisplay.cameraWorldPosition.y = y.value;
        return 0;
}

DEFINE_EXECUTION_BEHAVIOUR(CORE_MOVECAMERA) {
        stdRef x = *(stdRef*)(data->nodeSet[data->currentNode].access.data);
        stdRef y = *(stdRef*)(data->nodeSet[data->currentNode].access.data + sizeof(stdRef));

        EVAL_STDREF(x);
        EVAL_STDREF(y);

        nthp::core.p_coreDisplay.cameraWorldPosition += nthp::worldPosition(nthp::f_fixedProduct(x.value, nthp::deltaTime), nthp::f_fixedProduct(y.value, nthp::deltaTime));
        return 0;
}


DEFINE_EXECUTION_BEHAVIOUR(ACTION_DEFINE) {
        stdRef size = *(stdRef*)(data->nodeSet[data->currentNode].access.data);

        EVAL_STDREF(size);

        data->actionList = new nthp::script::Script::Action[nthp::fixedToInt(size.value)];
        data->actionListSize = nthp::fixedToInt(size.value);
        return 0;
}

DEFINE_EXECUTION_BEHAVIOUR(ACTION_BIND) {
        stdRef output = *(stdRef*)(data->nodeSet[data->currentNode].access.data);
        indRef var = *(indRef*)(data->nodeSet[data->currentNode].access.data + sizeof(stdRef));
        int key = *(int32_t*)(data->nodeSet[data->currentNode].access.data + sizeof(stdRef) + sizeof(indRef));

        EVAL_STDREF(output);

        data->actionList[nthp::fixedToInt(output.value)].varIndex = var;  
        data->actionList[nthp::fixedToInt(output.value)].boundKey = key;      

        return 0;
}

DEFINE_EXECUTION_BEHAVIOUR(ACTION_CLEAR) {
        delete[] data->actionList;
        data->actionListSize = 0;

        return 0;
}



// Genius design; Automatically updates ID indecies and places functions accordingly. Just add/change stuff in 's_instructions.hpp'.
// the 'nthp::script::instructions::ID' will correspond with the index of the desired instruction in this array.
static const int (*exec_func[nthp::script::instructions::ID::numberOfInstructions])(nthp::script::Script::ScriptDataSet* data) { INSTRUCTION_TOKENS() };






/////////////////////////////////////////////////////
/////////////////////////////////////////////////////


nthp::script::Script::Script() {
        data = NULL;
        inStageContext = false;
        localCurrentNode = 0;
        localLabelBlock = nullptr;
        localLabelBlockSize = 0;
        localVarSet = nullptr;
        localVarSetSize = 0;
}

nthp::script::Script::Script(const char* filename, ScriptDataSet* dataSet) {
        localCurrentNode = 0;
        localLabelBlock = nullptr;
        localLabelBlockSize = 0;
        localVarSet = nullptr;
        localVarSetSize = 0;



        this->import(filename, dataSet);
}


int nthp::script::Script::import(const char* filename, ScriptDataSet* dataSet) {
        if(dataSet == NULL) {
                dataSet = new nthp::script::Script::ScriptDataSet;
                memset(dataSet, 0, sizeof(nthp::script::Script::ScriptDataSet));
                inStageContext = false;
        }
        else {
                inStageContext = true;
        }

        std::fstream file(filename, std::ios::in | std::ios::binary);
        data = dataSet;

        if(file.fail()) {
                PRINT_DEBUG_ERROR("Unable to load Script file [%s]; File not found.\n");
                
                return 1;
        }


        nthp::script::Node read;
        
        {
                NOVERB_PRINT_DEBUG("Importing Script [%s] ::..\n", filename);
                for(size_t i = 0; read.access.ID != GET_INSTRUCTION_ID(EXIT); ++i) {

                        file.read((char*)&read, sizeof(nthp::script::Node::n_file_t));
                        nodeSet.push_back(read);
                        NOVERB_PRINT_DEBUG("\t[%zu] ID:%u Size:%u\n", i, read.access.ID, read.access.size);
                        
                        if(nodeSet.back().access.size != 0) {
                                nodeSet.back().access.data = (char*)malloc(nodeSet.back().access.size);
                                file.read(nodeSet.back().access.data, nodeSet.back().access.size);
                        }

                        else nodeSet.back().access.data = nullptr;

                }
        }

        localVarSetSize = *(uint32_t*)(nodeSet[0].access.data);
        data->globalMemBudget += *(uint32_t*)(nodeSet[0].access.data + sizeof(uint32_t)); // Unused unless in stage context.

        // Note this is a pointer to the region that stores labels.
        localLabelBlock = (uint32_t*)(nodeSet[0].access.data + (sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t)));
        localLabelBlockSize = *(uint32_t*)(nodeSet[0].access.data + (sizeof(uint32_t) + sizeof(uint32_t)));

        localVarSet = (decltype(localVarSet))malloc(sizeof(nthp::fixed_t) * localVarSetSize);;

        memset(localVarSet, 0, sizeof(nthp::fixed_t) * localVarSetSize);
        

        return 0;
}




int nthp::script::Script::execute() {
        data->isSuspended = false;

        data->currentLocalMemory = &localVarSet;
        data->currentLabelBlock = localLabelBlock;
        data->currentLabelBlockSize = localLabelBlockSize;
        data->currentNode = localCurrentNode;
        data->nodeSet = nodeSet.data();

        // Could leave this to rot, but might be important later.
        if(!inStageContext) {
                data->globalVarSet = localVarSet;
        }

        while(data->currentNode < nodeSet.size() && data->isSuspended == false) {
                if(exec_func[nodeSet[data->currentNode].access.ID](data)) return 1;

                ++data->currentNode;
        }
        if(data->currentNode == nodeSet.size()) data->currentNode = 0;

        localCurrentNode = data->currentNode;

        return 0;
}


int nthp::script::Script::execute(uint32_t entryPoint) {
        localCurrentNode = entryPoint;

        return execute();
}




nthp::script::Script::~Script() {
        if(localVarSetSize > 0)
                delete[] localVarSet;
        
        if(!inStageContext) {
                delete data;
        }


        for(size_t i = 0; i < nodeSet.size(); ++i) {
                if(nodeSet[i].access.size > 0) free(nodeSet[i].access.data);
        }
}
