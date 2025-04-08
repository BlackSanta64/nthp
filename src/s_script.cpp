
#include "s_script.hpp"
using namespace nthp::script::instructions;
#define DEFINE_EXECUTION_BEHAVIOUR(instruction) const int instruction (nthp::script::Script::ScriptDataSet* data)

char nthp::script::stageMemory[STAGEMEM_MAX];
nthp::texture::Palette nthp::script::activePalette;


inline void ____eval_std(stdRef& ref, nthp::script::Script::ScriptDataSet* data) {
        if(PR_METADATA_GET(ref, nthp::script::flagBits::IS_REFERENCE)) {
                ref.value = data->globalVarSet[nthp::fixedToInt(ref.value)];

                // This is okay because the compiler simplifies ptr_descriptor call dereferences.
                // Technically *&var is syntaxically correct and will evaluate correctly, but will take much longer.
                // The compiler therefore simplifies constant ptr_descriptor dereferences to a simple reference '$'.
                if(PR_METADATA_GET(ref, nthp::script::flagBits::IS_PTR)) {
                        const auto ptr = nthp::script::parsePtrDescriptor(ref.value);
                        if(ptr.block) {
                                ref.value = data->blockData[ptr.block - 1].data[ptr.address + ref.offset];
                                return;
                        }
                        ref.value = data->globalVarSet[ptr.address];
                }
                        
                if(PR_METADATA_GET(ref, nthp::script::flagBits::IS_NEGATED)) ref.value = -(ref.value);
        }
}
        


#define EVAL_STDREF(ref)        ____eval_std(ref, data)

#define EVAL_PTRREF(ref)\
        nthp::script::stdVarWidth* target_dsc;\
        do {\
                EVAL_STDREF(ref);\
                const auto ptr_dsc = nthp::script::parsePtrDescriptor(ref.value);\
                if(ptr_dsc.block) { target_dsc = (data->blockData[ptr_dsc.block - 1].data + ptr_dsc.address + ref.offset); break; }\
                target_dsc = (data->globalVarSet + ptr_dsc.address);\
        }\
        while(0)

inline char* ____eval_str(ptrRef& ref, nthp::script::Script::ScriptDataSet* data) {
        if(PR_METADATA_GET(ref, nthp::script::flagBits::IS_NODE_STRING_PTR)) {
                return data->nodeSet[nthp::fixedToInt(ref.value) + data->currentScriptHeaderLocation].access.data;
        }
        EVAL_STDREF(ref);

        const auto ptr_dsc = nthp::script::parsePtrDescriptor(ref.value);
        if(ptr_dsc.block) { return (char*)(data->blockData[ptr_dsc.block - 1].data + ptr_dsc.address); }
        
        return (char*)(data->globalVarSet + ptr_dsc.address);
}




#define EVAL_STRREF(ref)        ____eval_str(ref, data)


#ifdef DEBUG
        nthp::vectGeneric nthp::script::debug::debugInstructionCall = nthp::vectGeneric(-1, -1);
        bool nthp::script::debug::suspendExecution = false;
#endif



DEFINE_EXECUTION_BEHAVIOUR(EXIT) {
        data->isSuspended = true;

        return 0;
}

DEFINE_EXECUTION_BEHAVIOUR(HEADER) {
        data->currentScriptHeaderLocation = data->currentNode;

        return 0;
}

DEFINE_EXECUTION_BEHAVIOUR(LABEL) {
        return 0;
}

// Fastest instruction jumping.
DEFINE_EXECUTION_BEHAVIOUR(GOTO) {
        data->currentNode = (*(uint32_t*)data->nodeSet[data->currentNode].access.data) + data->currentScriptHeaderLocation;
        --data->currentNode;

        return 0;
}

