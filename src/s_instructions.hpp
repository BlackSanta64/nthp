#pragma once
#include "s_node.hpp"

namespace nthp { namespace script { namespace instructions {


#define INSTRUCTION_LIST(...) typedef enum __inst {\
        __VA_ARGS__\
        } Instruction

#define INSTRUCTION_SIZE_LIST(...) constexpr uint32_t\
        __VA_ARGS__

#define ____INSTRUCTION_TOKENS(...) __VA_ARGS__


namespace ID {
        #define INSTRUCTION_TOKENS() ____INSTRUCTION_TOKENS(\
                HEADER,\
                EXIT,\
                LABEL,\
                GOTO,\
                JUMP,\
                SUSPEND,\
                RETURN,\
                GETINDEX,\
                INC,\
                DEC,\
                ADD,\
                SUB,\
                MUL,\
                DIV,\
                SQRT,\
                LOGIC_EQU,\
                LOGIC_NOT,\
                LOGIC_GRT,\
                LOGIC_LST,\
                LOGIC_GRTE,\
                LOGIC_LSTE,\
                END,\
                SET,\
                CLEAR,\
                DEFINE\
        )

        INSTRUCTION_LIST( INSTRUCTION_TOKENS(), numberOfInstructions);
}

#define GET_INSTRUCTION_ID(instruction) nthp::script::instructions::ID::instruction
typedef P_Reference<nthp::script::stdVarWidth> stdRef;

// Sizes must have the same name as the ENUM entry in 'ID'.
namespace Size {

        INSTRUCTION_SIZE_LIST(
                HEADER = 0,
                EXIT = 0,

                LABEL = sizeof(uint32_t),
                GOTO = sizeof(uint32_t),
                JUMP = sizeof(stdRef),
                SUSPEND = 0,
                RETURN = sizeof(stdRef),
                GETINDEX = sizeof(uint32_t),

                INC = sizeof(uint32_t),
                DEC = sizeof(uint32_t),

                ADD = sizeof(stdRef) + sizeof(stdRef) + sizeof(uint32_t),
                SUB = sizeof(stdRef) + sizeof(stdRef) + sizeof(uint32_t),
                MUL = sizeof(stdRef) + sizeof(stdRef) + sizeof(uint32_t),
                DIV = sizeof(stdRef) + sizeof(stdRef) + sizeof(uint32_t),
                SQRT = sizeof(stdRef) + sizeof(uint32_t),

                LOGIC_EQU = sizeof(stdRef) + sizeof(stdRef) + sizeof(uint32_t),
                LOGIC_NOT = sizeof(stdRef) + sizeof(stdRef) + sizeof(uint32_t),
                LOGIC_GRT = sizeof(stdRef) + sizeof(stdRef) + sizeof(uint32_t),
                LOGIC_LST = sizeof(stdRef) + sizeof(stdRef) + sizeof(uint32_t),
                LOGIC_GRTE = sizeof(stdRef) + sizeof(stdRef) + sizeof(uint32_t),
                LOGIC_LSTE = sizeof(stdRef) + sizeof(stdRef) + sizeof(uint32_t),

                END = 0,

                SET = sizeof(uint32_t) + sizeof(nthp::script::stdVarWidth),
                CLEAR = 0,
                DEFINE = sizeof(uint32_t)

        );
}


#define GET_INSTRUCTION_SIZE(instruction) nthp::script::instructions::Size::instruction

}}}