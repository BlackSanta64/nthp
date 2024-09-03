#include "s_compiler.hpp"

#define DEFINE_COMPILATION_BEHAVIOUR(instruction) int instruction (std::vector<nthp::script::Node>& nodeList,\
                                                                        std::fstream& file,\
                                                                        std::string& fileRead,\
                                                                        std::vector<nthp::script::CompilerInstance::CONST_DEF>& constantList,\
                                                                        std::vector<nthp::script::CompilerInstance::MACRO_DEF>& macroList,\
                                                                        std::vector<nthp::script::CompilerInstance::VAR_DEF>& varList,\
                                                                        std::vector<nthp::script::CompilerInstance::LABEL_DEF>& labelList,\
                                                                        std::vector<nthp::script::CompilerInstance::GOTO_DEF>& gotoList,\
                                                                        size_t& currentMacroPosition,\
                                                                        size_t& targetMacro,\
                                                                        bool& evaluateMacro\
                                                                        )

#define currentNode nodeList.size() - (decltype(nodeList.size()))1

// Adds node with corresponding instruction ID and allocates node memory.
#define ADD_NODE(instruction) nodeList.push_back(nthp::script::Node(GET_INSTRUCTION_ID(instruction), GET_INSTRUCTION_SIZE(instruction)));\
        if(GET_INSTRUCTION_SIZE(instruction) > 0) nodeList[currentNode].access.data = (char*)malloc(GET_INSTRUCTION_SIZE(instruction));\
        else nodeList[currentNode].access.data = NULL

#define VECT_END(vect) vect.size() - 1

#ifdef DEBUG

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

	fprintf(NTHP_debug_output, "[%u] COMPILER ERROR: ", SDL_GetTicks());	
	vfprintf(NTHP_debug_output, format, ap);


	va_end(ap);
}

void PRINT_COMPILER_WARNING(const char* format, ...) {
        va_list ap;
	
	va_start(ap, format);

	fprintf(NTHP_debug_output, "[%u] COMPILER WARNING: ", SDL_GetTicks());	
	vfprintf(NTHP_debug_output, format, ap);


	va_end(ap);
}

#endif




int EvaluateConst(std::string& expression, std::vector<nthp::script::CompilerInstance::CONST_DEF>& list) {
        if(expression[0] == '#') {

                for(size_t i = 0; i < list.size(); ++i) {
                        if(expression == list[i].constName) {
                                expression = list[i].value;
                                return 0;
                        }
                }

                PRINT_COMPILER_ERROR("Unable to evaluate CONST substitution [%s]; CONST not declared.\n", expression.c_str());
                return 1;
        }
        return 0;
}

int EvaluateMacro(std::string& expression, std::vector<nthp::script::CompilerInstance::MACRO_DEF>& list, size_t& mp, size_t& targetMacro, bool& beginMacroEval) {
        if(expression[0] == '@') {
                for(size_t i = 0; i < list.size(); ++i) {
                        if(expression == list[i].macroName) {
                                targetMacro = i;
                                expression = list[i].macroData[0];

                                mp = 0;
                                beginMacroEval = true;
                                
                                return 0;
                        }
                }
                
                PRINT_COMPILER_ERROR("Unable to evaluate MACRO substitution [%s]; MACRO not declared.\n", expression.c_str());
                return 1;
        }
       return 0;
}



// Generic for reading from source file. Checks for MACRO or CONST refs, and evaluates
// MACRO substitutions. Sets 'expression' to whatever the next valid symbol is; regardless if it's from a MACRO or the source file.
int EvaluateSymbol(std::fstream& file, std::string& expression, std::vector<nthp::script::CompilerInstance::CONST_DEF>& constList, std::vector<nthp::script::CompilerInstance::MACRO_DEF>& macroList, size_t& currentMacroPosition, size_t& targetMacro, bool& evaluatingMacro) {
        if(evaluatingMacro) {
                ++currentMacroPosition;

                PRINT_COMPILER("Eval. Arg ; %s\n", macroList[targetMacro].macroData[currentMacroPosition]);

                if(macroList[targetMacro].macroData[currentMacroPosition] == "}") {
                        evaluatingMacro = false;       
                }
                else {
                        expression = macroList[targetMacro].macroData[currentMacroPosition];
                        if(EvaluateConst(expression, constList)) return 1;

                        return 0;
                }
        }

        (file) >> (expression);
        if(EvaluateConst(expression, constList)) return 1;
        if(EvaluateMacro(expression, macroList, currentMacroPosition, targetMacro, evaluatingMacro)) return 1;
        return 0;
}

