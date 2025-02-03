#include "s_stage.hpp"
#ifdef PM
        int nthp::script::stage::currentStagePhase = 0;
#endif

nthp::script::stage::Stage::Stage() {
        memset(&data, 0, sizeof(nthp::script::Script::ScriptDataSet));
        data.currentMusicTrack = -1;
        scriptBlock = NULL;
        triggerBlock = NULL;
        

        stageSize = 0;
}

nthp::script::stage::Stage::Stage(const char* stagefile) {
        memset(&data, 0, sizeof(nthp::script::Script::ScriptDataSet));
        data.currentMusicTrack = -1;
        if(loadStage(stagefile)) {
                scriptBlock = NULL;
                triggerBlock = NULL;

                stageSize = 0;
        }

}


int nthp::script::stage::Stage::loadStage(const char* stageFile) {
        std::fstream file(stageFile, std::ios::in | std::ios::binary);
        
        if(file.fail()) {
                PRINT_DEBUG_ERROR("Unable to load stage file [%s]; File not found.\n", stageFile);
                return 1;
        }

        if(stageSize > 0) {
                if(stageSize > 0) {
                        delete[] scriptBlock;
                        delete[] triggerBlock;
                }
                if(data.textureBlockSize > 0) delete[] data.textureBlock;
                if(data.entityBlockSize > 0) delete[] data.entityBlock;
                if(data.frameBlockSize > 0) delete[] data.frameBlock;
                if(data.varSetSize > 0) delete[] data.globalVarSet;
        }

        memset(&data, 0, sizeof(nthp::script::Script::ScriptDataSet));
        data.globalMemBudget = COUNT_PREDEFINED_GLOBALS;
        
        std::vector<nthp::script::stage::scriptConfig> configs;
        nthp::script::stage::scriptConfig temp;
        uint8_t length = 0;
        uint16_t primTrig = 0;
        char nameBlock[UINT8_MAX];

        do {
                nameBlock[0] = '\000';
                file.read((char*)&primTrig, sizeof(primTrig));
                file.read((char*)&length, sizeof(length));
                file.read(nameBlock, length);
                nameBlock[length] = '\000';

                temp.scriptFile = nameBlock;
                temp.trig.ID = TRIGGER_GETID(primTrig);
                temp.trig.GPR = TRIGGER_GETGPR(primTrig);
                temp.trig.MEM = TRIGGER_GETMEM(primTrig);

                configs.push_back(temp);
                NOVERB_PRINT_DEBUG("ID=%u GPR=%u MEM=%u src=%s\n", temp.trig.ID, temp.trig.GPR, temp.trig.MEM, temp.scriptFile.c_str());
        } while(TRIGGER_GETID(primTrig) != TRIG_END);


        scriptBlock = new nthp::script::Script[configs.size()];
        triggerBlock = new nthp::script::scriptTriggerComplex[configs.size()];
        stageSize = configs.size();

        for(size_t i = 0; i < configs.size() - 1; ++i) {

                if(scriptBlock[i].import(configs[i].scriptFile.c_str(), &data)) {
                        PRINT_DEBUG_ERROR("Unable to complete stage import; binary [%s] failed to load.\n", configs[i].scriptFile.c_str());
                        
                        
                        return 1;
                }
                triggerBlock[i] = configs[i].trig;

                switch(triggerBlock[i].ID) {
                        case TRIG_TICK:
                                tickList.push_back(i);
                                break;
                        case TRIG_LOGIC:
                                logicList.push_back(i);
                                break;
                        case TRIG_INIT:
                                initList.push_back(i);
                                break;
                        case TRIG_EXIT:
                                exitList.push_back(i);
                                break;
                        
                        default:
                                PRINT_DEBUG_ERROR("Invalid Trigger type [%X]; Aborting...\n", triggerBlock[i].ID);
                                delete[] scriptBlock;
                                delete[] triggerBlock;
                                return 1;
                }

        }

        // Allocate all global memory at once.
        data.globalVarSet = new nthp::script::stdVarWidth[data.globalMemBudget];
        memset((data.globalVarSet), 0, sizeof(nthp::script::stdVarWidth) * data.globalMemBudget);
        PRINT_DEBUG("Allocating [%zu] bytes; global stage memory.\n", sizeof(nthp::script::stdVarWidth) * data.globalMemBudget);


        return 0;
}


