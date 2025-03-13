
#include "s_script.hpp"
using namespace nthp::script::instructions;
#define DEFINE_EXECUTION_BEHAVIOUR(instruction) const int instruction (nthp::script::Script::ScriptDataSet* data)

char nthp::script::stageMemory[STAGEMEM_MAX];
nthp::texture::Palette nthp::script::activePalette;

#define EVAL_STDREF(ref)        do {\
                                if(PR_METADATA_GET(ref, nthp::script::flagBits::IS_REFERENCE)) {\
                                        if(PR_METADATA_GET(ref, nthp::script::flagBits::IS_GLOBAL)) {\
                                                if(PR_METADATA_GET(ref, nthp::script::flagBits::IS_PTR)) {\
                                                        ref.value = data->globalVarSet[nthp::fixedToInt(data->globalVarSet[nthp::fixedToInt(ref.value)])];\
                                                        break;\
                                                }\
                                                ref.value = data->globalVarSet[nthp::fixedToInt(ref.value)];\
                                        }\
                                        else {\
                                                if(PR_METADATA_GET(ref, nthp::script::flagBits::IS_PTR)) {\
                                                        ref.value = (*data->currentLocalMemory)[nthp::fixedToInt((*data->currentLocalMemory)[nthp::fixedToInt(ref.value)])];\
                                                        break;\
                                                }\
                                                ref.value = (*data->currentLocalMemory)[nthp::fixedToInt(ref.value)];\
                                        }\
                                        if(PR_METADATA_GET(ref, nthp::script::flagBits::IS_NEGATED)) ref.value = -(ref.value);\
                                } }\
                                while(0);



#ifdef DEBUG
        nthp::vectGeneric nthp::script::debug::debugInstructionCall = nthp::vectGeneric(-1, -1);
        bool nthp::script::debug::suspendExecution = false;
#endif



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

        for(uint32_t i = 0; i < data->currentLabelBlockSize; ++i) {
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



        data->currentNode = nthp::fixedToInt(index.value) - 1;

        return 0;
}

DEFINE_EXECUTION_BEHAVIOUR(GETINDEX) {
        ptrRef var = *(ptrRef*)data->nodeSet[data->currentNode].access.data;
        EVAL_STDREF(var);

        if(PR_METADATA_GET(var, nthp::script::flagBits::IS_GLOBAL)) {
                data->globalVarSet[nthp::fixedToInt(var.value)] = nthp::intToFixed(data->currentNode);
        }
        else {
                (*data->currentLocalMemory)[nthp::fixedToInt(var.value)] = nthp::intToFixed(data->currentNode);
        }

        return 0;
}

DEFINE_EXECUTION_BEHAVIOUR(INC) {
        ptrRef var = *(ptrRef*)data->nodeSet[data->currentNode].access.data;
        EVAL_STDREF(var);

        if(PR_METADATA_GET(var, nthp::script::flagBits::IS_GLOBAL)) {
                data->globalVarSet[nthp::fixedToInt(var.value)] += nthp::intToFixed(1);
        }
        else {
                (*data->currentLocalMemory)[nthp::fixedToInt(var.value)] += nthp::intToFixed(1);
        }

        return 0;
}

DEFINE_EXECUTION_BEHAVIOUR(DEC) {
        ptrRef var = *(ptrRef*)data->nodeSet[data->currentNode].access.data;
        EVAL_STDREF(var);

        if(PR_METADATA_GET(var, nthp::script::flagBits::IS_GLOBAL)) {
                data->globalVarSet[nthp::fixedToInt(var.value)] -= nthp::intToFixed(1);
        }
        else {
                (*data->currentLocalMemory)[nthp::fixedToInt(var.value)] -= nthp::intToFixed(1);
        }

        return 0;
}

DEFINE_EXECUTION_BEHAVIOUR(LSHIFT) {
        ptrRef var =  *(ptrRef*)data->nodeSet[data->currentNode].access.data;
        stdRef count = *(stdRef*)(data->nodeSet[data->currentNode].access.data + sizeof(ptrRef));

        EVAL_STDREF(var);
        EVAL_STDREF(count);
        
        if(PR_METADATA_GET(var, nthp::script::flagBits::IS_GLOBAL)) {
                data->globalVarSet[nthp::fixedToInt(var.value)] = ((data->globalVarSet[var.value]) << nthp::fixedToInt(count.value));
        }
        else {                
                (*data->currentLocalMemory)[nthp::fixedToInt(var.value)] = (((*data->currentLocalMemory)[var.value]) << nthp::fixedToInt(count.value));
        }

        return 0;
}

DEFINE_EXECUTION_BEHAVIOUR(RSHIFT) {
        ptrRef var =  *(ptrRef*)data->nodeSet[data->currentNode].access.data;
        stdRef count = *(stdRef*)(data->nodeSet[data->currentNode].access.data + sizeof(ptrRef));
        
        
        EVAL_STDREF(var);
        EVAL_STDREF(count);

        
        if(PR_METADATA_GET(var, nthp::script::flagBits::IS_GLOBAL)) {
                data->globalVarSet[nthp::fixedToInt(var.value)] = ((data->globalVarSet[var.value]) >> nthp::fixedToInt(count.value));
        }
        else {                
                (*data->currentLocalMemory)[nthp::fixedToInt(var.value)] = (((*data->currentLocalMemory)[var.value]) >> nthp::fixedToInt(count.value));
        }

        return 0;
}


