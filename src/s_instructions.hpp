#pragma once
#include "s_node.hpp"

typedef enum {
        MOUSEPOS_X_GLOBAL_INDEX,
        MOUSEPOS_Y_GLOBAL_INDEX,
        DELTATIME_GLOBAL_INDEX,
        MOUSE1_GLOBAL_INDEX,
        MOUSE2_GLOBAL_INDEX,
        MOUSE3_GLOBAL_INDEX,
        RPOLL1_GLOBAL_INDEX,
        RPOLL2_GLOBAL_INDEX,
        RPOLL3_GLOBAL_INDEX,
        RPOLL4_GLOBAL_INDEX,

        COUNT_PREDEFINED_GLOBALS
} predefined_globals;

namespace nthp { namespace script { namespace instructions {


#define INSTRUCTION_LIST(...) typedef enum __inst {\
        __VA_ARGS__\
        } Instruction

#define INSTRUCTION_SIZE_LIST(...) constexpr decltype(nthp::script::Node::n_binary_t::size)\
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
                LOGIC_IF_TRUE,\
                LOGIC_EQU,\
                LOGIC_NOT,\
                LOGIC_GRT,\
                LOGIC_LST,\
                LOGIC_GRTE,\
                LOGIC_LSTE,\
                END,\
                SET,\
                SET_BINARY,\
                CLEAR,\
                DEFINE,\
                COPY,\
		TEXTURE_DEFINE,\
		TEXTURE_CLEAR,\
		TEXTURE_LOAD,\
		SET_ACTIVE_PALETTE,\
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
                ENT_SETRENDERSIZE,\
                ENT_CHECKCOLLISION,\
                CORE_INIT,\
                CORE_QRENDER,\
                CORE_ABS_QRENDER,\
                CORE_CLEAR,\
                CORE_DISPLAY,\
                CORE_SETMAXFPS,\
                CORE_SETWINDOWRES,\
                CORE_SETCAMERARES,\
                CORE_SETCAMERAPOSITION,\
                CORE_MOVECAMERA,\
                CORE_STOP,\
                ACTION_DEFINE,\
                ACTION_BIND,\
                ACTION_CLEAR,\
                STAGE_LOAD,\
                POLL_ENT_POSITION,\
                POLL_ENT_HITBOX,\
                POLL_ENT_RENDERSIZE,\
                POLL_ENT_CURRENTFRAME,\
                DRAW_SETCOLOR,\
                DRAW_LINE\
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


                LOGIC_IF_TRUE = sizeof(stdRef) + sizeof(uint32_t),
                LOGIC_EQU = sizeof(stdRef) + sizeof(stdRef) + sizeof(uint32_t),
                LOGIC_NOT = sizeof(stdRef) + sizeof(stdRef) + sizeof(uint32_t),
                LOGIC_GRT = sizeof(stdRef) + sizeof(stdRef) + sizeof(uint32_t),
                LOGIC_LST = sizeof(stdRef) + sizeof(stdRef) + sizeof(uint32_t),
                LOGIC_GRTE = sizeof(stdRef) + sizeof(stdRef) + sizeof(uint32_t),
                LOGIC_LSTE = sizeof(stdRef) + sizeof(stdRef) + sizeof(uint32_t),

                END = 0,

                SET = sizeof(indRef) + sizeof(nthp::script::stdVarWidth),
                SET_BINARY = sizeof(indRef) + sizeof(nthp::script::stdVarWidth),
                CLEAR = 0,
                DEFINE = sizeof(uint32_t),
                COPY = sizeof(indRef) + sizeof(indRef),

		TEXTURE_DEFINE = sizeof(stdRef),
		TEXTURE_CLEAR = 0,
		TEXTURE_LOAD = DYNAMIC_SIZE, 
		SET_ACTIVE_PALETTE = DYNAMIC_SIZE, 


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
                ENT_SETRENDERSIZE = sizeof(stdRef) + sizeof(stdRef) + sizeof(stdRef),
                ENT_CHECKCOLLISION = sizeof(stdRef) + sizeof(stdRef) + sizeof(indRef),


                CORE_INIT = DYNAMIC_SIZE,
                CORE_QRENDER = sizeof(stdRef),
                CORE_ABS_QRENDER = sizeof(stdRef),
                CORE_CLEAR = 0,
                CORE_DISPLAY = 0,
                CORE_SETMAXFPS = sizeof(stdRef),
                CORE_SETWINDOWRES = sizeof(stdRef) + sizeof(stdRef),
                CORE_SETCAMERARES = sizeof(stdRef) + sizeof(stdRef),
                CORE_SETCAMERAPOSITION = sizeof(stdRef) + sizeof(stdRef),
                CORE_MOVECAMERA = sizeof(stdRef) + sizeof(stdRef),
                CORE_STOP = 0,

                ACTION_DEFINE = sizeof(stdRef),
                ACTION_CLEAR = 0, //
                ACTION_BIND =  sizeof(stdRef) + sizeof(uint32_t) + sizeof(int32_t), // actionIndex, varIndex, key
                STAGE_LOAD = DYNAMIC_SIZE,

                POLL_ENT_POSITION = sizeof(stdRef),
                POLL_ENT_HITBOX = sizeof(stdRef),
                POLL_ENT_RENDERSIZE = sizeof(stdRef),
                POLL_ENT_CURRENTFRAME = sizeof(stdRef),

                DRAW_SETCOLOR = sizeof(stdRef),
                DRAW_LINE = sizeof(stdRef) + sizeof(stdRef) + sizeof(stdRef) + sizeof(stdRef)
        );
}


#define GET_INSTRUCTION_SIZE(instruction) nthp::script::instructions::Size::instruction

}}}