void nthp::script::stage::Stage::handleEvents() {
        int x,y;
        SDL_GetMouseState(&x, &y);
        nthp::mousePosition = nthp::generateWorldPosition(nthp::vectGeneric(x, y), &nthp::core.p_coreDisplay);
        nthp::mousePosition -= nthp::core.p_coreDisplay.cameraWorldPosition;

        data.globalVarSet[MOUSEPOS_X_GLOBAL_INDEX] = nthp::mousePosition.x;
        data.globalVarSet[MOUSEPOS_Y_GLOBAL_INDEX] = nthp::mousePosition.y;

        while(SDL_PollEvent(&nthp::core.eventList)) {
                switch(nthp::core.eventList.type) {
                case SDL_QUIT:
                        nthp::core.stop();
                break;

                case SDL_KEYDOWN:
                        for(size_t i = 0; i < data.actionListSize; ++i) {
                                if(nthp::core.eventList.key.keysym.sym == data.actionList[i].boundKey) {
                                        data.globalVarSet[data.actionList[i].varIndex] = nthp::intToFixed(1);
                                }
                        }
                break;
                
                case SDL_KEYUP:
                        for(size_t i = 0; i < data.actionListSize; ++i) {
                                if(nthp::core.eventList.key.keysym.sym == data.actionList[i].boundKey) {
                                        data.globalVarSet[data.actionList[i].varIndex] = 0;
                                }
                        }
                break;

                case SDL_MOUSEBUTTONDOWN:
                        if(nthp::core.eventList.button.button == SDL_BUTTON_LEFT) {
                                data.globalVarSet[MOUSE1_GLOBAL_INDEX] = nthp::intToFixed(1);
                                break;
                        }
                        if(nthp::core.eventList.button.button == SDL_BUTTON_RIGHT) {
                                data.globalVarSet[MOUSE2_GLOBAL_INDEX] = nthp::intToFixed(1);
                                break;
                        }
                        if(nthp::core.eventList.button.button == SDL_BUTTON_MIDDLE) {
                                data.globalVarSet[MOUSE3_GLOBAL_INDEX] = nthp::intToFixed(1);
                                break;
                        }
                        
                break;

                case SDL_MOUSEBUTTONUP:
                        if(nthp::core.eventList.button.button == SDL_BUTTON_LEFT) {
                                data.globalVarSet[MOUSE1_GLOBAL_INDEX] = 0;
                                break;
                        }
                        if(nthp::core.eventList.button.button == SDL_BUTTON_RIGHT) {
                                data.globalVarSet[MOUSE2_GLOBAL_INDEX] = 0;
                                break;
                        }
                        if(nthp::core.eventList.button.button == SDL_BUTTON_MIDDLE) {
                                data.globalVarSet[MOUSE3_GLOBAL_INDEX] = 0;
                                break;
                        }
                break;


                default:
                        break;
                }
        }
}


int nthp::script::stage::Stage::init() {
        for(size_t i = 0; i < initList.size(); ++i) {
                data.currentTriggerConfig = triggerBlock[initList[i]];
                if(scriptBlock[initList[i]].execute()) return 1;
        }

        return 0;
}

int nthp::script::stage::Stage::tick() {
        for(size_t i = 0; i < tickList.size(); ++i) {
                data.currentTriggerConfig = triggerBlock[tickList[i]];

                if(scriptBlock[tickList[i]].execute()) return 1;
        }

        return 0;
}

int nthp::script::stage::Stage::logic() {
        for(size_t i = 0; i < logicList.size(); ++i) {

                // Checks the FIRST BIT of the corresponding stage memory. Can use RSHIFT and LSHIFT
                // with M_STAGE_READ and M_STAGE_WRITE to cycle bits in script.
                if(nthp::script::stageMemory[triggerBlock[logicList[i]].MEM] & 1) {
                        data.currentTriggerConfig = triggerBlock[logicList[i]];

                        if(scriptBlock[logicList[i]].execute()) return 1;
                }
        }


        return 0;
}

int nthp::script::stage::Stage::exit() {
        for(size_t i = 0; i < exitList.size(); ++i) {
                data.currentTriggerConfig = triggerBlock[exitList[i]];
                
                if(scriptBlock[exitList[i]].execute()) return 1;
        }

        return 0;
}


// Cleanly erases the whole stage; scripts, triggers, and any shared script data.
// After calling, stage object is safe to use again (i.e. load another stage file)
void nthp::script::stage::Stage::clean() {
        if(stageSize > 0) {
                delete[] scriptBlock;
                delete[] triggerBlock;
        }
        if(data.textureBlockSize > 0) delete[] data.textureBlock;
        if(data.entityBlockSize > 0) delete[] data.entityBlock;
        if(data.frameBlockSize > 0) delete[] data.frameBlock;
        if(data.globalMemBudget > 0) delete[] data.globalVarSet;
        if(data.actionListSize > 0) delete[] data.actionList;

        memset(&data, 0, sizeof(data));

        initList.clear();
        tickList.clear();
        logicList.clear();
        exitList.clear();

        globalMemBudget = 0;
        stageSize = 0;
}




nthp::script::stage::Stage::~Stage() {
        clean();
}