DEFINE_EXECUTION_BEHAVIOUR(ADD) {
        stdRef a = *(stdRef*)data->nodeSet[data->currentNode].access.data;
        stdRef b = *(stdRef*)(data->nodeSet[data->currentNode].access.data + sizeof(stdRef));
        ptrRef output = *(ptrRef*)(data->nodeSet[data->currentNode].access.data + sizeof(stdRef) + sizeof(stdRef));


        EVAL_STDREF(a);
        EVAL_STDREF(b);
        EVAL_STDREF(output);

        if(PR_METADATA_GET(output, nthp::script::flagBits::IS_GLOBAL)) {
                data->globalVarSet[nthp::fixedToInt(output.value)] = (a.value + b.value);
        } else {
                (*data->currentLocalMemory)[nthp::fixedToInt(output.value)] = (a.value + b.value);
        }

        return 0;
}

DEFINE_EXECUTION_BEHAVIOUR(SUB) {
        stdRef a = *(stdRef*)data->nodeSet[data->currentNode].access.data;
        stdRef b = *(stdRef*)(data->nodeSet[data->currentNode].access.data + sizeof(stdRef));
        ptrRef output = *(ptrRef*)(data->nodeSet[data->currentNode].access.data + sizeof(stdRef) + sizeof(stdRef));


        EVAL_STDREF(a);
        EVAL_STDREF(b);
        EVAL_STDREF(output)

        if(PR_METADATA_GET(output, nthp::script::flagBits::IS_GLOBAL)) {
                data->globalVarSet[nthp::fixedToInt(output.value)] = (a.value - b.value);
        } else {
                (*data->currentLocalMemory)[nthp::fixedToInt(output.value)] = (a.value - b.value);
        }
        return 0;
}

DEFINE_EXECUTION_BEHAVIOUR(MUL) {
        stdRef a = *(stdRef*)data->nodeSet[data->currentNode].access.data;
        stdRef b = *(stdRef*)(data->nodeSet[data->currentNode].access.data + sizeof(stdRef));
        ptrRef output = *(ptrRef*)(data->nodeSet[data->currentNode].access.data + sizeof(stdRef) + sizeof(stdRef));


        EVAL_STDREF(a);
        EVAL_STDREF(b);
        EVAL_STDREF(output);

        if(PR_METADATA_GET(output, nthp::script::flagBits::IS_GLOBAL)) {
                data->globalVarSet[nthp::fixedToInt(output.value)] = nthp::f_fixedProduct(a.value, b.value);
        } else {
                (*data->currentLocalMemory)[nthp::fixedToInt(output.value)] = nthp::f_fixedProduct(a.value, b.value);
        }
        return 0;
}

DEFINE_EXECUTION_BEHAVIOUR(DIV) {
        stdRef a = *(stdRef*)data->nodeSet[data->currentNode].access.data;
        stdRef b = *(stdRef*)(data->nodeSet[data->currentNode].access.data + sizeof(stdRef));
        ptrRef output = *(ptrRef*)(data->nodeSet[data->currentNode].access.data + sizeof(stdRef) + sizeof(stdRef));


        EVAL_STDREF(a);
        EVAL_STDREF(b);
        EVAL_STDREF(output);

        if(PR_METADATA_GET(output, nthp::script::flagBits::IS_GLOBAL)) {
                data->globalVarSet[nthp::fixedToInt(output.value)] = nthp::f_fixedQuotient(a.value, b.value);
        } else {
                (*data->currentLocalMemory)[nthp::fixedToInt(output.value)] = nthp::f_fixedQuotient(a.value, b.value);
        }
        return 0;
}

DEFINE_EXECUTION_BEHAVIOUR(SQRT) {
        stdRef base = *(stdRef*)(data->nodeSet[data->currentNode].access.data);
        ptrRef output = *(ptrRef*)(data->nodeSet[data->currentNode].access.data + sizeof(stdRef));

        EVAL_STDREF(base);
        EVAL_STDREF(output);

        if(PR_METADATA_GET(output, nthp::script::flagBits::IS_GLOBAL)) {
                data->globalVarSet[nthp::fixedToInt(output.value)] = nthp::f_sqrt(base.value);
        }
        else {
                (*data->currentLocalMemory)[nthp::fixedToInt(output.value)] = nthp::f_sqrt(base.value);
        }
       
        return 0;
}



DEFINE_EXECUTION_BEHAVIOUR(END) {
        return 0;
}

DEFINE_EXECUTION_BEHAVIOUR(ELSE) {
        uint32_t endLocation = *(uint32_t*)(data->nodeSet[data->currentNode].access.data);

        data->currentNode = endLocation;
        return 0;
}

DEFINE_EXECUTION_BEHAVIOUR(SKIP) {
        uint32_t skip_to = *(uint32_t*)(data->nodeSet[data->currentNode].access.data);

        data->currentNode = skip_to;
        return 0;
}

DEFINE_EXECUTION_BEHAVIOUR(SKIP_END) {
        return 0;
}