template<class T>
nthp::script::P_Reference<T> EvaluateReference(std::string& expression) {
        nthp::script::P_Reference<T> ref;
        
}



#define ____S_EVAL(...) if(EvaluateSymbol(__VA_ARGS__)) return 1
#define EVAL_SYMBOL() ____S_EVAL(file, fileRead, constantList, macroList, currentMacroPosition, targetMacro, evaluateMacro);

// DEFINE COMPILER BEHAVIOUR FOR EACH INSTRUCTION HERE ||
//                                                     VV


// The HEADER Contains the signature for a compiled script ID,
// as well as a list of all labels and their positions, and the memory requirement
// to execute the script. 
DEFINE_COMPILATION_BEHAVIOUR(HEADER) {
        ADD_NODE(HEADER);

        return 0;
}

DEFINE_COMPILATION_BEHAVIOUR(EXIT) {
        ADD_NODE(EXIT);

        return 0;
}


DEFINE_COMPILATION_BEHAVIOUR(LABEL) {
        PRINT_COMPILER("Evaluating LABEL Node...\n");
        ADD_NODE(LABEL);

        EVAL_SYMBOL(); // Label ID
        uint32_t* labelID = (decltype(labelID))nodeList[currentNode].access.data;

        try {
                *labelID = std::stoul(fileRead);
        }
        catch(std::invalid_argument) { 
                PRINT_COMPILER_ERROR("Unable to evaluate numeral at [%zu]; Invalid Argument.\n", currentNode);
                return 1; 
        }

        // Check for duplicate IDs.
        for(size_t i = 0; i < labelList.size(); ++i) {

                if(*labelID == labelList[i].ID) {
                        PRINT_COMPILER_ERROR("Unable to evaluate LABEL Node; Duplicate Label ID found at position [%u].\n", labelList[i].label_position);
                        return 1;
                }

        }

        // Add label to list.
        nthp::script::CompilerInstance::LABEL_DEF l_def;
        l_def.ID = *labelID;
        l_def.label_position = nodeList.size();

        labelList.push_back(l_def);
        PRINT_COMPILER("[%zu] LABEL Node evaluated: ID = %u\n", currentNode, *labelID);

        return 0;
}


DEFINE_COMPILATION_BEHAVIOUR(GOTO) {
        PRINT_COMPILER("Evaluating GOTO Node...\n");
        ADD_NODE(GOTO);

        EVAL_SYMBOL();
        uint32_t* labelID = (decltype(labelID))nodeList[currentNode].access.data;
        uint32_t static_label;

        try {
                static_label = std::stoul(fileRead);
        }
        catch(std::invalid_argument) {
                PRINT_COMPILER_ERROR("Unable to evaluate numeral at [%zu]; Invalid Argument.\n", currentNode);
                return 1; 
        }


        nthp::script::CompilerInstance::GOTO_DEF newDef;
        newDef.goto_position = currentNode;
        newDef.points_to = static_label;

        *labelID = static_label;

        gotoList.push_back(newDef);
        PRINT_COMPILER("[%zu] GOTO Node evaluated: ID = %u\n", currentNode, *labelID);

        return 0;
}


DEFINE_COMPILATION_BEHAVIOUR(SUSPEND) {
        ADD_NODE(SUSPEND);

        return 0;
}


DEFINE_COMPILATION_BEHAVIOUR(JUMP) {
        ADD_NODE(JUMP);
        PRINT_COMPILER("Evaluating JUMP Node...\n");

        EVAL_SYMBOL();
        nthp::script::P_Reference<uint32_t>* labelID = decltype(labelID)(nodeList[currentNode].access.data);
        uint32_t static_label;

        try {
                static_label = std::stoul(fileRead);
        }
        catch(std::invalid_argument) {
                PRINT_COMPILER_ERROR("Unable to evaluate numeral at [%zu]; Invalid Argument.\n", currentNode);
                return 1; 
        }

        labelID->value = static_label;
        PRINT_COMPILER("[%zu] JUMP Node evaluated: ID = %u\n", currentNode, *labelID);
}


DEFINE_COMPILATION_BEHAVIOUR(ADD) {
        ADD_NODE(ADD);
        PRINT_COMPILER("Evaluating ADD Node...\n");
        nthp::script::P_Reference<nthp::fixed_t>* operandA = decltype(operandA)(nodeList[currentNode].access.data);
        nthp::script::P_Reference<nthp::fixed_t>* operandB = decltype(operandA)(nodeList[currentNode].access.data + sizeof(nthp::script::P_Reference<nthp::fixed_t>));

        EVAL_SYMBOL();
        
        

}

