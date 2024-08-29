#include "s_compiler.hpp"

#define DEFINE_COMPILATION_BEHAVIOUR(instruction) void instruction (std::vector<nthp::script::Node>* workingNodeBlock, std::fstream* file)

#define ADD_NODE(instruction) workingNodeBlock->push_back(nthp::script::Node(GET_INSTRUCTION_ID(instruction), GET_INSTRUCTION_SIZE(instruction)));\
        workingNodeBlock->data()[currentNode].access.data = (char*)malloc(GET_INSTRUCTION_SIZE(instruction))

#define currentNode workingNodeBlock->size() - 1


void PRINT_COMPILER(const char* format, ...) {
        va_list ap;
	
	va_start(ap, format);

	fprintf(NTHP_debug_output, "[%u] COMPILER: ", SDL_GetTicks());	
	vfprintf(NTHP_debug_output, format, ap);


	va_end(ap);
}

void PRINT_COMPILER_ERROR(const char* format, ...) {
        va_list ap;
	
	va_start(ap, format);

	fprintf(NTHP_debug_output, "[%u] COMP. ERROR: ", SDL_GetTicks());	
	vfprintf(NTHP_debug_output, format, ap);


	va_end(ap);
}

void PRINT_COMPILER_WARNING(const char* format, ...) {
        va_list ap;
	
	va_start(ap, format);

	fprintf(NTHP_debug_output, "[%u] COMP. WARNING: ", SDL_GetTicks());	
	vfprintf(NTHP_debug_output, format, ap);


	va_end(ap);
}








// DEFINE COMPILER BEHAVIOUR FOR EACH INSTRUCTION HERE ||
//                                                     VV


DEFINE_COMPILATION_BEHAVIOUR(HEADER) {
        ADD_NODE(HEADER);

}











// COMPILER INSTANCE BEHAVIOUR GOES HERE                ||
//                                                     VV



int nthp::script::CompilerInstance::compileSourceFile(const char* filename, const char* outputFile) {
        
        std::fstream file(filename, std::ios::in);
        if(file.fail()) {
                PRINT_COMPILER_ERROR("Compilation failed; input source file not found.\n");
                return -1;
        }



        std::vector<nthp::script::CompilerInstance::CONST_DEF>  constantList;
        std::vector<nthp::script::CompilerInstance::MACRO_DEF>  macroList;
        std::vector<nthp::script::CompilerInstance::VAR_DEF>    varList;


        // Labels and Goto's are matched post-compilation.
        std::vector<nthp::script::CompilerInstance::LABEL_DEF>  labelList;
        std::vector<nthp::script::CompilerInstance::GOTO_DEF>   gotoList;


        std::vector<nthp::script::Node>                         nodeList;
        #define COMPILE(instruction) instruction ( &nodeList, &file )


        bool operationOngoing = true;

        while(operationOngoing) {






        }
}