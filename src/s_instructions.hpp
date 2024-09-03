#pragma once
#include "s_node.hpp"

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
                JUMP,
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
typedef uint32_t standard_VarRef_type;

namespace Size {

        INSTRUCTION_SIZE_LIST(
                HEADER = 0,
                EXIT = 0,

                LABEL = sizeof(uint32_t),
                GOTO = sizeof(uint32_t),
                JUMP = sizeof(P_Reference<uint32_t>),
                SUSPEND = 0,

                INC = sizeof(uint32_t),
                DEC = sizeof(uint32_t),

                ADD = sizeof(P_Reference<nthp::script::stdVarWidth>) + sizeof(P_Reference<nthp::script::stdVarWidth>) + sizeof(uint32_t),
                SUB = sizeof(P_Reference<nthp::script::stdVarWidth>) + sizeof(P_Reference<nthp::script::stdVarWidth>) + sizeof(uint32_t),
                MUL = sizeof(P_Reference<nthp::script::stdVarWidth>) + sizeof(P_Reference<nthp::script::stdVarWidth>) + sizeof(uint32_t),
                DIV = sizeof(P_Reference<nthp::script::stdVarWidth>) + sizeof(P_Reference<nthp::script::stdVarWidth>) + sizeof(uint32_t)



        );
}

#define GET_INSTRUCTION_SIZE(instruction) nthp::script::instructions::Size::instruction

}}}