DEFINE_EXECUTION_BEHAVIOUR(LOGIC_IF_TRUE) {
        stdRef opA = *(stdRef*)(data->nodeSet[data->currentNode].access.data);
        uint32_t endIndex = *(uint32_t*)(data->nodeSet[data->currentNode].access.data + sizeof(stdRef));
        uint32_t elseIndex = *(uint32_t*)(data->nodeSet[data->currentNode].access.data + sizeof(stdRef) + sizeof(uint32_t));


        EVAL_STDREF(opA);
        if(nthp::fixedToInt(opA.value)) return 0;
        if(elseIndex) { data->currentNode = elseIndex; return 0; }
        
        data->currentNode = endIndex;

        return 0;
}


DEFINE_EXECUTION_BEHAVIOUR(LOGIC_EQU) {
        stdRef opA = *(stdRef*)(data->nodeSet[data->currentNode].access.data);
        stdRef opB = *(stdRef*)(data->nodeSet[data->currentNode].access.data + sizeof(stdRef));
        uint32_t endIndex = *(uint32_t*)(data->nodeSet[data->currentNode].access.data + sizeof(stdRef) + sizeof(stdRef));
        uint32_t elseIndex = *(uint32_t*)(data->nodeSet[data->currentNode].access.data + (sizeof(stdRef) + sizeof(stdRef) + sizeof(uint32_t)));

        EVAL_STDREF(opA);
        EVAL_STDREF(opB);

        if(opA.value == opB.value) return 0;
        if(elseIndex) { data->currentNode = elseIndex; return 0; }

        data->currentNode = endIndex;

        return 0;
}

DEFINE_EXECUTION_BEHAVIOUR(LOGIC_NOT) {
        stdRef opA = *(stdRef*)(data->nodeSet[data->currentNode].access.data);
        stdRef opB = *(stdRef*)(data->nodeSet[data->currentNode].access.data + sizeof(stdRef));
        uint32_t endIndex = *(uint32_t*)(data->nodeSet[data->currentNode].access.data + sizeof(stdRef) + sizeof(stdRef));
        uint32_t elseIndex = *(uint32_t*)(data->nodeSet[data->currentNode].access.data + (sizeof(stdRef) + sizeof(stdRef) + sizeof(uint32_t)));

        EVAL_STDREF(opA);
        EVAL_STDREF(opB);

        if(opA.value != opB.value) return 0;
        if(elseIndex) { data->currentNode = elseIndex; return 0; }

        data->currentNode = endIndex;

        return 0;
}

DEFINE_EXECUTION_BEHAVIOUR(LOGIC_GRT) {
        stdRef opA = *(stdRef*)(data->nodeSet[data->currentNode].access.data);
        stdRef opB = *(stdRef*)(data->nodeSet[data->currentNode].access.data + sizeof(stdRef));
        uint32_t endIndex = *(uint32_t*)(data->nodeSet[data->currentNode].access.data + sizeof(stdRef) + sizeof(stdRef));
        uint32_t elseIndex = *(uint32_t*)(data->nodeSet[data->currentNode].access.data + (sizeof(stdRef) + sizeof(stdRef) + sizeof(uint32_t)));

        EVAL_STDREF(opA);
        EVAL_STDREF(opB);

        if(opA.value > opB.value) return 0;
        if(elseIndex) { data->currentNode = elseIndex; return 0; }

        data->currentNode = endIndex;

        return 0;
}

DEFINE_EXECUTION_BEHAVIOUR(LOGIC_LST) {
        stdRef opA = *(stdRef*)(data->nodeSet[data->currentNode].access.data);
        stdRef opB = *(stdRef*)(data->nodeSet[data->currentNode].access.data + sizeof(stdRef));
        uint32_t endIndex = *(uint32_t*)(data->nodeSet[data->currentNode].access.data + sizeof(stdRef) + sizeof(stdRef));
        uint32_t elseIndex = *(uint32_t*)(data->nodeSet[data->currentNode].access.data + (sizeof(stdRef) + sizeof(stdRef) + sizeof(uint32_t)));

        EVAL_STDREF(opA);
        EVAL_STDREF(opB);

        if(opA.value < opB.value) return 0;
        if(elseIndex) { data->currentNode = elseIndex; return 0; }

        data->currentNode = endIndex;

        return 0;
}

DEFINE_EXECUTION_BEHAVIOUR(LOGIC_GRTE) {
        stdRef opA = *(stdRef*)(data->nodeSet[data->currentNode].access.data);
        stdRef opB = *(stdRef*)(data->nodeSet[data->currentNode].access.data + sizeof(stdRef));
        uint32_t endIndex = *(uint32_t*)(data->nodeSet[data->currentNode].access.data + sizeof(stdRef) + sizeof(stdRef));
        uint32_t elseIndex = *(uint32_t*)(data->nodeSet[data->currentNode].access.data + (sizeof(stdRef) + sizeof(stdRef) + sizeof(uint32_t)));

        EVAL_STDREF(opA);
        EVAL_STDREF(opB);

        if(opA.value >= opB.value) return 0;
        if(elseIndex) { data->currentNode = elseIndex; return 0; }

        data->currentNode = endIndex;

        return 0;
}


