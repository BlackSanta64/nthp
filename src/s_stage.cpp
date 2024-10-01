#include "s_stage.hpp"


nthp::script::stage::Stage::Stage() {
        memset(&data, 0, sizeof(nthp::script::Script::ScriptDataSet));
        scriptBlock = NULL;
        triggerBlock = NULL;

        stageSize = 0;
}

nthp::script::stage::Stage::Stage(const char* stagefile) {
        memset(&data, 0, sizeof(nthp::script::Script::ScriptDataSet));
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
                        delete[] scriptBlock;
                        delete[] triggerBlock;
                        
                        return 1;
                }
                triggerBlock[i] = configs[i].trig;
                if(triggerBlock[i].ID == TRIG_TICK) {
                        tickList.push_back(i);
                        continue;
                }
                if(triggerBlock[i].ID == TRIG_LOGIC) {
                        logicList.push_back(i);
                        continue;
                }
                if(triggerBlock[i].ID == TRIG_INIT) {
                        initList.push_back(i);
                        continue;
                }
                if(triggerBlock[i].ID == TRIG_EXIT) {
                        exitList.push_back(i);
                        continue;
                }
        }

        // Allocate all global memory at once.
        data.globalVarSet = new nthp::script::stdVarWidth[data.globalMemBudget];


        return 0;
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







nthp::script::stage::Stage::~Stage() {
        if(stageSize > 0) {
                delete[] scriptBlock;
                delete[] triggerBlock;
        }
}