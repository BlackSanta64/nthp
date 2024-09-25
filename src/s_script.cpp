
#include "s_script.hpp"
using namespace nthp::script::instructions;
#define DEFINE_EXECUTION_BEHAVIOUR(instruction) const int instruction (nthp::script::Script::ScriptDataSet* data)

#define EVAL_REF(ref) if(PR_METADATA_GET(ref, nthp::script::flagBits::IS_REFERENCE)) { ref.value = data->varSet[nthp::fixedToInt(ref.value)]; }

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

        EVAL_REF(label);

        for(size_t i = 0; i < data->labelBlockSize; ++i) {
                if(data->labelBlock[i + i] == static_label) {
                        data->currentNode = data->labelBlock[i + i + 1];
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

        EVAL_REF(index);
        data->currentNode = index.value;

        return 0;
}

DEFINE_EXECUTION_BEHAVIOUR(GETINDEX) {
        uint32_t var = *(uint32_t*)data->nodeSet[data->currentNode].access.data;
        data->varSet[var] = nthp::intToFixed(data->currentNode);

        return 0;
}

DEFINE_EXECUTION_BEHAVIOUR(INC) {
        uint32_t var = *(uint32_t*)data->nodeSet[data->currentNode].access.data;
        data->varSet[var] += nthp::intToFixed(1);

        return 0;
}

DEFINE_EXECUTION_BEHAVIOUR(DEC) {
        uint32_t var = *(uint32_t*)data->nodeSet[data->currentNode].access.data;
        data->varSet[var] -= nthp::intToFixed(1);

        return 0;
}

DEFINE_EXECUTION_BEHAVIOUR(LSHIFT) {
        uint32_t var =  *(uint32_t*)data->nodeSet[data->currentNode].access.data;
        stdRef count = *(stdRef*)(data->nodeSet[data->currentNode].access.data + sizeof(uint32_t));

        EVAL_REF(count);
        
        data->varSet[var] = ((data->varSet[var]) << nthp::fixedToInt(count.value));
        return 0;
}

DEFINE_EXECUTION_BEHAVIOUR(RSHIFT) {
        uint32_t var =  *(uint32_t*)data->nodeSet[data->currentNode].access.data;
        stdRef count = *(stdRef*)(data->nodeSet[data->currentNode].access.data + sizeof(uint32_t));

        EVAL_REF(count);

        data->varSet[var] = ((data->varSet[var]) >> nthp::fixedToInt(count.value));
        return 0;
}


DEFINE_EXECUTION_BEHAVIOUR(ADD) {
        stdRef a = *(stdRef*)data->nodeSet[data->currentNode].access.data;
        stdRef b = *(stdRef*)(data->nodeSet[data->currentNode].access.data + sizeof(stdRef));
        uint32_t output = *(uint32_t*)(data->nodeSet[data->currentNode].access.data + sizeof(stdRef) + sizeof(stdRef));


        EVAL_REF(a);
        EVAL_REF(b);
        
        data->varSet[output] = (a.value + b.value);
        return 0;
}

DEFINE_EXECUTION_BEHAVIOUR(SUB) {
        stdRef a = *(stdRef*)data->nodeSet[data->currentNode].access.data;
        stdRef b = *(stdRef*)(data->nodeSet[data->currentNode].access.data + sizeof(stdRef));
        uint32_t output = *(uint32_t*)(data->nodeSet[data->currentNode].access.data + sizeof(stdRef) + sizeof(stdRef));

        EVAL_REF(a);
        EVAL_REF(b);

        data->varSet[output] = (a.value - b.value);
        return 0;
}

DEFINE_EXECUTION_BEHAVIOUR(MUL) {
        stdRef a = *(stdRef*)data->nodeSet[data->currentNode].access.data;
        stdRef b = *(stdRef*)(data->nodeSet[data->currentNode].access.data + sizeof(stdRef));
        uint32_t output = *(uint32_t*)(data->nodeSet[data->currentNode].access.data + sizeof(stdRef) + sizeof(stdRef));

        EVAL_REF(a);
        EVAL_REF(b);

        data->varSet[output] = nthp::f_fixedProduct(a.value, b.value);
        return 0;
}

DEFINE_EXECUTION_BEHAVIOUR(DIV) {
        stdRef a = *(stdRef*)data->nodeSet[data->currentNode].access.data;
        stdRef b = *(stdRef*)(data->nodeSet[data->currentNode].access.data + sizeof(stdRef));
        uint32_t output = *(uint32_t*)(data->nodeSet[data->currentNode].access.data + sizeof(stdRef) + sizeof(stdRef));

        EVAL_REF(a);
        EVAL_REF(b);

        data->varSet[output] = nthp::f_fixedQuotient(a.value, b.value);
        return 0;
}

DEFINE_EXECUTION_BEHAVIOUR(SQRT) {
        stdRef base = *(stdRef*)(data->nodeSet[data->currentNode].access.data);
        uint32_t pointer = *(uint32_t*)(data->nodeSet[data->currentNode].access.data + sizeof(stdRef));

        EVAL_REF(base);

        data->varSet[pointer] = nthp::f_sqrt(base.value);
        return 0;
}



DEFINE_EXECUTION_BEHAVIOUR(END) {
        return 0;
}

DEFINE_EXECUTION_BEHAVIOUR(LOGIC_EQU) {
        stdRef opA = *(stdRef*)(data->nodeSet[data->currentNode].access.data);
        stdRef opB = *(stdRef*)(data->nodeSet[data->currentNode].access.data + sizeof(stdRef));
        uint32_t endIndex = *(uint32_t*)(data->nodeSet[data->currentNode].access.data + sizeof(stdRef) + sizeof(stdRef));

        EVAL_REF(opA);
        EVAL_REF(opB);

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

        EVAL_REF(opA);
        EVAL_REF(opB);
        
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

        EVAL_REF(opA);
        EVAL_REF(opB);

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

        EVAL_REF(opA);
        EVAL_REF(opB);

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

        EVAL_REF(opA);
        EVAL_REF(opB);

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

        EVAL_REF(opA);
        EVAL_REF(opB);

        if(opA.value <= opB.value)
                return 0;
        else 
                data->currentNode = endIndex;

        return 0;
}

DEFINE_EXECUTION_BEHAVIOUR(SET) {
        uint32_t pointer = *(uint32_t*)(data->nodeSet[data->currentNode].access.data);
        nthp::script::stdVarWidth value = *(nthp::script::stdVarWidth*)(data->nodeSet[data->currentNode].access.data + sizeof(uint32_t));

        data->varSet[pointer] = value;

        return 0;
}

DEFINE_EXECUTION_BEHAVIOUR(CLEAR) {
        delete[] data->varSet;
        data->varSetSize = 0;

        return 0;
}

DEFINE_EXECUTION_BEHAVIOUR(DEFINE) {
        uint32_t size = *(uint32_t*)(data->nodeSet[data->currentNode].access.data);
        data->varSet = new nthp::script::stdVarWidth[size];

        if(data->varSet == nullptr) return 1;

        data->varSetSize = size;
        return 0;
}

DEFINE_EXECUTION_BEHAVIOUR(COPY) {
        uint32_t from = *(uint32_t*)(data->nodeSet[data->currentNode].access.data);
        uint32_t to = *(uint32_t*)(data->nodeSet[data->currentNode].access.data + sizeof(uint32_t));

        data->varSet[to] = data->varSet[from];
        return 0;
}

DEFINE_EXECUTION_BEHAVIOUR(TEXTURE_DEFINE) {
	stdRef size = *(stdRef*)(data->nodeSet[data->currentNode].access.data);
	
	EVAL_REF(size);
	
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

        EVAL_REF(output);

	if(nthp::fixedToInt(output.value) > data->textureBlockSize) {
		PRINT_DEBUG_ERROR("Output index of LOAD_TEXTURE instuction out of bounds.\n");		
		return 1;
	}
	
	data->textureBlock[nthp::fixedToInt(output.value)].autoLoadTextureFile(file, &data->activePalette, nthp::core.getRenderer());

	return 0;
}

DEFINE_EXECUTION_BEHAVIOUR(SET_ACTIVE_PALETTE) {

        data->activePalette.importPaletteFromFile(data->nodeSet[data->currentNode].access.data);

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

        EVAL_REF(px);
        EVAL_REF(py);
        EVAL_REF(tx);
        EVAL_REF(ty);
        EVAL_REF(cx);
        EVAL_REF(cy);

        const char* title = (char*)(data->nodeSet[data->currentNode].access.data + (sizeof(stdRef) * 6) + sizeof(uint8_t));

        nthp::core.init(nthp::RenderRuleSet(nthp::fixedToInt(px.value), nthp::fixedToInt(py.value), tx.value, ty.value, nthp::vectFixed(cx.value, cy.value)), title, (flags >> NTHP_CORE_INIT_FULLSCREEN) & 1, (flags >> NTHP_CORE_INIT_SOFTWARE_RENDERING) & 1);


        return 0;
}

DEFINE_EXECUTION_BEHAVIOUR(FRAME_DEFINE) {
        stdRef size = *(stdRef*)(data->nodeSet[data->currentNode].access.data);
        
        EVAL_REF(size);
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

        EVAL_REF(frameIndex);
        EVAL_REF(x);
        EVAL_REF(y);
        EVAL_REF(w);
        EVAL_REF(h);
        EVAL_REF(textureIndex);

        SDL_Rect rect;
        rect.x = nthp::fixedToInt(x.value);
        rect.y = nthp::fixedToInt(y.value);
        rect.w = nthp::fixedToInt(w.value);
        rect.h = nthp::fixedToInt(h.value);
        
        data->frameBlock[nthp::fixedToInt(frameIndex.value)].src = rect;
        data->frameBlock[nthp::fixedToInt(frameIndex.value)].texture = data->textureBlock[nthp::fixedToInt(textureIndex.value)].getTextureData().texture;

        return 0;
}


// Genius design; Automatically updates ID indecies and places functions accordingly. Just add/change stuff in 's_instructions.hpp'.
// the 'nthp::script::instructions::ID' will correspond with the index of the desired instruction in this array.
static const int (*exec_func[nthp::script::instructions::ID::numberOfInstructions])(nthp::script::Script::ScriptDataSet* data) { INSTRUCTION_TOKENS() };



/////////////////////////////////////////////////////
/////////////////////////////////////////////////////


nthp::script::Script::Script() {
        data.varSet = nullptr;
        data.varSetSize = 0;

        data.entityBlock = nullptr;
        data.entityBlockSize = 0;

        data.textureBlock = nullptr;
        data.textureBlockSize = 0;

        data.frameBlock = nullptr;
        data.frameBlockSize = 0;

        data.currentNode = 0;
        data.localMemBudget = 0;
        data.globalMemBudget = 0;
        data.labelBlock = nullptr;

}

nthp::script::Script::Script(const char* filename) {
        data.varSet = nullptr;
        data.varSetSize = 0;

        data.entityBlock = nullptr;
        data.entityBlockSize = 0;

        data.textureBlock = nullptr;
        data.textureBlockSize = 0;

        data.frameBlock = nullptr;
        data.frameBlockSize = 0;


        data.localMemBudget = 0;
        data.globalMemBudget = 0;
        data.labelBlock = nullptr;

        data.currentNode = 0;

        this->import(filename);
}


int nthp::script::Script::import(const char* filename) {
        std::fstream file(filename, std::ios::in | std::ios::binary);

        if(file.fail()) {
                PRINT_DEBUG_ERROR("Unable to load Script file [%s]; File not found.\n");
                
                return 1;
        }


        nthp::script::Node read;
        
        {
                for(size_t i = 0; read.access.ID != GET_INSTRUCTION_ID(EXIT); ++i) {

                        file.read((char*)&read, sizeof(nthp::script::Node::n_file_t));
                        data.nodeSet.push_back((read));
                        NOVERB_PRINT_DEBUG("[%zu] ID:%u Size:%u\n", i, read.access.ID, read.access.size);
                        
                        if(data.nodeSet.back().access.size != 0) {
                                data.nodeSet.back().access.data = (char*)malloc(data.nodeSet.back().access.size);
                                file.read(data.nodeSet.back().access.data, data.nodeSet.back().access.size);
                        }

                        else data.nodeSet.back().access.data = nullptr;

                }
        }

        data.localMemBudget = *(uint32_t*)(data.nodeSet[0].access.data);
        data.globalMemBudget = *(uint32_t*)(data.nodeSet[0].access.data + sizeof(uint32_t)); // Unused unless in stage context.

        // Note this is a pointer to the region that stores labels.
        data.labelBlock = (uint32_t*)(data.nodeSet[0].access.data + (sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t)));
        data.labelBlockSize = *(uint32_t*)(data.nodeSet[0].access.data + (sizeof(uint32_t) + sizeof(uint32_t)));

        data.varSet = (decltype(data.varSet))malloc(sizeof(nthp::fixed_t) * data.localMemBudget);
        memset(data.varSet, 0, sizeof(nthp::fixed_t) * data.localMemBudget);

        data.varSetSize = data.localMemBudget;

        return 0;
}




int nthp::script::Script::execute() {
        data.isSuspended = false;

        while(data.currentNode < data.nodeSet.size() && data.isSuspended == false) {
                if(exec_func[data.nodeSet[data.currentNode].access.ID](&data)) return 1;

                ++data.currentNode;
        }
        if(data.currentNode == data.nodeSet.size()) data.currentNode = 0;

        return 0;
}


int nthp::script::Script::execute(uint32_t entryPoint) {
        data.currentNode = entryPoint;

        return execute();
}




nthp::script::Script::~Script() {
        if(data.varSetSize > 0)
                delete[] data.varSet;
        if(data.textureBlockSize > 0)
                delete[] data.textureBlock;
        if(data.frameBlockSize > 0)
                delete[] data.frameBlock;
        if(data.entityBlockSize > 0)
                delete[] data.entityBlock;

        for(size_t i = 0; i < data.nodeSet.size(); ++i) {
                if(data.nodeSet[i].access.size > 0) free(data.nodeSet[i].access.data);
        }
}