DEFINE_EXECUTION_BEHAVIOUR(LOGIC_LSTE) {
        stdRef opA = *(stdRef*)(data->nodeSet[data->currentNode].access.data);
        stdRef opB = *(stdRef*)(data->nodeSet[data->currentNode].access.data + sizeof(stdRef));
        uint32_t endIndex = *(uint32_t*)(data->nodeSet[data->currentNode].access.data + sizeof(stdRef) + sizeof(stdRef));
        uint32_t elseIndex = *(uint32_t*)(data->nodeSet[data->currentNode].access.data + (sizeof(stdRef) + sizeof(stdRef) + sizeof(uint32_t)));

        EVAL_STDREF(opA);
        EVAL_STDREF(opB);

        if(opA.value <= opB.value) return 0;
        if(elseIndex) { data->currentNode = elseIndex; return 0; }

        data->currentNode = endIndex;

        return 0;
}

DEFINE_EXECUTION_BEHAVIOUR(SET) {
        ptrRef pointer = *(ptrRef*)(data->nodeSet[data->currentNode].access.data);
        nthp::script::stdVarWidth value = *(nthp::script::stdVarWidth*)(data->nodeSet[data->currentNode].access.data + sizeof(ptrRef));

        EVAL_STDREF(pointer);
        printf("ref.value = %lf\n", nthp::fixedToDouble(pointer.value));

        if(PR_METADATA_GET(pointer, nthp::script::flagBits::IS_GLOBAL)) {
                data->globalVarSet[nthp::fixedToInt(pointer.value)] = value;
        }
        else {
                (*data->currentLocalMemory)[nthp::fixedToInt(pointer.value)] = value;
        }
       
        return 0;
}


DEFINE_EXECUTION_BEHAVIOUR(CLEAR) {
        free((*data->currentLocalMemory));
        data->varSetSize = 0;

        return 0;
}

DEFINE_EXECUTION_BEHAVIOUR(DEFINE) {
        stdRef size = *(stdRef*)(data->nodeSet[data->currentNode].access.data);
        void* varData = (*data->currentLocalMemory);
        
        EVAL_STDREF(size);
        

        if((data->varSetSize > 0)) {
                auto x = (nthp::script::stdVarWidth*)realloc(varData, nthp::fixedToInt(size.value));
                if(x == NULL) {
                        PRINT_DEBUG_ERROR("Unable to allocate local memory at [%zu]; closing..\n", data->currentNode);
                        return 1;
                }
                
                (*data->currentLocalMemory) = x;
                data->varSetSize = nthp::fixedToInt(size.value);

                return 0;
        }

        (*data->currentLocalMemory) = (nthp::script::stdVarWidth*)malloc(nthp::fixedToInt(size.value));
        data->varSetSize = nthp::fixedToInt(size.value);

        return 0;
}

DEFINE_EXECUTION_BEHAVIOUR(COPY) {
        ptrRef from = *(ptrRef*)(data->nodeSet[data->currentNode].access.data);
        ptrRef to = *(ptrRef*)(data->nodeSet[data->currentNode].access.data + sizeof(ptrRef));
        nthp::script::stdVarWidth* from_target;
        nthp::script::stdVarWidth* to_target;

        EVAL_STDREF(from);
        EVAL_STDREF(to);


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


        to_target[nthp::fixedToInt(to.value)] = from_target[nthp::fixedToInt(to.value)];
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
		PRINT_DEBUG_ERROR("Output index of TEXTURE_LOAD instuction out of bounds.\n");		
		return 1;
	}
	
	data->textureBlock[nthp::fixedToInt(output.value)].autoLoadTextureFile(file, &nthp::script::activePalette, nthp::core.getRenderer());

	return 0;
}

DEFINE_EXECUTION_BEHAVIOUR(SET_ACTIVE_PALETTE) {

        nthp::script::activePalette.importPaletteFromFile(data->nodeSet[data->currentNode].access.data);

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
        ptrRef into = *(ptrRef*)(data->nodeSet[data->currentNode].access.data);
        EVAL_STDREF(into);

        if(PR_METADATA_GET(into, nthp::script::flagBits::IS_GLOBAL)) {
                data->globalVarSet[nthp::fixedToInt(into.value)] = nthp::intToFixed(data->currentTriggerConfig.GPR);
        }
        else {
                (*data->currentLocalMemory)[nthp::fixedToInt(into.value)] = nthp::intToFixed(data->currentTriggerConfig.GPR);
        }

        return 0;
}

DEFINE_EXECUTION_BEHAVIOUR(SM_WRITE) {
        stdRef to = *(stdRef*)(data->nodeSet[data->currentNode].access.data);
        stdRef from = *(stdRef*)(data->nodeSet[data->currentNode].access.data + sizeof(stdRef));

        EVAL_STDREF(to);
        EVAL_STDREF(from);

        nthp::script::stageMemory[nthp::fixedToInt(to.value)] = nthp::fixedToInt(from.value);

        return 0;
}