// COMPILER INSTANCE BEHAVIOUR GOES HERE                ||
//                                                      VV



int nthp::script::CompilerInstance::compileSourceFile(const char* filename, const char* outputFile) {
        NOVERB_PRINT_COMPILER("\tCompiling Source File [%s]...\n", filename);
        
        std::fstream file(filename, std::ios::in);
        if(file.fail()) {
                PRINT_COMPILER_ERROR("Compilation failed; input source file not found.\n");
                return 1;
        }
        std::string fileRead;

        std::vector<nthp::script::CompilerInstance::CONST_DEF>  constantList;
        std::vector<nthp::script::CompilerInstance::MACRO_DEF>  macroList;
        std::vector<nthp::script::CompilerInstance::VAR_DEF>    varList;


        // Labels and Goto's are matched post-compilation.
        std::vector<nthp::script::CompilerInstance::LABEL_DEF>  labelList;
        std::vector<nthp::script::CompilerInstance::GOTO_DEF>   gotoList;


        std::vector<nthp::script::Node>                         nodeList;

        size_t currentMacroPosition = 0;
        size_t targetMacro = 0;
        bool evaluateMacro = false;


        #define COMPILE(instruction) if( instruction ( nodeList, file, fileRead, constantList, macroList, varList, labelList, gotoList, currentMacroPosition, targetMacro, evaluateMacro ) ) return 1
       

        bool operationOngoing = true;

        COMPILE(HEADER);

        while(operationOngoing) {

                EVAL_SYMBOL();
                if(fileRead == "EXIT") {
                        COMPILE(EXIT);
                        operationOngoing = false;
                }

                // Evaluate Compiler Definitions first.
                if(fileRead == "VAR") {
                        // Define a new variable.
                        EVAL_SYMBOL();
                        PRINT_COMPILER("Defined VAR [%s].\n", fileRead.c_str());

                        VAR_DEF newDef;
                        newDef.varName = fileRead;
                        newDef.relativeIndex = varList.size();

                        varList.push_back(newDef);
                }

                if(fileRead == "CONST") {
                        // Define new Const sub.

                        EVAL_SYMBOL();  // Name
                        CONST_DEF newDef;

                        fileRead = '#' + fileRead;
                        newDef.constName = fileRead;

                        EVAL_SYMBOL(); // Substitution
                        newDef.value = fileRead;

                        constantList.push_back(newDef);
                }

                if(fileRead == "MACRO") {
                        // Define new Macro.
                        EVAL_SYMBOL();          // Name
                        MACRO_DEF newDef;

                        PRINT_COMPILER("Defining new MACRO [%s]...", fileRead.c_str());
                        newDef.macroName = '@' + fileRead;

                        (file) >> fileRead;     // Gets rid of the '{'
                        for(size_t i = 0; fileRead != "}"; ++i) {
                                EVAL_SYMBOL();
                                newDef.macroData.push_back(fileRead);
                                
                                if((i % 5) == 0) { NOVERB_PRINT_COMPILER("\n\t"); }

                                NOVERB_PRINT_COMPILER("[%s]", fileRead.c_str());
                        }
                        NOVERB_PRINT_COMPILER("\n");

                        macroList.push_back(newDef);
                }

                

                if(fileRead == "LABEL") COMPILE(LABEL);
                if(fileRead == "GOTO")  COMPILE(GOTO);
                

        }

        NOVERB_PRINT_COMPILER("\tSuccessfully compiled source file [%s].\n", filename);
        file.close();


    
        size_t labelIndex = 0;

        // Match GOTOs to LABELs.
        for(size_t gotoIndex = 0; gotoIndex < gotoList.size(); ++gotoIndex) {
                for(labelIndex = 0; labelIndex < labelList.size(); ++labelIndex) {

                        if(gotoList[gotoIndex].points_to == labelList[labelIndex].ID) {

                                uint32_t* location = (decltype(location))nodeList[gotoList[gotoIndex].goto_position].access.data;
                                *location = labelList[labelIndex].label_position;
                                break;
                        }
                        
                }
                if(labelIndex == labelList.size()) {
                        PRINT_DEBUG_WARNING("Failed to link GOTO [%zu] to LABEL block. Broken GOTO created.", gotoIndex);
                        uint32_t* location = (decltype(location))nodeList[gotoList[gotoIndex].goto_position].access.data;

                        *location = gotoList[gotoIndex].goto_position;
                }
        }
        


        return 0;

}