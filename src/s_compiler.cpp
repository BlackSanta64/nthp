#include "s_compiler.hpp"

// I love this
#define DEFINE_COMPILATION_BEHAVIOUR(instruction) int instruction (std::vector<nthp::script::Node>& nodeList,\
                                                                        std::fstream& file,\
                                                                        std::string& fileRead,\
                                                                        std::vector<nthp::script::CompilerInstance::CONST_DEF>& constantList,\
                                                                        std::vector<nthp::script::CompilerInstance::MACRO_DEF>& macroList,\
                                                                        std::vector<nthp::script::CompilerInstance::VAR_DEF>& varList,\
                                                                        std::vector<nthp::script::CompilerInstance::LABEL_DEF>& labelList,\
                                                                        std::vector<nthp::script::CompilerInstance::GOTO_DEF>& gotoList,\
                                                                        std::vector<size_t>& ifList,\
                                                                        std::vector<size_t>& endList,\
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

	fprintf(NTHP_debug_output, "[%u] ERROR: ", SDL_GetTicks());	
	vfprintf(NTHP_debug_output, format, ap);


	va_end(ap);
}

void PRINT_COMPILER_WARNING(const char* format, ...) {
        va_list ap;
	
	va_start(ap, format);

	fprintf(NTHP_debug_output, "[%u] WARNING: ", SDL_GetTicks());	
	vfprintf(NTHP_debug_output, format, ap);


	va_end(ap);
}

#endif