DEFINE_EXECUTION_BEHAVIOUR(JUMP) {
        stdRef label = *(stdRef*)data->nodeSet[data->currentNode].access.data;
        uint32_t static_label = nthp::fixedToInt(label.value);

        EVAL_STDREF(label);

        for(uint32_t i = 0; i < data->currentLabelBlockSize; ++i) {
                if(data->currentLabelBlock[i + i] == static_label) {
                        data->currentNode = data->currentLabelBlock[i + i + 1] + data->currentScriptHeaderLocation;
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
        --data->stackPointer;
        const nthp::script::Script::ReturnStackEntry ret = data->returnStack[data->stackPointer];

        data->currentScriptHeaderLocation = ret.sourceHeaderLocation;
        data->currentNode = ret.sourceDestination;

        --data->currentNode;
        return 0;
}

DEFINE_EXECUTION_BEHAVIOUR(GETINDEX) {
        ptrRef var = *(ptrRef*)data->nodeSet[data->currentNode].access.data;
        EVAL_PTRREF(var);

        *target_dsc = nthp::intToFixed(data->currentNode);

        return 0;
}

DEFINE_EXECUTION_BEHAVIOUR(INC) {
        ptrRef var = *(ptrRef*)data->nodeSet[data->currentNode].access.data;
        EVAL_PTRREF(var);

        *target_dsc = *target_dsc + nthp::intToFixed(1);

        return 0;
}

DEFINE_EXECUTION_BEHAVIOUR(DEC) {
        ptrRef var = *(ptrRef*)data->nodeSet[data->currentNode].access.data;
        EVAL_PTRREF(var);

       
        *target_dsc = *target_dsc - nthp::intToFixed(1);

        return 0;
}

DEFINE_EXECUTION_BEHAVIOUR(LSHIFT) {
        ptrRef var =  *(ptrRef*)data->nodeSet[data->currentNode].access.data;
        stdRef count = *(stdRef*)(data->nodeSet[data->currentNode].access.data + sizeof(ptrRef));

        EVAL_PTRREF(var);
        EVAL_STDREF(count);
        
        *target_dsc = ((data->globalVarSet[var.value]) << nthp::fixedToInt(count.value));

        return 0;
}

DEFINE_EXECUTION_BEHAVIOUR(RSHIFT) {
        ptrRef var =  *(ptrRef*)data->nodeSet[data->currentNode].access.data;
        stdRef count = *(stdRef*)(data->nodeSet[data->currentNode].access.data + sizeof(ptrRef));
        
        
        EVAL_PTRREF(var);
        EVAL_STDREF(count);

        
        *target_dsc = ((data->globalVarSet[var.value]) >> nthp::fixedToInt(count.value));

      

        return 0;
}


DEFINE_EXECUTION_BEHAVIOUR(ADD) {
        stdRef a = *(stdRef*)data->nodeSet[data->currentNode].access.data;
        stdRef b = *(stdRef*)(data->nodeSet[data->currentNode].access.data + sizeof(stdRef));
        ptrRef output = *(ptrRef*)(data->nodeSet[data->currentNode].access.data + sizeof(stdRef) + sizeof(stdRef));


        EVAL_STDREF(a);
        EVAL_STDREF(b);
        EVAL_PTRREF(output);


        *target_dsc = (a.value + b.value);
       

        return 0;
}

DEFINE_EXECUTION_BEHAVIOUR(SUB) {
        stdRef a = *(stdRef*)data->nodeSet[data->currentNode].access.data;
        stdRef b = *(stdRef*)(data->nodeSet[data->currentNode].access.data + sizeof(stdRef));
        ptrRef output = *(ptrRef*)(data->nodeSet[data->currentNode].access.data + sizeof(stdRef) + sizeof(stdRef));


        EVAL_STDREF(a);
        EVAL_STDREF(b);
        EVAL_PTRREF(output);


        *target_dsc = (a.value - b.value);

       
        return 0;
}

DEFINE_EXECUTION_BEHAVIOUR(MUL) {
        stdRef a = *(stdRef*)data->nodeSet[data->currentNode].access.data;
        stdRef b = *(stdRef*)(data->nodeSet[data->currentNode].access.data + sizeof(stdRef));
        ptrRef output = *(ptrRef*)(data->nodeSet[data->currentNode].access.data + sizeof(stdRef) + sizeof(stdRef));


        EVAL_STDREF(a);
        EVAL_STDREF(b);
        EVAL_PTRREF(output);


        *target_dsc = nthp::f_fixedProduct(a.value, b.value);
      
        return 0;
}

DEFINE_EXECUTION_BEHAVIOUR(DIV) {
        stdRef a = *(stdRef*)data->nodeSet[data->currentNode].access.data;
        stdRef b = *(stdRef*)(data->nodeSet[data->currentNode].access.data + sizeof(stdRef));
        ptrRef output = *(ptrRef*)(data->nodeSet[data->currentNode].access.data + sizeof(stdRef) + sizeof(stdRef));


        EVAL_STDREF(a);
        EVAL_STDREF(b);
        EVAL_PTRREF(output);

        *target_dsc = nthp::f_fixedQuotient(a.value, b.value);

        return 0;
}

DEFINE_EXECUTION_BEHAVIOUR(SQRT) {
        stdRef base = *(stdRef*)(data->nodeSet[data->currentNode].access.data);
        ptrRef output = *(ptrRef*)(data->nodeSet[data->currentNode].access.data + sizeof(stdRef));

        EVAL_STDREF(base);
        EVAL_PTRREF(output);

        *target_dsc = nthp::f_sqrt(base.value);

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
        
        data->currentNode = endIndex + data->currentScriptHeaderLocation;

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

        data->currentNode = endIndex + data->currentScriptHeaderLocation;

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

        data->currentNode = endIndex + data->currentScriptHeaderLocation;

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

        data->currentNode = endIndex + data->currentScriptHeaderLocation;

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

        data->currentNode = endIndex + data->currentScriptHeaderLocation;

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

        data->currentNode = endIndex + data->currentScriptHeaderLocation;

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

        data->currentNode = endIndex + data->currentScriptHeaderLocation;

        return 0;
}

DEFINE_EXECUTION_BEHAVIOUR(SET) {
        ptrRef pointer = *(ptrRef*)(data->nodeSet[data->currentNode].access.data);
        stdRef value = *(stdRef*)(data->nodeSet[data->currentNode].access.data + sizeof(ptrRef));

        EVAL_PTRREF(pointer);
        EVAL_STDREF(value);

        *target_dsc = value.value;
       
        return 0;
}

int nthp_internal_alloc(nthp::script::Script::ScriptDataSet* data, nthp::script::stdVarWidth* target_dsc, nthp::script::stdVarWidth size) {
                // Linear search for open blocks. If none, reallocate block memory and use
        // last entry.
        for(size_t i = 0; i < data->blockDataSize; ++i) {
                if(data->blockData[i].isFree) {
                        data->blockData[i].data = (nthp::script::stdVarWidth*)malloc(sizeof(nthp::script::stdVarWidth) * nthp::fixedToInt(size));
                        
              
                        if(data->blockData[i].data == NULL) {
                                PRINT_DEBUG_ERROR("Unable to allocate data block at [%p] (%zu).\n",data->blockData + i, i);
                                return 1;
                        }
        
        
                        data->blockData[i].size = nthp::fixedToInt(size);
                        data->blockData[i].isFree = false;
                        *target_dsc = nthp::script::constructPtrDescriptor(i + 1, 0); // Initalize the ptr to the first element in the allocated block.
                        return 0;
                }
        }

        

        ++data->blockDataSize;
        nthp::script::BlockMemoryEntry* temp = (nthp::script::BlockMemoryEntry*)realloc(data->blockData, sizeof(nthp::script::BlockMemoryEntry) * data->blockDataSize);

        if(temp == NULL) {
                PRINT_DEBUG_ERROR("Unable to resize data block at [%p].\n", data->blockData);
                return 1;
        }
        data->blockData = temp;
        
        data->blockData[data->blockDataSize - 1].data = (nthp::script::stdVarWidth*)malloc(sizeof(nthp::script::stdVarWidth) * nthp::fixedToInt(size));
        data->blockData[data->blockDataSize - 1].size = nthp::fixedToInt(size);
        data->blockData[data->blockDataSize - 1].isFree = false;

        *target_dsc = nthp::script::constructPtrDescriptor(data->blockDataSize, 0); // Initalize the ptr to the first element in the allocated block.

        return 0;
}

DEFINE_EXECUTION_BEHAVIOUR(ALLOC) {
        stdRef size = *(stdRef*)(data->nodeSet[data->currentNode].access.data);
        ptrRef ptrOutput = *(ptrRef*)(data->nodeSet[data->currentNode].access.data + sizeof(stdRef));

        EVAL_STDREF(size);
        EVAL_PTRREF(ptrOutput);

        nthp_internal_alloc(data, target_dsc, size.value);
        
        return 0;
}

DEFINE_EXECUTION_BEHAVIOUR(NEW) {
        stdRef size = *(stdRef*)(data->nodeSet[data->currentNode].access.data);
        ptrRef ptrOutput = *(ptrRef*)(data->nodeSet[data->currentNode].access.data + sizeof(stdRef));
        const uint32_t entrySize = *(uint32_t*)(data->nodeSet[data->currentNode].access.data + sizeof(stdRef) + sizeof(ptrRef));

        EVAL_STDREF(size);
        EVAL_PTRREF(ptrOutput);

        nthp_internal_alloc(data, target_dsc, nthp::intToFixed(nthp::fixedToInt(size.value) * entrySize));

        return 0;
}


DEFINE_EXECUTION_BEHAVIOUR(FREE) {
        ptrRef ptr = *(ptrRef*)(data->nodeSet[data->currentNode].access.data);

        EVAL_PTRREF(ptr);
        const auto ptr_dsc = nthp::script::parsePtrDescriptor(ptr.value);
        
        if(ptr_dsc.block) {
                free(data->blockData[ptr_dsc.block - 1].data);
                data->blockData[ptr_dsc.block - 1].isFree = true;
                data->blockData[ptr_dsc.block - 1].size = 0;

                return 0;
        }

        PRINT_DEBUG_WARNING("FREE at [%zu] Attempted to free global list.\n", data->currentNode);
        return 0;
}

DEFINE_EXECUTION_BEHAVIOUR(COPY) {
        ptrRef src = *(ptrRef*)(data->nodeSet[data->currentNode].access.data);
        stdRef size = *(ptrRef*)(data->nodeSet[data->currentNode].access.data + sizeof(ptrRef));
        ptrRef dst = *(ptrRef*)(data->nodeSet[data->currentNode].access.data + sizeof(ptrRef) + sizeof(stdRef));

        nthp::script::stdVarWidth* r_src = nullptr;
        {                                       // Fucky shit you have to do when evaluating 2 ptrrefs.
                EVAL_PTRREF(src);
                r_src = target_dsc;
        }
        EVAL_STDREF(size);
        EVAL_PTRREF(dst);

        memcpy(target_dsc, r_src, nthp::fixedToInt(size.value) * sizeof(nthp::script::stdVarWidth));

        return 0;
}

DEFINE_EXECUTION_BEHAVIOUR(NEXT) {
        ptrRef ptr = *(ptrRef*)(data->nodeSet[data->currentNode].access.data);
        
        EVAL_PTRREF(ptr);

       
        *target_dsc = (*target_dsc + 1);

        return 0;
}

DEFINE_EXECUTION_BEHAVIOUR(PREV) {
        ptrRef ptr = *(ptrRef*)(data->nodeSet[data->currentNode].access.data);
        
        EVAL_PTRREF(ptr);

        *target_dsc = (*target_dsc - 1);
        return 0;
}

DEFINE_EXECUTION_BEHAVIOUR(INDEX) {
        ptrRef ptr = *(ptrRef*)(data->nodeSet[data->currentNode].access.data);
        stdRef addr = *(stdRef*)(data->nodeSet[data->currentNode].access.data + sizeof(ptrRef));

        EVAL_PTRREF(ptr);
        EVAL_STDREF(addr);

        *target_dsc = ((*target_dsc) & nthp::script::internal_constants::blockMemoryBlockMask) | nthp::fixedToInt(addr.value);
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
	strRef file = *(stdRef*)(data->nodeSet[data->currentNode].access.data + sizeof(stdRef));

        EVAL_STDREF(output);
        auto filename = EVAL_STRREF(file);

	if(nthp::fixedToInt(output.value) > data->textureBlockSize) {
		PRINT_DEBUG_ERROR("Output index of TEXTURE_LOAD instuction out of bounds.\n");		
		return 1;
	}
	
	data->textureBlock[nthp::fixedToInt(output.value)].autoLoadTextureFile(filename, &nthp::script::activePalette, nthp::core.getRenderer());

	return 0;
}

DEFINE_EXECUTION_BEHAVIOUR(SET_ACTIVE_PALETTE) {
        strRef paletteFile = *(strRef*)(data->nodeSet[data->currentNode].access.data);

        auto filename = EVAL_STRREF(paletteFile);

        nthp::script::activePalette.importPaletteFromFile(filename);

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
        EVAL_PTRREF(output);


        *target_dsc = nthp::intToFixed(nthp::script::stageMemory[nthp::fixedToInt(location.value)]);


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
        EVAL_PTRREF(output);


        *target_dsc = nthp::intToFixed((int)nthp::entity::checkRectCollision(data->entityBlock[nthp::fixedToInt(entA.value)].getHitbox(), data->entityBlock[nthp::fixedToInt(entB.value)].getHitbox()));

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
        const uint8_t flags = *(uint8_t*)(data->nodeSet[data->currentNode].access.data + (sizeof(stdRef) * 6));
        strRef title = *(strRef*)(data->nodeSet[data->currentNode].access.data + (sizeof(stdRef) * 6) + sizeof(uint8_t));

        EVAL_STDREF(px);
        EVAL_STDREF(py);
        EVAL_STDREF(tx);
        EVAL_STDREF(ty);
        EVAL_STDREF(cx);
        EVAL_STDREF(cy);
        auto titleString = EVAL_STRREF(title);


        nthp::core.init(nthp::RenderRuleSet(nthp::fixedToInt(px.value), nthp::fixedToInt(py.value), tx.value, ty.value, nthp::vectFixed(cx.value, cy.value)), titleString, (flags >> NTHP_CORE_INIT_FULLSCREEN) & 1, (flags >> NTHP_CORE_INIT_SOFTWARE_RENDERING) & 1);


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
        strRef newStage = *(strRef*)(data->nodeSet[data->currentNode].access.data);

        auto filename = EVAL_STRREF(newStage);

        data->changeStage = true;
        // Copies new stage name into stage memory. 
        int size = 0;
        for(uint8_t i = 0; i < 255; ++i) {
                if(filename[i] == '\000') {
                        size = i;
                        break;
                }
        }

        memcpy(nthp::script::stageMemory, filename, size);
        data->isSuspended = true;

        return 0;
}


DEFINE_EXECUTION_BEHAVIOUR(POLL_ENT_POSITION) {
        stdRef target = *(ptrRef*)(data->nodeSet[data->currentNode].access.data);
        EVAL_STDREF(target);

        const auto pos = data->entityBlock[nthp::fixedToInt(target.value)].getPosition();

        data->globalVarSet[nthp::script::predefined_globals::RPOLL1_GLOBAL_INDEX] = pos.x;
        data->globalVarSet[nthp::script::predefined_globals::RPOLL2_GLOBAL_INDEX] = pos.y;

        return 0;
}

DEFINE_EXECUTION_BEHAVIOUR(POLL_ENT_CURRENTFRAME) {
        stdRef target = *(stdRef*)(data->nodeSet[data->currentNode].access.data);
        EVAL_STDREF(target);

        const auto cf = data->entityBlock[nthp::fixedToInt(target.value)].getCurrentFrameIndex();
        
        data->globalVarSet[nthp::script::predefined_globals::RPOLL1_GLOBAL_INDEX] = nthp::intToFixed(cf);

        return 0;
}

DEFINE_EXECUTION_BEHAVIOUR(POLL_ENT_HITBOX) {
        stdRef target = *(stdRef*)(data->nodeSet[data->currentNode].access.data);
        EVAL_STDREF(target);

        const auto box = data->entityBlock[nthp::fixedToInt(target.value)].getHitbox();

        data->globalVarSet[nthp::script::predefined_globals::RPOLL1_GLOBAL_INDEX] = box.x;
        data->globalVarSet[nthp::script::predefined_globals::RPOLL2_GLOBAL_INDEX] = box.y;
        data->globalVarSet[nthp::script::predefined_globals::RPOLL3_GLOBAL_INDEX] = box.w;
        data->globalVarSet[nthp::script::predefined_globals::RPOLL4_GLOBAL_INDEX] = box.h;

        return 0;
}

DEFINE_EXECUTION_BEHAVIOUR(POLL_ENT_RENDERSIZE) {
        stdRef target = *(stdRef*)(data->nodeSet[data->currentNode].access.data);
        EVAL_STDREF(target);
        

        const auto rs = data->entityBlock[target.value].getRenderSize();

        data->globalVarSet[nthp::script::predefined_globals::RPOLL1_GLOBAL_INDEX] = rs.x;
        data->globalVarSet[nthp::script::predefined_globals::RPOLL2_GLOBAL_INDEX] = rs.y;

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
        strRef filename = *(strRef*)(data->nodeSet[data->currentNode].access.data + sizeof(stdRef));

        EVAL_STDREF(objectIndex);
        auto fileString = EVAL_STRREF(filename);

        int ret = nthp::core.audioSystem.music[nthp::fixedToInt(objectIndex.value)].load(fileString);
        return ret;
}

DEFINE_EXECUTION_BEHAVIOUR(SOUND_LOAD) {
        stdRef objectIndex = *(stdRef*)(data->nodeSet[data->currentNode].access.data);
        strRef filename = *(strRef*)(data->nodeSet[data->currentNode].access.data + sizeof(stdRef));

        EVAL_STDREF(objectIndex);
        auto fileString = EVAL_STRREF(filename);

        int ret = nthp::core.audioSystem.soundEffects[nthp::fixedToInt(objectIndex.value)].load(fileString);
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

DEFINE_EXECUTION_BEHAVIOUR(DFILE_READ) {
        ptrRef target = *(ptrRef*)(data->nodeSet[data->currentNode].access.data);
        strRef filename = *(strRef*)(data->nodeSet[data->currentNode].access.data + sizeof(ptrRef));

        EVAL_PTRREF(target);
        auto fileString = EVAL_STRREF(filename);

        std::fstream file;
        file.open(fileString, std::ios::in | std::ios::binary);
        if(file.fail()) {
                PRINT_DEBUG_ERROR("Unable to open file [%s]; File inaccessible.\n", fileString);
                return 1;
        }
        nthp::script::stdVarWidth fileSize = 0;
        file.read((char*)&fileSize, sizeof(nthp::script::stdVarWidth));

        size_t byteSize = nthp::fixedToInt(fileSize) * sizeof(nthp::script::stdVarWidth);

        nthp_internal_alloc(data, target_dsc, fileSize);
        auto ptr = nthp::script::parsePtrDescriptor(*target_dsc);
        if(ptr.block) {
                file.read((char*)data->blockData[ptr.block - 1].data, fileSize);
                file.close();

                return 0;
        }
        else {
                PRINT_DEBUG_ERROR("Cannot use DFILE_READ to input external data into global list.\n");
                return 1;
        }

        return 0;
}

DEFINE_EXECUTION_BEHAVIOUR(DFILE_WRITE) {
        ptrRef target = *(ptrRef*)(data->nodeSet[data->currentNode].access.data);
        strRef filename = *(strRef*)(data->nodeSet[data->currentNode].access.data + sizeof(ptrRef));


        EVAL_PTRREF(target); // EVAL_PTRREF does NOT change the evaluated ptr_descriptor in 'target.value'; it just creates target_dsc after evaluating it.
        const auto ptr = nthp::script::parsePtrDescriptor(target.value);


        auto fileString = EVAL_STRREF(filename);


        std::fstream file;
        file.open(fileString, std::ios::out | std::ios::binary);
        if(file.fail()) {
                PRINT_DEBUG_ERROR("Unable to open file [%s] for writing; File not accessible.\n", fileString);
                return 1;
        }


        nthp::script::stdVarWidth size = nthp::intToFixed(data->blockData[ptr.block - 1].size);
        printf("size = %u\n", nthp::fixedToInt(size));
        file.write((char*)&size, sizeof(nthp::script::stdVarWidth));

        size_t byteSize = data->blockData[ptr.block - 1].size * sizeof(nthp::script::stdVarWidth);
        file.write((char*)target_dsc, byteSize);

        file.close();

        return 0;
}


DEFINE_EXECUTION_BEHAVIOUR(PRINT_REF) {
#ifdef DEBUG
        stdRef output = *(stdRef*)(data->nodeSet[data->currentNode].access.data);

        EVAL_STDREF(output);

        GENERIC_PRINT("%lf\n", nthp::fixedToDouble(output.value));
#endif

        return 0;
}

DEFINE_EXECUTION_BEHAVIOUR(PRINT_STRING) {
#ifdef DEBUG
        strRef output = *(strRef*)(data->nodeSet[data->currentNode].access.data);

        auto message = EVAL_STRREF(output);

        GENERIC_PRINT("[t %u] %s\n", SDL_GetTicks(), message);
#endif
        return 0;
}

DEFINE_EXECUTION_BEHAVIOUR(STRING) {

        return 0;
}

DEFINE_EXECUTION_BEHAVIOUR(FUNC_START) {

        return 0;
}

DEFINE_EXECUTION_BEHAVIOUR(FUNC_CALL) {
        uint32_t location = *(uint32_t*)(data->nodeSet[data->currentNode].access.data);

        nthp::script::Script::ReturnStackEntry newEntry;
        newEntry.sourceDestination = data->currentNode + 1;
        newEntry.sourceHeaderLocation = data->currentScriptHeaderLocation;

        data->returnStack[data->stackPointer] = newEntry;
        ++(data->stackPointer);

        data->currentNode = location;

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
}

nthp::script::Script::Script(ScriptDataSet* dataSet, uint32_t headerLocation) {
        localCurrentNode = 0;
        localLabelBlock = nullptr;
        localLabelBlockSize = 0;


        import(dataSet, headerLocation);
}


int nthp::script::Script::import(ScriptDataSet* const dataSet, const uint32_t headerLocation) {
        if(dataSet == NULL) {
                data = new nthp::script::Script::ScriptDataSet;
                memset(data, 0, sizeof(nthp::script::Script::ScriptDataSet));
                inStageContext = false;
        }
        else {
                inStageContext = true;
        }


        data = dataSet;
        script_begin = headerLocation;
        if(!(data->nodeSet[headerLocation].access.ID == GET_INSTRUCTION_ID(HEADER))) {
                PRINT_DEBUG_ERROR("Provided script origin does not correspond to valid header node.\n");
                return 1;
        }
        
        // Note this is a pointer to the region that stores labels.
        localLabelBlock = (uint32_t*)(data->nodeSet[headerLocation].access.data + (sizeof(uint32_t) + sizeof(uint32_t)));
        localLabelBlockSize = *(uint32_t*)(data->nodeSet[headerLocation].access.data + (sizeof(uint32_t)));
        localCurrentNode = headerLocation;


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

        data->currentLabelBlock = localLabelBlock;
        data->currentLabelBlockSize = localLabelBlockSize;
        data->currentNode = localCurrentNode;
        data->currentScriptHeaderLocation = script_begin;

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

        while((data->currentNode < data->nodeSetSize) && 
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
                if(exec_func[data->nodeSet[data->currentNode].access.ID](data)) return 1;
                ++data->currentNode;
        }

        if(data->nodeSet[data->currentNode - 1].access.ID == GET_INSTRUCTION_ID(EXIT)) { localCurrentNode = script_begin; }
        else localCurrentNode = data->currentNode;

#ifdef DEBUG
        SKIP_EXECUTION: // Using goto. Hail to the king, baby.
#endif

        

        return 0;
}


int nthp::script::Script::execute(uint32_t entryPoint) {
        localCurrentNode = entryPoint;

        return execute();
}




nthp::script::Script::~Script() {

        // Make sure NOT to delete the localLabelBlock, as tempting as it seems.
        // it points to the HEADER node data, so it's freed with the rest of the
        // nodes. I write this because I made that mistake on Oct. 15, 2024.

        if(!inStageContext) {
                nthp::script::Script::cleanDataSet(data);
        }

}
