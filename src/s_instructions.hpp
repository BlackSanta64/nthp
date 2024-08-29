#pragma once

namespace nthp { namespace script { namespace instructions {


#define INSTRUCTION_LIST(...) typedef enum __inst {\
        __VA_ARGS__\
        } Instruction

#define INSTRUCTION_SIZE_LIST(...) constexpr uint32_t\
        __VA_ARGS__




namespace ID {

        INSTRUCTION_LIST(
                HEADER, 
                EXIT, 


                LABEL,
                GOTO,
                SUSPEND,


                INC,
                DEC,
                ADD,
                SUB,
                MUL,
                DIV,

                LOGIC_EQU,
                LOGIC_NOT,
                LOGIC_GRT,
                LOGIC_LST,
                LOGIC_GRTE,
                LOGIC_LSTE,



                SET,
                CLEAR,



                numberOfInstructions
        );
}

#define GET_INSTRUCTION_ID(instruction) nthp::script::instructions::ID::instruction


namespace Size {

        INSTRUCTION_SIZE_LIST(
                HEADER = 0,
                EXIT = 0,

                LABEL = sizeof(uint32_t),
                GOTO = sizeof(uint32_t),
                SUSPEND = 0,

                INC = sizeof(nthp::script::stdVarWidth),
                DEC = sizeof(nthp::script::stdVarWidth),

                ADD = sizeof(nthp::script::stdVarWidth) + sizeof(nthp::script::stdVarWidth) + sizeof(uint32_t),
                SUB = sizeof(nthp::script::stdVarWidth) + sizeof(nthp::script::stdVarWidth) + sizeof(uint32_t),
                MUL = sizeof(nthp::script::stdVarWidth) + sizeof(nthp::script::stdVarWidth) + sizeof(uint32_t),
                DIV = sizeof(nthp::script::stdVarWidth) + sizeof(nthp::script::stdVarWidth) + sizeof(uint32_t)



        );
}

#define GET_INSTRUCTION_SIZE(instruction) nthp::script::instructions::Size::instruction

}}}