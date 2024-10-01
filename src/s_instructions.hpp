#pragma once
#include "s_node.hpp"

namespace nthp { namespace script { namespace instructions {


#define INSTRUCTION_LIST(...) typedef enum __inst {\
        __VA_ARGS__\
        } Instruction

#define INSTRUCTION_SIZE_LIST(...) constexpr uint8_t\
        __VA_ARGS__

#define ____INSTRUCTION_TOKENS(...) __VA_ARGS__

#define         DYNAMIC_SIZE    0
#define         NTHP_CORE_INIT_SOFTWARE_RENDERING    1
#define         NTHP_CORE_INIT_FULLSCREEN            0

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
                LSHIFT,\
                RSHIFT,\
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
                DEFINE,\
                COPY,\
		\
		TEXTURE_DEFINE,\
		TEXTURE_CLEAR,\
		TEXTURE_LOAD,\
		SET_ACTIVE_PALETTE,\
                CORE_INIT,\
                FRAME_DEFINE,\
                FRAME_CLEAR,\
                FRAME_SET,\
                GETGPR,\
                SM_READ,\
                SM_WRITE,\
                ENT_DEFINE,\
                ENT_CLEAR,\
                ENT_SETCURRENTFRAME,\
                ENT_SETPOS,\
                ENT_MOVE,\
                ENT_SETFRAMERANGE,\
                ENT_SETHITBOXSIZE,\
                ENT_SETHITBOXOFFSET,\
                ENT_SETRENDERSIZE\
        )

        INSTRUCTION_LIST( INSTRUCTION_TOKENS(), numberOfInstructions);
}

#define GET_INSTRUCTION_ID(instruction) nthp::script::instructions::ID::instruction
typedef P_Reference<nthp::script::stdVarWidth> stdRef;
typedef P_Reference<uint32_t> indRef;


// Sizes must have the same name as the ENUM entry in 'ID'.
namespace Size {

        INSTRUCTION_SIZE_LIST(
                HEADER = DYNAMIC_SIZE,
                EXIT = 0,

                LABEL = sizeof(uint32_t),
                GOTO = sizeof(uint32_t),
                JUMP = sizeof(stdRef),
                SUSPEND = 0,
                RETURN = sizeof(stdRef),
                GETINDEX = sizeof(indRef),

                INC = sizeof(indRef),
                DEC = sizeof(indRef),
                RSHIFT = sizeof(indRef) + sizeof(stdRef),
                LSHIFT = sizeof(indRef) + sizeof(stdRef),

                ADD = sizeof(stdRef) + sizeof(stdRef) + sizeof(indRef),
                SUB = sizeof(stdRef) + sizeof(stdRef) + sizeof(indRef),
                MUL = sizeof(stdRef) + sizeof(stdRef) + sizeof(indRef),
                DIV = sizeof(stdRef) + sizeof(stdRef) + sizeof(indRef),
                SQRT = sizeof(stdRef) + sizeof(indRef),

                LOGIC_EQU = sizeof(stdRef) + sizeof(stdRef) + sizeof(uint32_t),
                LOGIC_NOT = sizeof(stdRef) + sizeof(stdRef) + sizeof(uint32_t),
                LOGIC_GRT = sizeof(stdRef) + sizeof(stdRef) + sizeof(uint32_t),
                LOGIC_LST = sizeof(stdRef) + sizeof(stdRef) + sizeof(uint32_t),
                LOGIC_GRTE = sizeof(stdRef) + sizeof(stdRef) + sizeof(uint32_t),
                LOGIC_LSTE = sizeof(stdRef) + sizeof(stdRef) + sizeof(uint32_t),

                END = 0,

                SET = sizeof(indRef) + sizeof(nthp::script::stdVarWidth),
                CLEAR = 0,
                DEFINE = sizeof(uint32_t),
                COPY = sizeof(indRef) + sizeof(indRef),

		TEXTURE_DEFINE = sizeof(stdRef),
		TEXTURE_CLEAR = 0,
		TEXTURE_LOAD = DYNAMIC_SIZE, 
		SET_ACTIVE_PALETTE = DYNAMIC_SIZE, 
                
                CORE_INIT = DYNAMIC_SIZE,

                FRAME_DEFINE = sizeof(stdRef),
                FRAME_CLEAR = 0,
                FRAME_SET = sizeof(stdRef) + sizeof(stdRef) + sizeof(stdRef) + sizeof(stdRef) + sizeof(stdRef) + sizeof(stdRef),
                GETGPR = sizeof(indRef),
                SM_WRITE = sizeof(stdRef) + sizeof(stdRef),
                SM_READ = sizeof(stdRef) + sizeof(indRef),

                ENT_DEFINE = sizeof(stdRef),
                ENT_CLEAR = 0,
                ENT_SETCURRENTFRAME = sizeof(stdRef) + sizeof(stdRef),
                ENT_SETPOS = sizeof(stdRef) + sizeof(stdRef) + sizeof(stdRef),
                ENT_MOVE = sizeof(stdRef) + sizeof(stdRef) + sizeof(stdRef),
                ENT_SETFRAMERANGE = sizeof(stdRef) + sizeof(stdRef) + sizeof(stdRef),
                ENT_SETHITBOXSIZE = sizeof(stdRef) + sizeof(stdRef) + sizeof(stdRef),
                ENT_SETHITBOXOFFSET = sizeof(stdRef) + sizeof(stdRef) + sizeof(stdRef),
                ENT_SETRENDERSIZE = sizeof(stdRef) + sizeof(stdRef) + sizeof(stdRef) 

        );
}


#define GET_INSTRUCTION_SIZE(instruction) nthp::script::instructions::Size::instruction

}}}