#define PRINT_NODEDATA() NOVERB_PRINT_COMPILER("[%zu] [%p] Node ID: %u ; s=%u d[%p]\n", currentNode, (nodeList.data() + (currentNode)), nodeList[currentNode].access.ID, nodeList[currentNode].access.size, nodeList[currentNode].access.data) 


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
                                PRINT_COMPILER("Beginning Expansion of Macro [%s]...\n", list[targetMacro].macroName.c_str());
                                
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

                NOVERB_PRINT_COMPILER("Eval. Arg ; %s\n", macroList[targetMacro].macroData[currentMacroPosition].c_str());

                if(macroList[targetMacro].macroData[currentMacroPosition] == "}") {
                        evaluatingMacro = false;
                        NOVERB_PRINT_COMPILER("\tCompleted expansion of macro [%s].\n", macroList[targetMacro].macroName.c_str());
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


// Substitues a VAR reference or parses numeral references (for compatibility)
nthp::script::instructions::stdRef EvaluateReference(std::string expression, std::vector<nthp::script::CompilerInstance::VAR_DEF>& list) {
        NOVERB_PRINT_COMPILER("Parsing Potential Reference (P_Ref) [%s]...\n", expression.c_str());
        nthp::script::P_Reference<nthp::fixed_t> ref;
        ref.metadata = 0;

        if(expression[0] == '$') {
                PR_METADATA_SET         (ref, nthp::script::flagBits::IS_REFERENCE);
                PR_METADATA_CLEAR       (ref, nthp::script::flagBits::IS_GLOBAL);

                expression.erase(expression.begin());
        }
        else {
                if(expression[0] == '>') {
                        PR_METADATA_SET (ref, nthp::script::flagBits::IS_REFERENCE);
                        PR_METADATA_SET (ref, nthp::script::flagBits::IS_GLOBAL);

                        expression.erase(expression.begin());
                }
        }

        // Evaluate Var.
        // If no VARNAME is referenced, assumes numeral reference type (instead of $VARNAME or >VARNAME, $2 or >2), or constant. Throws
        // Invalid argument if otherwise.
        if(PR_METADATA_GET(ref, nthp::script::flagBits::IS_REFERENCE)) {
                for(size_t i = 0; i < list.size(); ++i) {
                        if(expression == list[i].varName) {
                                expression = std::to_string(list[i].relativeIndex);
                        }
                }
        }




        try {
                double value = std::stod(expression);
                ref.value = nthp::doubleToFixed(value);

                // A reference cannot have a decimal value.
                if(PR_METADATA_GET(ref, nthp::script::flagBits::IS_REFERENCE)) {
                        if(nthp::getFixedDecimal(ref.value) > 0) {
                                PRINT_COMPILER_WARNING("Reference [%s] is invalid; Decimal not permitted, discarding decimal.\n", expression.c_str());
                        }

                        ref.value = nthp::getFixedInteger(ref.value);
                }
                else {
                        ref.value = nthp::doubleToFixed(value);
                }
        }
        catch(std::invalid_argument) {
                PRINT_COMPILER_ERROR("Unable to evaluate reference [%s]; Invalid Argument.\n", expression.c_str());
                return ref;
        }
 
        PR_METADATA_SET(ref, nthp::script::flagBits::IS_VALID);
        NOVERB_PRINT_COMPILER("Evaluated Reference [%s]: Value = %llu, IR = %u, IG = %u\n", expression.c_str(), ref.value, PR_METADATA_GET(ref, nthp::script::flagBits::IS_REFERENCE), PR_METADATA_GET(ref, nthp::script::flagBits::IS_GLOBAL));
        
        return ref; 
}



#define ____S_EVAL(...) if(EvaluateSymbol(__VA_ARGS__)) return 1

// Generic conviencence macro to evaluate the next symbol in the stream. Automatically pulls the next
// symbol from a macro or source file into 'fileRead'
#define EVAL_SYMBOL() ____S_EVAL(file, fileRead, constantList, macroList, currentMacroPosition, targetMacro, evaluateMacro);

#define CHECK_REF(ref) if(!PR_METADATA_GET(ref, nthp::script::flagBits::IS_VALID)) return 1 

// DEFINE COMPILER BEHAVIOUR FOR EACH INSTRUCTION HERE ||
//                                                     VV

















// The HEADER Contains the signature for a compiled script ID,
// as well as a list of all labels and their positions, and the memory requirement
// to execute the script. 
DEFINE_COMPILATION_BEHAVIOUR(HEADER) {
        ADD_NODE(HEADER);

        PRINT_NODEDATA();
        return 0;
}

DEFINE_COMPILATION_BEHAVIOUR(EXIT) {
        ADD_NODE(EXIT);


        PRINT_NODEDATA();
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
        NOVERB_PRINT_COMPILER("[%zu] LABEL Node evaluated: ID = %u\n", currentNode, *labelID);

        PRINT_NODEDATA();
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
        NOVERB_PRINT_COMPILER("[%zu] GOTO Node evaluated: ID = %u\n", currentNode, *labelID);
        PRINT_NODEDATA();
        return 0;      
}


DEFINE_COMPILATION_BEHAVIOUR(SUSPEND) {
        ADD_NODE(SUSPEND);

        PRINT_NODEDATA();
        return 0;
}


DEFINE_COMPILATION_BEHAVIOUR(JUMP) {
        ADD_NODE(JUMP);
        NOVERB_PRINT_COMPILER("Evaluating JUMP Node...\n");

        EVAL_SYMBOL();
        nthp::script::P_Reference<nthp::script::stdVarWidth>* labelID = decltype(labelID)(nodeList[currentNode].access.data);
        auto static_label = EvaluateReference(fileRead, varList);

        CHECK_REF(static_label);
        if(!PR_METADATA_GET(static_label, nthp::script::flagBits::IS_REFERENCE)) {
                PRINT_COMPILER_WARNING("JUMP Node at [%zu] uses constant expression as label reference; Consider using GOTO instead.\n", currentNode);
        }



        *labelID = static_label;
        NOVERB_PRINT_COMPILER("[%zu] JUMP Node evaluated: ID = %lld\n", currentNode, nthp::fixedToInt(labelID->value));

        PRINT_NODEDATA();
        return 0;
}

DEFINE_COMPILATION_BEHAVIOUR(RETURN) {
        ADD_NODE(RETURN);

        nthp::script::instructions::stdRef* instruction = decltype(instruction)(nodeList[currentNode].access.data);
        EVAL_SYMBOL();

        auto static_ref = EvaluateReference(fileRead, varList);
        CHECK_REF(static_ref);

        *instruction = static_ref;

        PRINT_NODEDATA();
        return 0;
}


DEFINE_COMPILATION_BEHAVIOUR(GETINDEX) {
        ADD_NODE(GETINDEX);


        uint32_t* pointer = decltype(pointer)(nodeList[currentNode].access.data);
        
        auto static_ref = EvaluateReference(fileRead, varList);
        CHECK_REF(static_ref);

        if(!PR_METADATA_GET(static_ref, nthp::script::flagBits::IS_REFERENCE)) {
                PRINT_COMPILER_ERROR("GETINDEX Argument must reference.\n");
                return 1;
        }

        *pointer = nthp::fixedToInt(static_ref.value);

        PRINT_NODEDATA();
        return 0;
}


DEFINE_COMPILATION_BEHAVIOUR(INC) {
        ADD_NODE(INC);
        NOVERB_PRINT_COMPILER("Evaluating INC Node...\n");

        EVAL_SYMBOL();
        uint32_t* var = decltype(var)(nodeList[currentNode].access.data);
        auto static_var = EvaluateReference(fileRead, varList);

        CHECK_REF(static_var);

        if(!PR_METADATA_GET(static_var, nthp::script::flagBits::IS_REFERENCE)) {
                PRINT_COMPILER_ERROR("Unable to evaluate Reference; INC must take reference.\n");
                return 1;
        }
        *var = (uint32_t)nthp::fixedToInt(static_var.value);
      
        PRINT_NODEDATA();
        return 0;
}

DEFINE_COMPILATION_BEHAVIOUR(DEC) {
        ADD_NODE(DEC);
        NOVERB_PRINT_COMPILER("Evaluating DEC Node...\n");

        EVAL_SYMBOL();
        uint32_t* var = decltype(var)(nodeList[currentNode].access.data);
        auto static_var = EvaluateReference(fileRead, varList);

        CHECK_REF(static_var);

        if(!PR_METADATA_GET(static_var, nthp::script::flagBits::IS_REFERENCE)) {
                PRINT_COMPILER_ERROR("Unable to evaluate Reference; INC must take reference.\n");
                return 1;
        }
        *var = (uint32_t)nthp::fixedToInt(static_var.value);
      
        PRINT_NODEDATA();
        return 0;
}



DEFINE_COMPILATION_BEHAVIOUR(ADD) {
        ADD_NODE(ADD);

        NOVERB_PRINT_COMPILER("Evaluating ADD Node...\n");
        nthp::script::instructions::stdRef* operandA = decltype(operandA)(nodeList[currentNode].access.data);
        nthp::script::instructions::stdRef* operandB = decltype(operandB)(nodeList[currentNode].access.data + sizeof(nthp::script::instructions::stdRef));
        uint32_t* output = decltype(output)(nodeList[currentNode].access.data + sizeof(nthp::script::instructions::stdRef) + sizeof(nthp::script::instructions::stdRef));

        EVAL_SYMBOL();
        auto static_op_A = EvaluateReference(fileRead, varList);
        CHECK_REF(static_op_A);

        EVAL_SYMBOL();
        auto static_op_B = EvaluateReference(fileRead, varList);
        CHECK_REF(static_op_B);

        EVAL_SYMBOL();
        auto static_output = EvaluateReference(fileRead, varList);
        CHECK_REF(static_output);


        if(!PR_METADATA_GET(static_output, nthp::script::flagBits::IS_REFERENCE)) {
                PRINT_COMPILER_ERROR("Final Operand of ADD (arg.ADD_OUTPUT) must be a reference.\n");
                return 1;
        }


        *operandA = static_op_A;
        *operandB = static_op_B;
        *output = (uint32_t)nthp::fixedToInt(static_output.value);;

        NOVERB_PRINT_COMPILER("[%zu] ADD Node evaluated: OpA = %lf OpB = %lf Output = %u\n", currentNode, nthp::fixedToDouble(static_op_A.value), nthp::fixedToDouble(static_op_B.value), *output);
        
        PRINT_NODEDATA();
        return 0;
}

DEFINE_COMPILATION_BEHAVIOUR(SUB) {
        ADD_NODE(SUB);

        NOVERB_PRINT_COMPILER("Evaluating SUB Node...\n");
        nthp::script::instructions::stdRef* operandA = decltype(operandA)(nodeList[currentNode].access.data);
        nthp::script::instructions::stdRef* operandB = decltype(operandB)(nodeList[currentNode].access.data + sizeof(nthp::script::instructions::stdRef));
        uint32_t* output = decltype(output)(nodeList[currentNode].access.data + sizeof(nthp::script::instructions::stdRef) + sizeof(nthp::script::instructions::stdRef));

        EVAL_SYMBOL();
        auto static_op_A = EvaluateReference(fileRead, varList);
        CHECK_REF(static_op_A);

        EVAL_SYMBOL();
        auto static_op_B = EvaluateReference(fileRead, varList);
        CHECK_REF(static_op_B);

        EVAL_SYMBOL();
        auto static_output = EvaluateReference(fileRead, varList);
        CHECK_REF(static_output);


        if(!PR_METADATA_GET(static_output, nthp::script::flagBits::IS_REFERENCE)) {
                PRINT_COMPILER_ERROR("Final Operand of SUB (arg.SUB_OUTPUT) must be a reference.\n");
                return 1;
        }


        *operandA = static_op_A;
        *operandB = static_op_B;
        *output = (uint32_t)nthp::fixedToInt(static_output.value);;

        NOVERB_PRINT_COMPILER("[%zu] SUB Node evaluated: OpA = %lf OpB = %lf Output = %u\n", currentNode, nthp::fixedToDouble(static_op_A.value), nthp::fixedToDouble(static_op_B.value), *output);
        PRINT_NODEDATA();
        return 0;
}

DEFINE_COMPILATION_BEHAVIOUR(MUL) {
        ADD_NODE(MUL);

        NOVERB_PRINT_COMPILER("Evaluating MUL Node...\n");
        nthp::script::instructions::stdRef* operandA = decltype(operandA)(nodeList[currentNode].access.data);
        nthp::script::instructions::stdRef* operandB = decltype(operandB)(nodeList[currentNode].access.data + sizeof(nthp::script::instructions::stdRef));
        uint32_t* output = decltype(output)(nodeList[currentNode].access.data + sizeof(nthp::script::instructions::stdRef) + sizeof(nthp::script::instructions::stdRef));

        EVAL_SYMBOL();
        auto static_op_A = EvaluateReference(fileRead, varList);
        CHECK_REF(static_op_A);

        EVAL_SYMBOL();
        auto static_op_B = EvaluateReference(fileRead, varList);
        CHECK_REF(static_op_B);

        EVAL_SYMBOL();
        auto static_output = EvaluateReference(fileRead, varList);
        CHECK_REF(static_output);


        if(!PR_METADATA_GET(static_output, nthp::script::flagBits::IS_REFERENCE)) {
                PRINT_COMPILER_ERROR("Final Operand of MUL (arg.MUL_OUTPUT) must be a reference.\n");
                return 1;
        }


        *operandA = static_op_A;
        *operandB = static_op_B;
        *output = (uint32_t)nthp::fixedToInt(static_output.value);;

        NOVERB_PRINT_COMPILER("[%zu] MUL Node evaluated: OpA = %lf OpB = %lf Output = %u\n", currentNode, nthp::fixedToDouble(static_op_A.value), nthp::fixedToDouble(static_op_B.value), *output);


        PRINT_NODEDATA();
        return 0;
}

DEFINE_COMPILATION_BEHAVIOUR(DIV) {
        ADD_NODE(DIV);

        NOVERB_PRINT_COMPILER("Evaluating DIV Node...\n");
        nthp::script::instructions::stdRef* operandA = decltype(operandA)(nodeList[currentNode].access.data);
        nthp::script::instructions::stdRef* operandB = decltype(operandB)(nodeList[currentNode].access.data + sizeof(nthp::script::instructions::stdRef));
        uint32_t* output = decltype(output)(nodeList[currentNode].access.data + sizeof(nthp::script::instructions::stdRef) + sizeof(nthp::script::instructions::stdRef));

        EVAL_SYMBOL();
        auto static_op_A = EvaluateReference(fileRead, varList);
        CHECK_REF(static_op_A);

        EVAL_SYMBOL();
        auto static_op_B = EvaluateReference(fileRead, varList);
        CHECK_REF(static_op_B);

        EVAL_SYMBOL();
        auto static_output = EvaluateReference(fileRead, varList);
        CHECK_REF(static_output);


        if(!PR_METADATA_GET(static_output, nthp::script::flagBits::IS_REFERENCE)) {
                PRINT_COMPILER_ERROR("Final Operand of DIV (arg.DIV_OUTPUT) must be a reference.\n");
                return 1;
        }


        *operandA = static_op_A;
        *operandB = static_op_B;
        *output = (uint32_t)nthp::fixedToInt(static_output.value);;

        NOVERB_PRINT_COMPILER("[%zu] DIV Node evaluated: OpA = %lf OpB = %lf Output = %u\n", currentNode, nthp::fixedToDouble(static_op_A.value), nthp::fixedToDouble(static_op_B.value), *output);
     
        PRINT_NODEDATA();
        return 0;
}



DEFINE_COMPILATION_BEHAVIOUR(END) {
        ADD_NODE(END);
        endList.push_back(currentNode);

        PRINT_NODEDATA();
        return 0;
}


DEFINE_COMPILATION_BEHAVIOUR(IF) {

        nthp::script::instructions::stdRef static_opA;
        nthp::script::instructions::stdRef static_opB;

        EVAL_SYMBOL();
        static_opA = EvaluateReference(fileRead, varList);
        CHECK_REF(static_opA);

        // God this is wonderful. I love C++
        EVAL_SYMBOL();
        if (fileRead == "EQU") {
                ADD_NODE(LOGIC_EQU);
        }
        else 
        if (fileRead == "NOT") {
                ADD_NODE(LOGIC_NOT);
        }
        else 
        if (fileRead == "GRT") {
                ADD_NODE(LOGIC_GRT);
        }
        else 
        if (fileRead == "LST") {
                ADD_NODE(LOGIC_LST);
        }
        else 
        if (fileRead == "GRTE") {
                ADD_NODE(LOGIC_GRTE);
        }
        else 
        if (fileRead == "LSTE") {
                ADD_NODE(LOGIC_LSTE);
        }
        else {
                PRINT_COMPILER_ERROR("Invalid Comparison Operator [%s]; Valid operators: EQU NOT, GRT, LST, GRTE, LSTE\n", fileRead.c_str());
                return 1;
        }

        EVAL_SYMBOL();
        static_opB = EvaluateReference(fileRead, varList);
        CHECK_REF(static_opB);

        nthp::script::instructions::stdRef* opA = (decltype(opA))(nodeList[currentNode].access.data);
        nthp::script::instructions::stdRef* opB = (decltype(opB))(nodeList[currentNode].access.data + sizeof(nthp::script::instructions::stdRef));

        *opA = static_opA;
        *opB = static_opB;

        // End location (final argument) is parsed post-compilation. Add to list to speed up process.
        ifList.push_back(currentNode);

        PRINT_NODEDATA();
        return 0;
}



DEFINE_COMPILATION_BEHAVIOUR(CLEAR) {
        ADD_NODE(CLEAR);


        PRINT_NODEDATA();
        return 0;
}

DEFINE_COMPILATION_BEHAVIOUR(SET) {
        ADD_NODE(SET);


        uint32_t* pointer = decltype(pointer)(nodeList[currentNode].access.data);
        nthp::fixed_t* value = decltype(value)(nodeList[currentNode].access.data + sizeof(uint32_t));

        EVAL_SYMBOL(); // pointer
        auto ref = EvaluateReference(fileRead, varList);

        CHECK_REF(ref);
        if(!PR_METADATA_GET(ref, nthp::script::flagBits::IS_REFERENCE)) {
                PRINT_COMPILER_ERROR("First SET argument must be a reference. Syntax: SET $var value\n");
                return 1;
        }

        EVAL_SYMBOL(); // value
        nthp::fixed_t static_value;

        try {
                double conv = std::stod(fileRead);
                static_value = nthp::doubleToFixed(conv);
        }
        catch(std::invalid_argument) {
                PRINT_COMPILER_ERROR("Unable to evaluate numeral at [%zu]; Invalid Argument.\n", currentNode);
                return 1;
        }

        *pointer = nthp::fixedToInt(ref.value);
        *value = static_value;

        PRINT_NODEDATA();
        return 0;
}

DEFINE_COMPILATION_BEHAVIOUR(DEFINE) {
        ADD_NODE(DEFINE);

        uint32_t* size = decltype(size)(nodeList[currentNode].access.data);
        EVAL_SYMBOL();

        try {
                *size = std::stoul(fileRead);
        }
        catch(std::invalid_argument) {
                PRINT_COMPILER_ERROR("DEFINE Argument must be constant numeral type.\n");
                return 1;
        }
        
        PRINT_NODEDATA();
        return 0;
}












// COMPILER INSTANCE BEHAVIOUR GOES HERE                ||
//                                                      VV



int nthp::script::CompilerInstance::compileSourceFile(const char* inputFile, const char* outputFile) {
        NOVERB_PRINT_COMPILER("\tCompiling Source File [%s]...\n", inputFile);
        
        std::fstream file(inputFile, std::ios::in);
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

        std::vector<size_t> ifLocations;
        std::vector<size_t> endLocations;

        size_t currentMacroPosition = 0;
        size_t targetMacro = 0;
        bool evaluateMacro = false;


        #define COMPILE(instruction) if( instruction ( nodeList, file, fileRead, constantList, macroList, varList, labelList, gotoList, ifLocations, endLocations, currentMacroPosition, targetMacro, evaluateMacro ) ) return 1
        #define CHECK_COMP(instruction) if(fileRead == #instruction) { COMPILE(instruction); continue; }

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

                                NOVERB_PRINT_COMPILER(" [%s]", fileRead.c_str());
                        }
                        NOVERB_PRINT_COMPILER("\n");

                        macroList.push_back(newDef);
                }

                

                CHECK_COMP(LABEL);
                CHECK_COMP(GOTO);
                CHECK_COMP(SUSPEND);
                CHECK_COMP(JUMP);
                CHECK_COMP(GETINDEX);
                CHECK_COMP(RETURN);

                CHECK_COMP(INC);
                CHECK_COMP(DEC);

                CHECK_COMP(ADD);
                CHECK_COMP(SUB);
                CHECK_COMP(MUL);
                CHECK_COMP(DIV);

                CHECK_COMP(IF);
                CHECK_COMP(END);

                CHECK_COMP(SET);
                CHECK_COMP(CLEAR);
                CHECK_COMP(DEFINE);

        }

        NOVERB_PRINT_COMPILER("\tSuccessfully compiled source file [%s].\n", inputFile);
        file.close();

        if(ifLocations.size() != endLocations.size()) {
                PRINT_COMPILER_ERROR("Post-Mortem failure; Unequal IF and END statements.\n");
                return 1;
        }
    
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
                        PRINT_DEBUG_WARNING("Failed to link GOTO [%zu] to LABEL block. Broken GOTO created.\n", gotoList[gotoIndex].goto_position);
                        uint32_t* location = (decltype(location))nodeList[gotoList[gotoIndex].goto_position].access.data;

                        *location = gotoList[gotoIndex].goto_position;
                }
        }
        
        // Match IFs and ENDs

        unsigned int numberOfIfsFound = 1;
	unsigned int numberOfEndsFound = 0;
	unsigned int finalEndIndex = 0;
	uint32_t* endIndex = nullptr;

        for (size_t i = 0; i < ifLocations.size(); i++) {
                NOVERB_PRINT_COMPILER("Checking IF [%zu]\n", ifLocations[i]);
		for (finalEndIndex = ifLocations[i] + 1; numberOfIfsFound != numberOfEndsFound && finalEndIndex < nodeList.size(); ++finalEndIndex) {

			// If an IF statement is found before and END statement, the program requires that many more ENDs
			// to break the loop. The corresponding END will be the one found when there are equal IFs and ENDs found.
			if (nodeList[finalEndIndex].access.ID == nthp::script::instructions::ID::END) {
				numberOfEndsFound++;
			}
			if (nodeList[finalEndIndex].access.ID == nthp::script::instructions::ID::LOGIC_EQU ||
				nodeList[finalEndIndex].access.ID == nthp::script::instructions::ID::LOGIC_NOT ||
				nodeList[finalEndIndex].access.ID == nthp::script::instructions::ID::LOGIC_GRT ||
				nodeList[finalEndIndex].access.ID == nthp::script::instructions::ID::LOGIC_LST ||
				nodeList[finalEndIndex].access.ID == nthp::script::instructions::ID::LOGIC_GRTE ||
			        nodeList[finalEndIndex].access.ID == nthp::script::instructions::ID::LOGIC_LSTE
			        ) {
			numberOfIfsFound++;
		}

		}
                if(finalEndIndex == nodeList.size()) {
                        PRINT_COMPILER_ERROR("Unable to find matching END. The only reason this would print is if you put an END before an IF. Absolute moron.\n");
                        return 1;
                }

                // Important, not a mistake. Don't go blaming this 3 months from now.
                --finalEndIndex;
                // Assigns the pointer to the last 4 bytes of the node to store the end index.
		endIndex = (uint32_t*)(nodeList[ifLocations[i]].access.data + sizeof(nthp::script::instructions::stdRef) + sizeof(nthp::script::instructions::stdRef));

                NOVERB_PRINT_COMPILER("Matched IF to END at [%u].\n", finalEndIndex);


		*endIndex = finalEndIndex;
		numberOfIfsFound = 1;
		numberOfEndsFound = 0;

	}



        return 0;

}