DEFINE_EXECUTION_BEHAVIOUR(SM_READ) {
        stdRef location = *(stdRef*)(data->nodeSet[data->currentNode].access.data);
        ptrRef output = *(ptrRef*)(data->nodeSet[data->currentNode].access.data + sizeof(ptrRef));

        EVAL_STDREF(location);
        EVAL_STDREF(output);

        if(PR_METADATA_GET(output, nthp::script::flagBits::IS_GLOBAL)) {
                data->globalVarSet[nthp::fixedToInt(output.value)] = nthp::intToFixed(nthp::script::stageMemory[nthp::fixedToInt(location.value)]);
        }
        else {
                (*data->currentLocalMemory)[nthp::fixedToInt(output.value)] = nthp::intToFixed(nthp::script::stageMemory[nthp::fixedToInt(location.value)]);
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
        if(data->entityBlockSize > 0) delete[] data->entityBlock;
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


DEFINE_EXECUTION_BEHAVIOUR(ENT_CHECKCOLLISION) {
        stdRef entA = *(stdRef*)(data->nodeSet[data->currentNode].access.data);
        stdRef entB = *(stdRef*)(data->nodeSet[data->currentNode].access.data + sizeof(stdRef));
        ptrRef output = *(ptrRef*)(data->nodeSet[data->currentNode].access.data + sizeof(stdRef) + sizeof(stdRef));
        
        EVAL_STDREF(entA);
        EVAL_STDREF(entB);
        EVAL_STDREF(output);

        if(PR_METADATA_GET(output, nthp::script::flagBits::IS_GLOBAL)) {
                
                data->globalVarSet[nthp::fixedToInt(output.value)] = nthp::intToFixed((int)nthp::entity::checkRectCollision(data->entityBlock[nthp::fixedToInt(entA.value)].getHitbox(), data->entityBlock[nthp::fixedToInt(entB.value)].getHitbox()));
        }
        else {
                (*data->currentLocalMemory)[nthp::fixedToInt(output.value)] = nthp::intToFixed((int)nthp::entity::checkRectCollision(data->entityBlock[nthp::fixedToInt(entA.value)].getHitbox(), data->entityBlock[nthp::fixedToInt(entB.value)].getHitbox()));
        }

        return 0;
}


DEFINE_EXECUTION_BEHAVIOUR(CORE_INIT) {
        if(nthp::core.getInitSuccess())
                nthp::core.cleanup();

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

DEFINE_EXECUTION_BEHAVIOUR(CORE_STOP) {
        nthp::core.stop();
        data->isSuspended = true;
        PRINT_DEBUG("Core SHUTDOWN call with CORE_STOP...\n");

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
        stdRef target = *(stdRef*)(data->nodeSet[data->currentNode].access.data);
        uint32_t var = *(uint32_t*)(data->nodeSet[data->currentNode].access.data + sizeof(stdRef));
        int32_t key = *(int32_t*)(data->nodeSet[data->currentNode].access.data + sizeof(stdRef) + sizeof(uint32_t));

        EVAL_STDREF(target);

        data->actionList[nthp::fixedToInt(target.value)].varIndex = var;  
        data->actionList[nthp::fixedToInt(target.value)].boundKey = key;

#ifdef PM
        GENERIC_PRINT("bound ACTION [%d] key index [%d] to GLOBAL [%u]\n", nthp::fixedToInt(target.value), key, var);
#endif
        return 0;
}

DEFINE_EXECUTION_BEHAVIOUR(ACTION_CLEAR) {
        delete[] data->actionList;
        data->actionListSize = 0;

        return 0;
}

DEFINE_EXECUTION_BEHAVIOUR(STAGE_LOAD) {
        data->changeStage = true;
        // Copies new stage name into stage memory. 
        memcpy(nthp::script::stageMemory, data->nodeSet[data->currentNode].access.data, data->nodeSet[data->currentNode].access.size);
        data->isSuspended = true;

        return 0;
}


DEFINE_EXECUTION_BEHAVIOUR(POLL_ENT_POSITION) {
        stdRef target = *(ptrRef*)(data->nodeSet[data->currentNode].access.data);
        EVAL_STDREF(target);

        const auto pos = data->entityBlock[nthp::fixedToInt(target.value)].getPosition();

        data->globalVarSet[RPOLL1_GLOBAL_INDEX] = pos.x;
        data->globalVarSet[RPOLL2_GLOBAL_INDEX] = pos.y;

        return 0;
}

DEFINE_EXECUTION_BEHAVIOUR(POLL_ENT_CURRENTFRAME) {
        stdRef target = *(stdRef*)(data->nodeSet[data->currentNode].access.data);
        EVAL_STDREF(target);

        const auto cf = data->entityBlock[nthp::fixedToInt(target.value)].getCurrentFrameIndex();
        
        data->globalVarSet[RPOLL1_GLOBAL_INDEX] = nthp::intToFixed(cf);

        return 0;
}

DEFINE_EXECUTION_BEHAVIOUR(POLL_ENT_HITBOX) {
        stdRef target = *(stdRef*)(data->nodeSet[data->currentNode].access.data);
        EVAL_STDREF(target);

        const auto box = data->entityBlock[nthp::fixedToInt(target.value)].getHitbox();

        data->globalVarSet[RPOLL1_GLOBAL_INDEX] = box.x;
        data->globalVarSet[RPOLL2_GLOBAL_INDEX] = box.y;
        data->globalVarSet[RPOLL3_GLOBAL_INDEX] = box.w;
        data->globalVarSet[RPOLL4_GLOBAL_INDEX] = box.h;

        return 0;
}

DEFINE_EXECUTION_BEHAVIOUR(POLL_ENT_RENDERSIZE) {
        stdRef target = *(stdRef*)(data->nodeSet[data->currentNode].access.data);
        EVAL_STDREF(target);
        

        const auto rs = data->entityBlock[target.value].getRenderSize();

        data->globalVarSet[RPOLL1_GLOBAL_INDEX] = rs.x;
        data->globalVarSet[RPOLL2_GLOBAL_INDEX] = rs.y;

        return 0;
}

DEFINE_EXECUTION_BEHAVIOUR(DRAW_SETCOLOR) {
        stdRef colorIndex = *(stdRef*)(data->nodeSet[data->currentNode].access.data);

        EVAL_STDREF(colorIndex);

        data->penColor = (decltype(data->penColor))nthp::fixedToInt(colorIndex.value);


        return 0;
}

DEFINE_EXECUTION_BEHAVIOUR(DRAW_LINE) {
        stdRef x1 = *(stdRef*)(data->nodeSet[data->currentNode].access.data);
        stdRef y1 = *(stdRef*)(data->nodeSet[data->currentNode].access.data + sizeof(stdRef));
        stdRef x2 = *(stdRef*)(data->nodeSet[data->currentNode].access.data + sizeof(stdRef) + sizeof(stdRef));
        stdRef y2 = *(stdRef*)(data->nodeSet[data->currentNode].access.data + sizeof(stdRef) + sizeof(stdRef) + sizeof(stdRef));

        EVAL_STDREF(x1);
        EVAL_STDREF(y1);
        EVAL_STDREF(x2);
        EVAL_STDREF(y2);

        const nthp::vectGeneric pointA = nthp::generatePixelPosition(nthp::worldPosition(x1.value, y1.value), &nthp::core.p_coreDisplay) + nthp::generatePixelPosition(nthp::core.p_coreDisplay.cameraWorldPosition, &nthp::core.p_coreDisplay);
        const nthp::vectGeneric pointB = nthp::generatePixelPosition(nthp::worldPosition(x2.value, y2.value), &nthp::core.p_coreDisplay) + nthp::generatePixelPosition(nthp::core.p_coreDisplay.cameraWorldPosition, &nthp::core.p_coreDisplay);

        SDL_SetRenderDrawColor(nthp::core.getRenderer(), nthp::script::activePalette.colorSet[data->penColor].R,nthp::script::activePalette.colorSet[data->penColor].G, nthp::script::activePalette.colorSet[data->penColor].B, 255);
        
        SDL_RenderDrawLine(nthp::core.getRenderer(), pointA.x, pointA.y, pointB.x, pointB.y);

        SDL_SetRenderDrawColor(nthp::core.getRenderer(), DEFAULT_RENDER_COLOR);

        return 0;
}


DEFINE_EXECUTION_BEHAVIOUR(SOUND_DEFINE) {
        stdRef size = *(stdRef*)(data->nodeSet[data->currentNode].access.data);

        EVAL_STDREF(size);

        nthp::core.audioSystem.soundEffects = new (std::nothrow) nthp::audio::SoundChannel[nthp::fixedToInt(size.value)];
        if(nthp::core.audioSystem.soundEffects == nullptr) {
                PRINT_DEBUG_ERROR("SOUND_LOAD call failed to allocate sound data.\n");
                nthp::core.audioSystem.soundSize = 0;
        }
        else
                nthp::core.audioSystem.soundSize = nthp::fixedToInt(size.value);


        return 0;
}

DEFINE_EXECUTION_BEHAVIOUR(SOUND_CLEAR) {
        if(nthp::core.audioSystem.soundSize > 0)
                delete[] nthp::core.audioSystem.soundEffects;

        nthp::core.audioSystem.soundSize = 0;

        return 0;
}

DEFINE_EXECUTION_BEHAVIOUR(MUSIC_DEFINE) {
        stdRef size = *(stdRef*)(data->nodeSet[data->currentNode].access.data);

        EVAL_STDREF(size);

        nthp::core.audioSystem.music = new (std::nothrow) nthp::audio::MusicChannel[nthp::fixedToInt(size.value)];
        if(nthp::core.audioSystem.music == nullptr) {
                PRINT_DEBUG_ERROR("MUSIC_DEFINE call failed to allocate sound data. [%d] is not valid.\n", nthp::fixedToInt(size.value));
                nthp::core.audioSystem.musicSize = 0;
        }
        else
                nthp::core.audioSystem.musicSize = nthp::fixedToInt(size.value);


        return 0;
}

DEFINE_EXECUTION_BEHAVIOUR(MUSIC_CLEAR) {
        Mix_HaltMusic();

        if(nthp::core.audioSystem.musicSize > 0)
                delete[] nthp::core.audioSystem.music;

        nthp::core.audioSystem.musicSize = 0;

        return 0;
}

DEFINE_EXECUTION_BEHAVIOUR(MUSIC_LOAD) {
        stdRef objectIndex = *(stdRef*)(data->nodeSet[data->currentNode].access.data);
        const char* filename = (data->nodeSet[data->currentNode].access.data + sizeof(stdRef));

        EVAL_STDREF(objectIndex);

        int ret = nthp::core.audioSystem.music[nthp::fixedToInt(objectIndex.value)].load(filename);
        return ret;
}

DEFINE_EXECUTION_BEHAVIOUR(SOUND_LOAD) {
        stdRef objectIndex = *(stdRef*)(data->nodeSet[data->currentNode].access.data);
        const char* filename = (data->nodeSet[data->currentNode].access.data + sizeof(stdRef));

        EVAL_STDREF(objectIndex);

        int ret = nthp::core.audioSystem.soundEffects[nthp::fixedToInt(objectIndex.value)].load(filename);
        return ret;
}

DEFINE_EXECUTION_BEHAVIOUR(SOUND_PLAY) {
        stdRef obj = *(stdRef*)(data->nodeSet[data->currentNode].access.data);

        EVAL_STDREF(obj);

        nthp::core.audioSystem.soundEffects[nthp::fixedToInt(obj.value)].playSound();
        return 0;
}

DEFINE_EXECUTION_BEHAVIOUR(MUSIC_START) {
        stdRef obj = *(stdRef*)(data->nodeSet[data->currentNode].access.data);

        EVAL_STDREF(obj);

        data->currentMusicTrack = nthp::fixedToInt(obj.value);
        nthp::core.audioSystem.music[nthp::fixedToInt(obj.value)].start();

        return 0;
}

DEFINE_EXECUTION_BEHAVIOUR(MUSIC_STOP) {
        Mix_HaltMusic();
        data->currentMusicTrack = -1;

        return 0;
}

DEFINE_EXECUTION_BEHAVIOUR(MUSIC_PAUSE) {
        if(data->currentMusicTrack > -1)
                nthp::core.audioSystem.music[data->currentMusicTrack].pauseMusic();

        return 0;
}

DEFINE_EXECUTION_BEHAVIOUR(MUSIC_RESUME) {
        if(data->currentMusicTrack > -1)
                nthp::core.audioSystem.music[data->currentMusicTrack].resumeMusic();

        return 0;
}


DEFINE_EXECUTION_BEHAVIOUR(CACHE_DEFINE) {
        stdRef size = *(stdRef*)(data->nodeSet[data->currentNode].access.data);

        EVAL_STDREF(size);

        data->cache = (nthp::script::stdVarWidth*)(malloc(nthp::fixedToInt(size.value) * sizeof(nthp::script::stdVarWidth)));
        if(data->cache == NULL) {
                PRINT_DEBUG_ERROR("Failed to allocate cache; size = [%u].\n", nthp::fixedToInt(size.value));
                return 1;
        }

        PRINT_DEBUG("allocated [%zu] bytes.\n", nthp::fixedToInt(size.value) * sizeof(nthp::script::stdVarWidth));
        data->cacheSize = nthp::fixedToInt(size.value);
        memset(data->cache, 0, nthp::fixedToInt(size.value) * sizeof(nthp::script::stdVarWidth));

        return 0;
}


DEFINE_EXECUTION_BEHAVIOUR(CACHE_RESIZE) {
        stdRef size = *(stdRef*)(data->nodeSet[data->currentNode].access.data);

        EVAL_STDREF(size);

        nthp::script::stdVarWidth* temp = (nthp::script::stdVarWidth*)(realloc(data->cache, nthp::fixedToInt(size.value) * sizeof(nthp::script::stdVarWidth)));
        if(temp == NULL) {
                PRINT_DEBUG_ERROR("Failed to reallocate cache; size = [%u].\n", size.value);
                return 1;
        }

        data->cache = temp;
        data->cacheSize = nthp::fixedToInt(size.value);

        return 0;
}


DEFINE_EXECUTION_BEHAVIOUR(CACHE_OPEN) {
        const char* filename = data->nodeSet[data->currentNode].access.data;

        std::fstream file;
        file.open(filename, std::ios::binary | std::ios::in);
        if(file.fail()) {
                PRINT_DEBUG_ERROR("Unable to open cache file [%s].\n", filename);
                return 1;
        }

        nthp::script::stdVarWidth fileRead;
        file.read((char*)&fileRead, sizeof(fileRead));

        data->cache = (nthp::script::stdVarWidth*)(malloc(nthp::fixedToInt(fileRead) * sizeof(nthp::script::stdVarWidth)));
        data->cacheSize = nthp::fixedToInt(fileRead);
        file.read((char*)data->cache, nthp::fixedToInt(fileRead) * sizeof(nthp::script::stdVarWidth));
        file.close();


        return 0;
}

DEFINE_EXECUTION_BEHAVIOUR(CACHE_CLEAR) {
        if(data->cacheSize > 0) {
                free(data->cache);
                data->cacheSize = 0;
        }

        return 0;
}

DEFINE_EXECUTION_BEHAVIOUR(CACHE_WRITE) {
        stdRef target = *(stdRef*)(data->nodeSet[data->currentNode].access.data);
        stdRef value = *(stdRef*)(data->nodeSet[data->currentNode].access.data + sizeof(stdRef));

        EVAL_STDREF(target);
        EVAL_STDREF(value);


        data->cache[nthp::fixedToInt(target.value)] = value.value;
        return 0;
}


DEFINE_EXECUTION_BEHAVIOUR(CACHE_READ) {
        stdRef target = *(stdRef*)(data->nodeSet[data->currentNode].access.data);
        ptrRef var = *(ptrRef*)(data->nodeSet[data->currentNode].access.data + sizeof(stdRef));

        EVAL_STDREF(target);
        EVAL_STDREF(var);

        if(PR_METADATA_GET(var, nthp::script::flagBits::IS_GLOBAL)) {
                data->globalVarSet[nthp::fixedToInt(var.value)] = data->cache[nthp::fixedToInt(target.value)];
        }
        else {
                (*data->currentLocalMemory)[nthp::fixedToInt(var.value)] = data->cache[nthp::fixedToInt(target.value)];
        }

        return 0;
}

DEFINE_EXECUTION_BEHAVIOUR(CACHE_SAVE) {
        const char* filename = (data->nodeSet[data->currentNode].access.data);

        std::fstream file;
        file.open(filename, std::ios::binary | std::ios::out);
        if(file.fail()) {
                PRINT_DEBUG_ERROR("Unable to open file target [%s].\n", filename);
                return 1;
        }

        const nthp::script::stdVarWidth size = nthp::intToFixed(data->cacheSize);
        file.write((char*)&size, sizeof(nthp::script::stdVarWidth));
        file.write((char*)data->cache, sizeof(nthp::script::stdVarWidth) * data->cacheSize);

        file.close();
        return 0;
}


DEFINE_EXECUTION_BEHAVIOUR(PRINT) {
#ifdef DEBUG
        stdRef output = *(stdRef*)(data->nodeSet[data->currentNode].access.data);

        EVAL_STDREF(output);

        GENERIC_PRINT("%lf\n", nthp::fixedToDouble(output.value));
#endif

        return 0;
}

// Automatically updates ID indecies and places functions accordingly. Just add/change stuff in 's_instructions.hpp'.
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



        import(filename, dataSet);
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

        localVarSet = (decltype(localVarSet))malloc(sizeof(nthp::fixed_t) * localVarSetSize);
        memset(localVarSet, 0, sizeof(nthp::fixed_t) * localVarSetSize);
        

        return 0;
}




int nthp::script::Script::execute() {

        #ifdef DEBUG
                if(nthp::script::debug::suspendExecution) data->isSuspended = true;
                else { data->isSuspended = false; }
                std::mutex debug_access;
        #else
                data->isSuspended = false;
        #endif

        data->currentLocalMemory = &localVarSet;
        data->varSetSize = localVarSetSize;
        data->currentLabelBlock = localLabelBlock;
        data->currentLabelBlockSize = localLabelBlockSize;
        data->currentNode = localCurrentNode;
        data->nodeSet = nodeSet.data();

        #ifdef DEBUG

        debug_access.lock();
                switch(nthp::script::debug::debugInstructionCall.x) {
                        case nthp::script::debug::CONTINUE: {
                                nthp::script::debug::suspendExecution = false;
                                nthp::script::debug::debugInstructionCall.x = -1;
                        }
                                break;
                        // Executes next instruction, then breaks.
                        case nthp::script::debug::STEP: {
                                nthp::script::debug::suspendExecution = false;
                                data->isSuspended = false;
                                
                                break;
                        }
                        case(nthp::script::debug::JUMP_TO): {
                                data->currentNode = nthp::script::debug::debugInstructionCall.y;
                                localCurrentNode = nthp::script::debug::debugInstructionCall.y;
                        }
                                break;

                        default:
                                break;
                }
        debug_access.unlock();

        #endif

        while((data->currentNode < nodeSet.size()) && 
                (data->isSuspended == false)) {
#ifdef DEBUG
                        debug_access.lock();
                        switch(nthp::script::debug::debugInstructionCall.x) {
                                case(nthp::script::debug::JUMP_TO):
                                        data->currentNode = nthp::script::debug::debugInstructionCall.y;
                                        localCurrentNode = nthp::script::debug::debugInstructionCall.y;
                                break;

                                case(nthp::script::debug::BREAK):
                                        nthp::script::debug::suspendExecution = true;
                                        nthp::script::debug::debugInstructionCall.x = -1;
                                        goto SKIP_EXECUTION;
                                        break;

                                case(nthp::script::debug::CONTINUE):
                                        nthp::script::debug::suspendExecution = false;
                                        nthp::script::debug::debugInstructionCall.x = -1;
                                        break;
                                
                                case(nthp::script::debug::STEP):
                                        // Executes the next instruction, and breaks the next tick.
                                        nthp::script::debug::debugInstructionCall.x = nthp::script::debug::BREAK;
                                        break;
                                default:
                                        break;

                        }
                        debug_access.unlock();
#endif

                if(exec_func[nodeSet[data->currentNode].access.ID](data)) return 1;
                ++data->currentNode;
        }
        if(data->currentNode == nodeSet.size()) data->currentNode = 0;

#ifdef DEBUG
        SKIP_EXECUTION: // Using goto. Hail to the king, baby.
#endif

        
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

        // Make sure NOT to delete the localLabelBlock, as tempting as it seems.
        // it points to the HEADER node data, so it's freed with the rest of the
        // nodes. I write this because I made that mistake on Oct. 15, 2024.

        if(!inStageContext) {
                delete data;
        }


        for(size_t i = 0; i < nodeSet.size(); ++i) {
                if(nodeSet[i].access.size > 0) free(nodeSet[i].access.data);
        }
}
