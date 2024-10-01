#pragma once

#include "s_script.hpp"


namespace nthp {
        namespace script {
                namespace stage {
                        
                        // Trigger IDs; 3 identifier bits.
                        #define TRIG_INIT      0b000
                        #define TRIG_EXIT       0b001
                        #define TRIG_TICK     0b010
                        #define TRIG_LOGIC     0b011
                        #define TRIG_HIDDEN     0b100

                        // Is not an execution trigger; highlights the end of the compiled stage file.
                        #define TRIG_END        0b111

                        extern uint8_t stagemMemory[UINT8_MAX];


                        struct scriptConfig {
                                std::string scriptFile;
                                scriptTriggerComplex trig;
                        };

                        typedef uint16_t trigger_w;
                        // BIT ORDER FOR STAGE TRIGGER ;
                        // 0-2  ; ID (3bits)
                        // 3-7  ; GPR (5bits)
                        // 8-15 ; MEM (8bits)

                        #define TRIGGER_SETID(trigger, ID)      (((uint16_t)trigger) |= (ID))
                        #define TRIGGER_SETGPR(trigger, GPR)    (((uint16_t)trigger) |= ((GPR << 3)))
                        #define TRIGGER_SETMEM(trigger, MEM)    (((uint16_t)trigger) |= (((uint16_t)MEM) << 8))

                        #define TRIGGER_GETID(trigger)      (((uint16_t)trigger) & 0b111)
                        #define TRIGGER_GETGPR(trigger)    ((((uint16_t)trigger) >> 3) & 0b11111)
                        #define TRIGGER_GETMEM(trigger)    ((((uint16_t)trigger) >> 8) & 0b11111111)

                        // Upper-most object in the script structure. Contains a list of scripts and
                        // 'triggers' to define when to execute them.
                        class Stage {
                        public:
                                Stage();
                                Stage(const char* stageFile);


                                int loadStage(const char* filename);

                                int init();
                                int tick();
                                int exit();
                                int logic();

                                nthp::script::Script& getScript(size_t index) { return scriptBlock[index]; }


                                ~Stage();
                        private:
                                nthp::script::Script* scriptBlock;
                                nthp::script::scriptTriggerComplex* triggerBlock;
                                size_t stageSize;

                                std::vector<uint32_t> tickList;
                                std::vector<uint32_t> logicList;
                                std::vector<uint32_t> exitList;
                                std::vector<uint32_t> initList;

                                nthp::script::Script::ScriptDataSet data;

                                uint32_t globalMemBudget;
                        };
                }
        }
}