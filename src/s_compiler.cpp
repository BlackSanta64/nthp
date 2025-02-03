#ifdef PM
        #ifndef DEBUG
	        #define DEBUG
        #endif
#endif

#include "s_compiler.hpp"
using namespace nthp::script::instructions;


// I love this
#define DEFINE_COMPILATION_BEHAVIOUR(instruction) int instruction (std::vector<nthp::script::Node>& nodeList,\
                                                                        std::fstream& file,\
                                                                        std::string& fileRead,\
                                                                        std::vector<nthp::script::CompilerInstance::CONST_DEF>& constantList,\
                                                                        std::vector<nthp::script::CompilerInstance::MACRO_DEF>& macroList,\
                                                                        std::vector<nthp::script::CompilerInstance::VAR_DEF>& varList,\
                                                                        std::vector<nthp::script::CompilerInstance::GLOBAL_DEF>& globalList,\
                                                                        std::vector<nthp::script::CompilerInstance::LABEL_DEF>& labelList,\
                                                                        std::vector<nthp::script::CompilerInstance::GOTO_DEF>& gotoList,\
                                                                        std::vector<size_t>& ifList,\
                                                                        std::vector<size_t>& endList,\
                                                                        std::vector<size_t>& skipList,\
                                                                        size_t& currentMacroPosition,\
                                                                        size_t& targetMacro,\
                                                                        bool& evaluateMacro,\
                                                                        const bool& buildSystemContext\
                                                                        )

#define currentNode nodeList.size() - (decltype(nodeList.size()))1

// Adds node with corresponding instruction ID and allocates node memory.
#define ADD_NODE(instruction) nodeList.push_back(nthp::script::Node(GET_INSTRUCTION_ID(instruction), GET_INSTRUCTION_SIZE(instruction)));\
        if(GET_INSTRUCTION_SIZE(instruction) > 0) nodeList[currentNode].access.data = (char*)malloc(GET_INSTRUCTION_SIZE(instruction));\
        else nodeList[currentNode].access.data = nullptr

#define VECT_END(vect) vect.size() - 1

#define READ_FILE() (file >> fileRead)

bool skipInstructionCheck = false;




#ifdef DEBUG

void PRINT_COMPILER(const char* format, ...) {
        va_list ap;
	
	va_start(ap, format);

	fprintf(NTHP_debug_output, "[t %u] COMPILER: ", SDL_GetTicks());	
	vfprintf(NTHP_debug_output, format, ap);


	va_end(ap);
}

void PRINT_COMPILER_ERROR(const char* format, ...) {
        va_list ap;
	
	va_start(ap, format);

	fprintf(NTHP_debug_output, "[t %u] ERROR: ", SDL_GetTicks());	
	vfprintf(NTHP_debug_output, format, ap);


	va_end(ap);
}

void PRINT_COMPILER_WARNING(const char* format, ...) {
        va_list ap;
	
	va_start(ap, format);

	fprintf(NTHP_debug_output, "[t %u] WARNING: ", SDL_GetTicks());	
	vfprintf(NTHP_debug_output, format, ap);


	va_end(ap);
}

void PRINT_COMPILER_DEPEND_ERROR(const char* format, ...) {
        va_list ap;
	
	va_start(ap, format);

	fprintf(NTHP_debug_output, "[t %u] DEPENDENCY ERROR: ", SDL_GetTicks());	
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
                                PRINT_COMPILER("Substituted main expression to [%s].\n", list[i].value.c_str());
                                return 0;
                        }
                }

                PRINT_COMPILER_ERROR("Unable to evaluate CONST substitution [%s]; CONST not declared.\n", expression.c_str());
                return 1;
        }
        return 0;
}

// Fucking hell. I suck at this.
void destroyArgumentConsts(std::vector<nthp::script::CompilerInstance::CONST_DEF>& constantList) {
        std::string search;
        for(size_t i = 0; i < constantList.size(); ++i) {
                search = constantList[i].constName;
                search.erase(search.begin() + 3, search.end()); // Keeps the first 3 characters.
                if(search == "#ar") {
                        nthp::script::CompilerInstance::undefConstant(i, constantList);
                        --i;
                }
        }

        PRINT_DEBUG("Purged temporary CONSTs.\n");
}





int EvaluateMacro(std::fstream& file, std::string& expression, std::vector<nthp::script::CompilerInstance::MACRO_DEF>& list, std::vector<nthp::script::CompilerInstance::CONST_DEF>& constantList, size_t& mp, size_t& targetMacro, bool& beginMacroEval) {
        if(expression[0] == '@') {
                auto initialPosition = file.tellg();

                for(size_t i = 0; i < list.size(); ++i) {
                        if(expression == list[i].macroName) {
                                targetMacro = i;

                                
                                (file >> expression);
                                nthp::script::CompilerInstance::portable_evalConst(expression, constantList);
                                if(expression == "(") {
                                        // Evaluate Arguments
                                        std::string argument_const;
                                        size_t argumentsFound = 0;
                                        do {
                                                (file >> expression);
                                                nthp::script::CompilerInstance::portable_evalConst(expression, constantList);
                                                if(expression != ")") {        
                                                        ++argumentsFound;
                                                        if(argumentsFound > 500) {
                                                                PRINT_COMPILER_ERROR("Macro Argument data is too large; no ARG_END character found.\n");
                                                                return 1;
                                                        }

                                                        argument_const = ("#ar" + std::to_string(argumentsFound - 1));

                                                        constantList.push_back(nthp::script::CompilerInstance::CONST_DEF());
                                                        constantList[constantList.size() - 1].constName = argument_const;
                                                        constantList[constantList.size() - 1].value = expression;
                                                        PRINT_COMPILER("Detected argument name[%s] = [%s];\n", constantList[constantList.size() - 1].constName.c_str(), constantList[constantList.size() - 1].value.c_str());

                                                }
                                        }
                                        while(expression != ")");
                                        PRINT_COMPILER("Evaluated Arguments.\n");

                                }
                                else {
                                        file.seekg(initialPosition);
                                }
                                // TODO: Test this shit




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


                if(currentMacroPosition == macroList[targetMacro].macroData.size()) {
                        evaluatingMacro = false;
                        NOVERB_PRINT_COMPILER("\tCompleted expansion of macro [%s].\n", macroList[targetMacro].macroName.c_str());
                        destroyArgumentConsts(constList);
                }
                else {
                        expression = macroList[targetMacro].macroData[currentMacroPosition];
                        if(EvaluateConst(expression, constList)) return 1;
                        

                        return 0;
                }
        }

        (file) >> (expression);
        if(EvaluateConst(expression, constList)) return 1;
        if(EvaluateMacro(file, expression, macroList, constList, currentMacroPosition, targetMacro, evaluatingMacro)) return 1;
        return 0;
}


// Substitues a VAR reference or parses numeral references (for compatibility)
nthp::script::instructions::stdRef EvaluateReference(std::string expression, std::vector<nthp::script::CompilerInstance::VAR_DEF>& varList, std::vector<nthp::script::CompilerInstance::GLOBAL_DEF>& globalList, bool buildSystemContext) {
        stdRef ref;
        ref.metadata = 0;
        ref.value = 0;

        do {
                if(expression[0] == '-') {
                        if(expression.size() < 2) {
                                PRINT_COMPILER_ERROR("Unable to evaluate reference [%s]; Invalid Argument.\n", expression.c_str());
                                return ref;
                        }
                        if(expression[1] == '$' || expression[1] == '>') {
                                PR_METADATA_SET(ref, nthp::script::flagBits::IS_NEGATED);
                                expression.erase(expression.begin());
                        }
                        else break;
                }

                // No need for 2 compares. Only reaches this point if dereference character is present ($ OR > checked prior).
                if(expression[0] == '$') {
                        PR_METADATA_SET         (ref, nthp::script::flagBits::IS_REFERENCE);

                        expression.erase(expression.begin());
                        break;
                }
                if(expression[0] == '>') {
                        PR_METADATA_SET (ref, nthp::script::flagBits::IS_REFERENCE);
                        if(buildSystemContext) PR_METADATA_SET (ref, nthp::script::flagBits::IS_GLOBAL);

                        expression.erase(expression.begin());
                }

        } while(0);

        // Evaluate Var.
        // If no VARNAME is referenced, assumes numeral reference type (instead of $VARNAME or >VARNAME, $2 or >2), or constant. Throws
        // Invalid argument if otherwise.
        if(PR_METADATA_GET(ref, nthp::script::flagBits::IS_REFERENCE)) {
                if(PR_METADATA_GET(ref, nthp::script::flagBits::IS_GLOBAL)) {
                        for(size_t i = 0; i < globalList.size(); ++i) {
                                if(expression == globalList[i].varName) {
                                        expression = std::to_string(globalList[i].relativeIndex);
                                }
                        }
                }
                else {
                        for(size_t i = 0; i < varList.size(); ++i) {
                                if(expression == varList[i].varName) {
                                        expression = std::to_string(varList[i].relativeIndex);
                                }
                        }
                }
        }




        try {
                double value = std::stod(expression);
                if((std::abs(value) < nthp::fixedTypeConstants::FIXED_EPSILON) && (value != 0)) {
                        PRINT_COMPILER_WARNING("Expression [%s] cannot be expressed/approximated in current fixed point system configuration. Expression will be rounded to 0.\n", expression.c_str());
                }



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
#define EVAL_SYMBOL() ____S_EVAL(file, fileRead, constantList, macroList, currentMacroPosition, targetMacro, evaluateMacro)
#define EVAL_PREF() EvaluateReference(fileRead, varList, globalList, buildSystemContext)

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


        EVAL_SYMBOL();
        nthp::script::instructions::indRef* labelID = decltype(labelID)(nodeList[currentNode].access.data);
        auto static_label = EVAL_PREF();

        CHECK_REF(static_label);
        if(!PR_METADATA_GET(static_label, nthp::script::flagBits::IS_REFERENCE)) {
                PRINT_COMPILER_WARNING("JUMP Node at [%zu] uses constant expression as label reference; Consider using GOTO instead.\n", currentNode);
        }

        labelID->metadata = static_label.metadata;
        labelID->value = nthp::fixedToInt(static_label.value);
        NOVERB_PRINT_COMPILER("[%zu] JUMP Node evaluated: ID = %lld\n", currentNode, nthp::fixedToInt(labelID->value));

        PRINT_NODEDATA();
        return 0;
}

DEFINE_COMPILATION_BEHAVIOUR(RETURN) {
        ADD_NODE(RETURN);

        EVAL_SYMBOL();
        auto static_ref = EVAL_PREF();
        CHECK_REF(static_ref);

        stdRef* instruction = decltype(instruction)(nodeList[currentNode].access.data);
        *instruction = static_ref;

        PRINT_NODEDATA();
        return 0;
}


DEFINE_COMPILATION_BEHAVIOUR(GETINDEX) {
        ADD_NODE(GETINDEX);


        indRef* pointer = decltype(pointer)(nodeList[currentNode].access.data);
        EVAL_SYMBOL();

        auto static_ref = EVAL_PREF();
        CHECK_REF(static_ref);

        if(!PR_METADATA_GET(static_ref, nthp::script::flagBits::IS_REFERENCE)) {
                PRINT_COMPILER_ERROR("GETINDEX Argument must reference.\n");
                return 1;
        }

        pointer->metadata = static_ref.metadata;
        pointer->value = nthp::fixedToInt(static_ref.value);

        PRINT_NODEDATA();
        return 0;
}


DEFINE_COMPILATION_BEHAVIOUR(INC) {
        ADD_NODE(INC);

        EVAL_SYMBOL();
        indRef* var = decltype(var)(nodeList[currentNode].access.data);
        auto static_var = EVAL_PREF();

        CHECK_REF(static_var);

        var->metadata = static_var.metadata;
        var->value = (uint32_t)nthp::fixedToInt(static_var.value);
      
        PRINT_NODEDATA();
        return 0;
}

DEFINE_COMPILATION_BEHAVIOUR(DEC) {
        ADD_NODE(DEC);

        EVAL_SYMBOL();
        indRef* var = decltype(var)(nodeList[currentNode].access.data);
        auto static_var = EVAL_PREF();

        CHECK_REF(static_var);

        if(!PR_METADATA_GET(static_var, nthp::script::flagBits::IS_REFERENCE)) {
                PRINT_COMPILER_ERROR("Unable to evaluate Reference; INC must take reference.\n");
                return 1;
        }
        var->metadata = static_var.metadata;
        var->value = (uint32_t)nthp::fixedToInt(static_var.value);
      
        PRINT_NODEDATA();
        return 0;
}


DEFINE_COMPILATION_BEHAVIOUR(RSHIFT) {
        ADD_NODE(RSHIFT);

        EVAL_SYMBOL();
        auto ref = EVAL_PREF();
        CHECK_REF(ref);

        if(!PR_METADATA_GET(ref, nthp::script::flagBits::IS_REFERENCE)) {
                PRINT_COMPILER_ERROR("Unable to evaluate Reference; RSHIFT must take reference.\n");
                return 1;
        }

        indRef* var = (decltype(var))(nodeList[currentNode].access.data);

        EVAL_SYMBOL();
        auto count = EVAL_PREF();
        CHECK_REF(count);

        stdRef* fcount = (decltype(fcount))(nodeList[currentNode].access.data + sizeof(indRef));

        var->metadata = ref.metadata;
        var->value = (uint32_t)nthp::fixedToInt(ref.value);

        *fcount = count;

        PRINT_NODEDATA();
        return 0;
}

DEFINE_COMPILATION_BEHAVIOUR(LSHIFT) {
        ADD_NODE(LSHIFT);

        EVAL_SYMBOL();
        auto ref = EVAL_PREF();
        CHECK_REF(ref);

        if(!PR_METADATA_GET(ref, nthp::script::flagBits::IS_REFERENCE)) {
                PRINT_COMPILER_ERROR("Unable to evaluate Reference; RSHIFT must take reference.\n");
                return 1;
        }

        indRef* var = (decltype(var))(nodeList[currentNode].access.data);

        EVAL_SYMBOL();
        auto count = EVAL_PREF();
        CHECK_REF(count);

        stdRef* fcount = (decltype(fcount))(nodeList[currentNode].access.data + sizeof(indRef));

        var->metadata = ref.metadata;
        var->value = (uint32_t)nthp::fixedToInt(ref.value);
        
        *fcount = count;

        PRINT_NODEDATA();
        return 0;
}



DEFINE_COMPILATION_BEHAVIOUR(ADD) {
        ADD_NODE(ADD);

        nthp::script::instructions::stdRef* operandA = decltype(operandA)(nodeList[currentNode].access.data);
        nthp::script::instructions::stdRef* operandB = decltype(operandB)(nodeList[currentNode].access.data + sizeof(nthp::script::instructions::stdRef));
        indRef* output = decltype(output)(nodeList[currentNode].access.data + sizeof(nthp::script::instructions::stdRef) + sizeof(nthp::script::instructions::stdRef));

        EVAL_SYMBOL();
        auto static_op_A = EVAL_PREF();
        CHECK_REF(static_op_A);

        EVAL_SYMBOL();
        auto static_op_B = EVAL_PREF();
        CHECK_REF(static_op_B);

        EVAL_SYMBOL();
        auto static_output = EVAL_PREF();
        CHECK_REF(static_output);


        if(!PR_METADATA_GET(static_output, nthp::script::flagBits::IS_REFERENCE)) {
                PRINT_COMPILER_ERROR("Final Operand of ADD (arg.ADD_OUTPUT) must be a reference.\n");
                return 1;
        }


        *operandA = static_op_A;
        *operandB = static_op_B;

        output->metadata = static_output.metadata;
        output->value = (uint32_t)nthp::fixedToInt(static_output.value);;

        
        PRINT_NODEDATA();
        return 0;
}

DEFINE_COMPILATION_BEHAVIOUR(SUB) {
        ADD_NODE(SUB);

        nthp::script::instructions::stdRef* operandA = decltype(operandA)(nodeList[currentNode].access.data);
        nthp::script::instructions::stdRef* operandB = decltype(operandB)(nodeList[currentNode].access.data + sizeof(nthp::script::instructions::stdRef));
        indRef* output = decltype(output)(nodeList[currentNode].access.data + sizeof(nthp::script::instructions::stdRef) + sizeof(nthp::script::instructions::stdRef));

        EVAL_SYMBOL();
        auto static_op_A = EVAL_PREF();
        CHECK_REF(static_op_A);

        EVAL_SYMBOL();
        auto static_op_B = EVAL_PREF();
        CHECK_REF(static_op_B);

        EVAL_SYMBOL();
        auto static_output = EVAL_PREF();
        CHECK_REF(static_output);


        if(!PR_METADATA_GET(static_output, nthp::script::flagBits::IS_REFERENCE)) {
                PRINT_COMPILER_ERROR("Final Operand of SUB (arg.SUB_OUTPUT) must be a reference.\n");
                return 1;
        }


        *operandA = static_op_A;
        *operandB = static_op_B;

        output->metadata = static_output.metadata;
        output->value = (uint32_t)nthp::fixedToInt(static_output.value);

        PRINT_NODEDATA();
        return 0;
}

DEFINE_COMPILATION_BEHAVIOUR(MUL) {
        ADD_NODE(MUL);

        nthp::script::instructions::stdRef* operandA = decltype(operandA)(nodeList[currentNode].access.data);
        nthp::script::instructions::stdRef* operandB = decltype(operandB)(nodeList[currentNode].access.data + sizeof(nthp::script::instructions::stdRef));
        indRef* output = decltype(output)(nodeList[currentNode].access.data + sizeof(nthp::script::instructions::stdRef) + sizeof(nthp::script::instructions::stdRef));

        EVAL_SYMBOL();
        auto static_op_A = EVAL_PREF();
        CHECK_REF(static_op_A);

        EVAL_SYMBOL();
        auto static_op_B = EVAL_PREF();
        CHECK_REF(static_op_B);

        EVAL_SYMBOL();
        auto static_output = EVAL_PREF();
        CHECK_REF(static_output);


        if(!PR_METADATA_GET(static_output, nthp::script::flagBits::IS_REFERENCE)) {
                PRINT_COMPILER_ERROR("Final Operand of MUL (arg.MUL_OUTPUT) must be a reference.\n");
                return 1;
        }


        *operandA = static_op_A;
        *operandB = static_op_B;

        output->metadata = static_output.metadata;
        output->value = (uint32_t)nthp::fixedToInt(static_output.value);


        PRINT_NODEDATA();
        return 0;
}

DEFINE_COMPILATION_BEHAVIOUR(DIV) {
        ADD_NODE(DIV);

        nthp::script::instructions::stdRef* operandA = decltype(operandA)(nodeList[currentNode].access.data);
        nthp::script::instructions::stdRef* operandB = decltype(operandB)(nodeList[currentNode].access.data + sizeof(nthp::script::instructions::stdRef));
        indRef* output = decltype(output)(nodeList[currentNode].access.data + sizeof(nthp::script::instructions::stdRef) + sizeof(nthp::script::instructions::stdRef));

        EVAL_SYMBOL();
        auto static_op_A = EVAL_PREF();
        CHECK_REF(static_op_A);

        EVAL_SYMBOL();
        auto static_op_B = EVAL_PREF();
        CHECK_REF(static_op_B);

        EVAL_SYMBOL();
        auto static_output = EVAL_PREF();
        CHECK_REF(static_output);


        if(!PR_METADATA_GET(static_output, nthp::script::flagBits::IS_REFERENCE)) {
                PRINT_COMPILER_ERROR("Final Operand of DIV (arg.DIV_OUTPUT) must be a reference.\n");
                return 1;
        }


        *operandA = static_op_A;
        *operandB = static_op_B;

        output->metadata = static_output.metadata;
        output->value = (uint32_t)nthp::fixedToInt(static_output.value);

     
        PRINT_NODEDATA();
        return 0;
}

DEFINE_COMPILATION_BEHAVIOUR(SQRT) {
        ADD_NODE(SQRT);

        nthp::script::instructions::stdRef* base = (decltype(base))nodeList[currentNode].access.data;
        indRef* output = (decltype(output))(nodeList[currentNode].access.data + sizeof(nthp::script::instructions::stdRef));

        EVAL_SYMBOL();
        auto static_base = EVAL_PREF();
        CHECK_REF(static_base);

        EVAL_SYMBOL();
        auto static_output = EVAL_PREF();
        CHECK_REF(static_output);

        if(!PR_METADATA_GET(static_output, nthp::script::flagBits::IS_REFERENCE)) {
                PRINT_COMPILER_ERROR("Final Operand of SQRT (arg.SQRT_OUTPUT) must be a reference\n");
                return 1;
        }

        *base = static_base;
        
        output->metadata = static_output.metadata;
        output->value = (uint32_t)nthp::fixedToInt(static_output.value);;


        PRINT_NODEDATA();
        return 0;
}



DEFINE_COMPILATION_BEHAVIOUR(END) {
        ADD_NODE(END);
        endList.push_back(currentNode);

        PRINT_NODEDATA();
        return 0;
}

DEFINE_COMPILATION_BEHAVIOUR(ELSE) {
        ADD_NODE(ELSE);

        PRINT_NODEDATA();
        return 0;
}

DEFINE_COMPILATION_BEHAVIOUR(SKIP) {
        ADD_NODE(SKIP);

        skipList.push_back(currentNode);

        PRINT_NODEDATA();
        return 0;
}

DEFINE_COMPILATION_BEHAVIOUR(SKIP_END) {
        ADD_NODE(SKIP_END);

        PRINT_NODEDATA();
        return 0;
}

DEFINE_COMPILATION_BEHAVIOUR(IF) {

        nthp::script::instructions::stdRef static_opB;

        EVAL_SYMBOL();
        auto static_opA = EVAL_PREF();
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
                // Completely different compiler behaviour if using the BNE instruction.
                // opA is the only data parsed here.
                
                
                // Because opA is the only argument, EXPRESSION currently is the next instruction after the IF.
                // This will make sure the next argument is NOT read at the start of the next pass.


                ADD_NODE(LOGIC_IF_TRUE);
                skipInstructionCheck = true;


                ifList.push_back(currentNode);
                nthp::script::instructions::stdRef* opA = (decltype(opA))(nodeList[currentNode].access.data);

                *opA = static_opA;

                PRINT_NODEDATA();
                return 0;
        }

        EVAL_SYMBOL();
        static_opB = EVAL_PREF();
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


        indRef* pointer = decltype(pointer)(nodeList[currentNode].access.data);
        nthp::script::stdVarWidth* value = decltype(value)(nodeList[currentNode].access.data + sizeof(indRef));

        EVAL_SYMBOL(); // pointer
        auto ref = EVAL_PREF();

        CHECK_REF(ref);
        if(!PR_METADATA_GET(ref, nthp::script::flagBits::IS_REFERENCE)) {
                PRINT_COMPILER_ERROR("First SET argument must be a reference. Syntax: SET $var value\n");
                return 1;
        }

        EVAL_SYMBOL(); // value
        nthp::script::stdVarWidth static_value;

        try {
                double conv = std::stod(fileRead);
                if((std::abs(conv) < nthp::fixedTypeConstants::FIXED_EPSILON) && (conv != 0)) {
                        PRINT_COMPILER_WARNING("Expression [%s] cannot be expressed/approximated in current fixed point system configuration. Expression will be rounded to 0.\n", fileRead.c_str());
                }

                static_value = nthp::doubleToFixed(conv);
        }
        catch(std::invalid_argument) {
                PRINT_COMPILER_ERROR("Unable to evaluate numeral at [%zu]; Invalid Argument.\n", currentNode);
                return 1;
        }

        pointer->metadata = ref.metadata;
        pointer->value = (uint32_t)nthp::fixedToInt(ref.value);

        *value = static_value;

        PRINT_NODEDATA();
        return 0;
}

DEFINE_COMPILATION_BEHAVIOUR(SET_BINARY) {
        ADD_NODE(SET_BINARY);


        indRef* pointer = decltype(pointer)(nodeList[currentNode].access.data);
        nthp::script::stdVarWidth* value = decltype(value)(nodeList[currentNode].access.data + sizeof(indRef));

        EVAL_SYMBOL(); // pointer
        auto ref = EVAL_PREF();

        CHECK_REF(ref);
        if(!PR_METADATA_GET(ref, nthp::script::flagBits::IS_REFERENCE)) {
                PRINT_COMPILER_ERROR("First SET argument must be a reference. Syntax: SET $var value\n");
                return 1;
        }

        EVAL_SYMBOL(); // value
        nthp::script::stdVarWidth static_value;

        try {
                auto conv = std::stoll(fileRead);
                static_value = conv;
        }
        catch(std::invalid_argument) {
                PRINT_COMPILER_ERROR("Unable to evaluate numeral at [%zu]; Invalid Argument.\n", currentNode);
                return 1;
        }

        pointer->metadata = ref.metadata;
        pointer->value = (uint32_t)nthp::fixedToInt(ref.value);

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


DEFINE_COMPILATION_BEHAVIOUR(COPY) {
        ADD_NODE(COPY);

        EVAL_SYMBOL();
        auto sfrom = EVAL_PREF();
        CHECK_REF(sfrom);

        EVAL_SYMBOL();
        auto sto = EVAL_PREF();
        CHECK_REF(sto);

        if(!PR_METADATA_GET(sfrom, nthp::script::flagBits::IS_REFERENCE) || !PR_METADATA_GET(sto, nthp::script::flagBits::IS_REFERENCE)) {
                PRINT_COMPILER_ERROR("COPY must take two references. (COPY $from $to).\n");
                return 1;
        }

        indRef* from = (indRef*)(nodeList[currentNode].access.data);
        indRef* to = (indRef*)(nodeList[currentNode].access.data + sizeof(indRef));        

        to->value = nthp::fixedToInt(sto.value);
        to->metadata = sto.metadata;

        from->value = nthp::fixedToInt(sfrom.value);
        from->metadata = sfrom.metadata;


        PRINT_NODEDATA();
        return 0;
}


DEFINE_COMPILATION_BEHAVIOUR(TEXTURE_DEFINE) {
	ADD_NODE(TEXTURE_DEFINE);

	stdRef* size = (decltype(size))(nodeList[currentNode].access.data);
	EVAL_SYMBOL(); // file

	auto ref = EVAL_PREF();
	CHECK_REF(ref);

	// Remove decimal, As it is defining a memory block. Redundant if it's a reference, but cheap so who cares.
	ref.value = nthp::getFixedInteger(ref.value);
	*size = ref;
	
        PRINT_NODEDATA();
	return 0;	
}

DEFINE_COMPILATION_BEHAVIOUR(TEXTURE_CLEAR) {
	ADD_NODE(TEXTURE_CLEAR);


        PRINT_NODEDATA();
	return 0;
}



DEFINE_COMPILATION_BEHAVIOUR(TEXTURE_LOAD) {
	
	EVAL_SYMBOL(); // output
        auto ref = EVAL_PREF();
        CHECK_REF(ref);

        EVAL_SYMBOL(); // file
        std::string textureFile = fileRead;


	if(textureFile.size() > UINT8_MAX - sizeof(nthp::script::instructions::stdRef)) {
		PRINT_COMPILER_ERROR("File path name in TEXTURE_LOAD at [%zu] too large. Must be less than [%u] characters.", currentNode, UINT8_MAX - sizeof(nthp::script::instructions::stdRef));
		return 1;
	}
	ADD_NODE(TEXTURE_LOAD);
	nodeList[currentNode].access.size = sizeof(nthp::script::instructions::stdRef) + textureFile.size() + 1;
	nodeList[currentNode].access.data = (char*)malloc(nodeList[currentNode].access.size);

	nthp::script::instructions::stdRef* output = (decltype(output))(nodeList[currentNode].access.data);

	// Funny pointer stuff.	
	memcpy(nodeList[currentNode].access.data + sizeof(nthp::script::instructions::stdRef), textureFile.c_str(), textureFile.size());
        nodeList[currentNode].access.data[nodeList[currentNode].access.size - 1] = '\000';
        
       

        ref.value = nthp::getFixedInteger(ref.value);
        *output = ref;

        PRINT_NODEDATA();
	return 0;
}



DEFINE_COMPILATION_BEHAVIOUR(SET_ACTIVE_PALETTE) {
        EVAL_SYMBOL();

        std::string paletteFile = fileRead;
        if(paletteFile.size() > UINT8_MAX) {
                PRINT_COMPILER_ERROR("File path name in TEXTURE_LOAD at [%zu] too large. Must be less than [%u] characters.", currentNode, UINT8_MAX);
		return 1;
        }
        ADD_NODE(SET_ACTIVE_PALETTE);
        nodeList[currentNode].access.size = paletteFile.length() + 1;
        nodeList[currentNode].access.data =  (char*)malloc(nodeList[currentNode].access.size);
        memcpy(nodeList[currentNode].access.data, paletteFile.c_str(), paletteFile.length() + 1);

        nodeList[currentNode].access.data[paletteFile.length()] = '\000';

        PRINT_NODEDATA();
        return 0;
}



DEFINE_COMPILATION_BEHAVIOUR(FRAME_DEFINE) {
        ADD_NODE(FRAME_DEFINE);

        EVAL_SYMBOL();
        auto size = EVAL_PREF();
        CHECK_REF(size);

        stdRef* dSize = (decltype(dSize))(nodeList[currentNode].access.data);
        *dSize = size;


        PRINT_NODEDATA();
        return 0;
}

DEFINE_COMPILATION_BEHAVIOUR(FRAME_CLEAR) {
        ADD_NODE(FRAME_CLEAR);

        PRINT_NODEDATA();
        return 0;
}

DEFINE_COMPILATION_BEHAVIOUR(FRAME_SET) {
        ADD_NODE(FRAME_SET);

        EVAL_SYMBOL();
        auto sframeIndex = EVAL_PREF();
        CHECK_REF(sframeIndex);

        EVAL_SYMBOL();
        auto sx = EVAL_PREF();
        CHECK_REF(sx);

        EVAL_SYMBOL();
        auto sy = EVAL_PREF();
        CHECK_REF(sy);
        
        EVAL_SYMBOL();
        auto sw = EVAL_PREF();
        CHECK_REF(sw);
        
        EVAL_SYMBOL();
        auto sh = EVAL_PREF();
        CHECK_REF(sh);

        EVAL_SYMBOL();
        auto stextureIndex = EVAL_PREF();
        CHECK_REF(stextureIndex);

        stdRef* frameIndex = (decltype(frameIndex))(nodeList[currentNode].access.data);
        stdRef* x = (decltype(frameIndex))(nodeList[currentNode].access.data + sizeof(stdRef));
        stdRef* y = (decltype(frameIndex))(nodeList[currentNode].access.data + (sizeof(stdRef) * 2));
        stdRef* w = (decltype(frameIndex))(nodeList[currentNode].access.data + (sizeof(stdRef) * 3));
        stdRef* h = (decltype(frameIndex))(nodeList[currentNode].access.data + (sizeof(stdRef) * 4));
        stdRef* textureIndex = (decltype(frameIndex))(nodeList[currentNode].access.data + (sizeof(stdRef) * 5));

        *frameIndex = sframeIndex;
        *x = sx;
        *y = sy;
        *w = sw;
        *h = sh;
        *textureIndex = stextureIndex;

        PRINT_NODEDATA();
        return 0;
}


DEFINE_COMPILATION_BEHAVIOUR(GETGPR) {
        ADD_NODE(GETGPR);

        EVAL_SYMBOL();
        auto write_to = EVAL_PREF();
        CHECK_REF(write_to);


        if(!PR_METADATA_GET(write_to, nthp::script::flagBits::IS_REFERENCE)) {
                PRINT_COMPILER_ERROR("GETGPR must take a reference.\n");
                return 1;
        }

        indRef* output = (indRef*)(nodeList[currentNode].access.data);
        output->value = nthp::fixedToInt(write_to.value);
        output->metadata = write_to.metadata;

        PRINT_NODEDATA();
        return 0;
}


DEFINE_COMPILATION_BEHAVIOUR(SM_WRITE) {
        ADD_NODE(SM_WRITE);

        EVAL_SYMBOL();
        auto to = EVAL_PREF();
        CHECK_REF(to);

        EVAL_SYMBOL();
        auto from = EVAL_PREF();
        CHECK_REF(from);

        stdRef* _to = (stdRef*)(nodeList[currentNode].access.data);
        stdRef* value = (stdRef*)(nodeList[currentNode].access.data + sizeof(stdRef));

        *_to = to;
        *value = from;


        PRINT_NODEDATA();
        return 0;
}

DEFINE_COMPILATION_BEHAVIOUR(SM_READ) {
        ADD_NODE(SM_READ);


        stdRef* from = (stdRef*)(nodeList[currentNode].access.data);
        indRef* into = (indRef*)(nodeList[currentNode].access.data + sizeof(stdRef));


        EVAL_SYMBOL();
        auto _from = EVAL_PREF();
        CHECK_REF(_from);

        EVAL_SYMBOL();
        auto _into = EVAL_PREF();
        CHECK_REF(_into);

        if(!PR_METADATA_GET(_into, nthp::script::flagBits::IS_REFERENCE)) {
                PRINT_COMPILER_ERROR("SM_READ must take reference as 2nd argument.\n");
                return 1;
        }

        *from = _from;
        into->value = nthp::fixedToInt(_into.value);
        into->metadata = _into.metadata;

        PRINT_NODEDATA();
        return 0;
}


DEFINE_COMPILATION_BEHAVIOUR(ENT_DEFINE) {
        ADD_NODE(ENT_DEFINE);

        EVAL_SYMBOL();
        auto size = EVAL_PREF();
        CHECK_REF(size);

        stdRef* wsize = (stdRef*)(nodeList[currentNode].access.data);

        *wsize = size;

        PRINT_NODEDATA();
        return 0;
}


DEFINE_COMPILATION_BEHAVIOUR(ENT_CLEAR) {
        ADD_NODE(ENT_CLEAR);

        PRINT_NODEDATA();
        return 0;
}


DEFINE_COMPILATION_BEHAVIOUR(ENT_SETCURRENTFRAME) {
        ADD_NODE(ENT_SETCURRENTFRAME);

        EVAL_SYMBOL();
        auto target = EVAL_PREF();
        CHECK_REF(target);

        EVAL_SYMBOL();
        auto frameN = EVAL_PREF();
        CHECK_REF(frameN);

        stdRef* tout = (stdRef*)(nodeList[currentNode].access.data);
        stdRef* tframe = (stdRef*)(nodeList[currentNode].access.data + sizeof(stdRef));

        *tout = target;
        *tframe = frameN;

        PRINT_NODEDATA();
        return 0;
}


DEFINE_COMPILATION_BEHAVIOUR(ENT_SETPOS) {
        ADD_NODE(ENT_SETPOS);

        EVAL_SYMBOL();
        auto target = EVAL_PREF();
        CHECK_REF(target);

        EVAL_SYMBOL();
        auto x = EVAL_PREF();
        CHECK_REF(x);

        EVAL_SYMBOL();
        auto y = EVAL_PREF();
        CHECK_REF(y);

        stdRef* _target = (stdRef*)(nodeList[currentNode].access.data);
        stdRef* _x = (stdRef*)(nodeList[currentNode].access.data + sizeof(stdRef));
        stdRef* _y = (stdRef*)(nodeList[currentNode].access.data + sizeof(stdRef) + sizeof(stdRef));


        *_target = target;
        *_x = x;
        *_y = y;

        PRINT_NODEDATA();
        return 0;
}

DEFINE_COMPILATION_BEHAVIOUR(ENT_MOVE) {
        ADD_NODE(ENT_MOVE);

        EVAL_SYMBOL();
        auto target = EVAL_PREF();
        CHECK_REF(target);

        EVAL_SYMBOL();
        auto x = EVAL_PREF();
        CHECK_REF(x);

        EVAL_SYMBOL();
        auto y = EVAL_PREF();
        CHECK_REF(y);

        stdRef* _target = (stdRef*)(nodeList[currentNode].access.data);
        stdRef* _x = (stdRef*)(nodeList[currentNode].access.data + sizeof(stdRef));
        stdRef* _y = (stdRef*)(nodeList[currentNode].access.data + sizeof(stdRef) + sizeof(stdRef));


        *_target = target;
        *_x = x;
        *_y = y;

        PRINT_NODEDATA();
        return 0;
}


DEFINE_COMPILATION_BEHAVIOUR(ENT_SETFRAMERANGE) {
        ADD_NODE(ENT_SETFRAMERANGE);

        EVAL_SYMBOL();
        auto target = EVAL_PREF();
        CHECK_REF(target);

        EVAL_SYMBOL();
        auto start = EVAL_PREF();
        CHECK_REF(start);

        EVAL_SYMBOL();
        auto size = EVAL_PREF();
        CHECK_REF(size);


        stdRef* _target = (stdRef*)(nodeList[currentNode].access.data);
        stdRef* _start = (stdRef*)(nodeList[currentNode].access.data + sizeof(stdRef));
        stdRef* _size = (stdRef*)(nodeList[currentNode].access.data + sizeof(stdRef) + sizeof(stdRef));

        *_target = target;
        *_start = start;
        *_size = size;

        PRINT_NODEDATA();
        return 0;
}


DEFINE_COMPILATION_BEHAVIOUR(ENT_SETHITBOXSIZE) {
        ADD_NODE(ENT_SETHITBOXSIZE);

        EVAL_SYMBOL();
        auto target = EVAL_PREF();
        CHECK_REF(target);

        EVAL_SYMBOL();
        auto x = EVAL_PREF();
        CHECK_REF(x);

        EVAL_SYMBOL();
        auto y = EVAL_PREF();
        CHECK_REF(y);

        stdRef* _target = (stdRef*)(nodeList[currentNode].access.data);
        stdRef* _x = (stdRef*)(nodeList[currentNode].access.data + sizeof(stdRef));
        stdRef* _y = (stdRef*)(nodeList[currentNode].access.data + sizeof(stdRef) + sizeof(stdRef));


        *_target = target;
        *_x = x;
        *_y = y;
        PRINT_NODEDATA();

        return 0;
}


DEFINE_COMPILATION_BEHAVIOUR(ENT_SETHITBOXOFFSET) {
        ADD_NODE(ENT_SETHITBOXOFFSET);

        EVAL_SYMBOL();
        auto target = EVAL_PREF();
        CHECK_REF(target);

        EVAL_SYMBOL();
        auto x = EVAL_PREF();
        CHECK_REF(x);

        EVAL_SYMBOL();
        auto y = EVAL_PREF();
        CHECK_REF(y);

        stdRef* _target = (stdRef*)(nodeList[currentNode].access.data);
        stdRef* _x = (stdRef*)(nodeList[currentNode].access.data + sizeof(stdRef));
        stdRef* _y = (stdRef*)(nodeList[currentNode].access.data + sizeof(stdRef) + sizeof(stdRef));


        *_target = target;
        *_x = x;
        *_y = y;

        PRINT_NODEDATA();
        return 0;
}



DEFINE_COMPILATION_BEHAVIOUR(ENT_SETRENDERSIZE) {
        ADD_NODE(ENT_SETRENDERSIZE);

        EVAL_SYMBOL();
        auto target = EVAL_PREF();
        CHECK_REF(target);

        EVAL_SYMBOL();
        auto x = EVAL_PREF();
        CHECK_REF(x);

        EVAL_SYMBOL();
        auto y = EVAL_PREF();
        CHECK_REF(y);

        stdRef* _target = (stdRef*)(nodeList[currentNode].access.data);
        stdRef* _x = (stdRef*)(nodeList[currentNode].access.data + sizeof(stdRef));
        stdRef* _y = (stdRef*)(nodeList[currentNode].access.data + sizeof(stdRef) + sizeof(stdRef));


        *_target = target;
        *_x = x;
        *_y = y;

        PRINT_NODEDATA();
        return 0;
}


DEFINE_COMPILATION_BEHAVIOUR(ENT_CHECKCOLLISION) {
        ADD_NODE(ENT_CHECKCOLLISION);

        EVAL_SYMBOL();
        auto ent_a = EVAL_PREF();
        CHECK_REF(ent_a);

        EVAL_SYMBOL();
        auto ent_b = EVAL_PREF();
        CHECK_REF(ent_b);

        EVAL_SYMBOL();
        auto _output = EVAL_PREF();
        CHECK_REF(_output);


        if(!PR_METADATA_GET(_output, nthp::script::flagBits::IS_REFERENCE)) {
                PRINT_COMPILER_ERROR("Last argument of ENT_CHECKCOLLISION invalid. (syn. ENT_CHECKCOLLISION entA entB outputVar)\n");
                return 1;
        }

        stdRef* enta = (stdRef*)(nodeList[currentNode].access.data);
        stdRef* entb = (stdRef*)(nodeList[currentNode].access.data + sizeof(stdRef));
        indRef* output = (indRef*)(nodeList[currentNode].access.data + sizeof(stdRef) + sizeof(stdRef));

        *enta = ent_a;
        *entb = ent_b;
        output->value = nthp::fixedToInt(_output.value);
        output->metadata = _output.metadata;


        PRINT_NODEDATA();
        return 0;
}


DEFINE_COMPILATION_BEHAVIOUR(CORE_INIT) {

        

        EVAL_SYMBOL();
        auto spx = EVAL_PREF();
        CHECK_REF(spx);

        EVAL_SYMBOL();
        auto spy = EVAL_PREF();
        CHECK_REF(spy);

        EVAL_SYMBOL();
        auto stx = EVAL_PREF();
        CHECK_REF(stx);

        EVAL_SYMBOL();
        auto sty = EVAL_PREF();
        CHECK_REF(sty);

        EVAL_SYMBOL();
        auto scx = EVAL_PREF();
        CHECK_REF(scx);

        EVAL_SYMBOL();
        auto scy = EVAL_PREF();
        CHECK_REF(scy);


        EVAL_SYMBOL();
        uint8_t initFlags = 0;
        try {
                unsigned int fullscreen = std::stoul(fileRead);
                if(fullscreen > 0) fullscreen = 1;
                
                EVAL_SYMBOL();
                unsigned int softwareRender = std::stoul(fileRead);
                if(softwareRender > 0) softwareRender = 1;

                initFlags |= (fullscreen << ((int)NTHP_CORE_INIT_FULLSCREEN));
                initFlags |= (softwareRender << ((int)NTHP_CORE_INIT_SOFTWARE_RENDERING));
                
        }
        catch(std::invalid_argument) {
                PRINT_COMPILER_ERROR("Unable to evaluate boolean [%s]; Invalid Argument.\n", fileRead.c_str());
                return 1;
        }

        EVAL_SYMBOL();
        std::string title = fileRead;

        ADD_NODE(CORE_INIT);
        nodeList[currentNode].access.size = (sizeof(stdRef) * 6) + sizeof(uint8_t) + title.size() + 1;
        nodeList[currentNode].access.data = (char*)malloc(nodeList[currentNode].access.size);

// px py tx ty cx cy fs sr title
        nthp::script::instructions::stdRef* px = (decltype(px))(nodeList[currentNode].access.data);
        nthp::script::instructions::stdRef* py = (decltype(py))(nodeList[currentNode].access.data + sizeof(stdRef));
        nthp::script::instructions::stdRef* tx = (decltype(tx))(nodeList[currentNode].access.data + (sizeof(stdRef) * 2));
        nthp::script::instructions::stdRef* ty = (decltype(ty))(nodeList[currentNode].access.data + (sizeof(stdRef) * 3));
        nthp::script::instructions::stdRef* cx = (decltype(cx))(nodeList[currentNode].access.data + (sizeof(stdRef) * 4));
        nthp::script::instructions::stdRef* cy = (decltype(cy))(nodeList[currentNode].access.data + (sizeof(stdRef) * 5));
        uint8_t* flags = (decltype(flags))(nodeList[currentNode].access.data + (sizeof(stdRef) * 6));

        memcpy(nodeList[currentNode].access.data + ((sizeof(stdRef) * 6) + sizeof(uint8_t)), title.c_str(), title.size());
        nodeList[currentNode].access.data[((sizeof(stdRef) * 6) + sizeof(uint8_t) + title.size())] = '\000';

        *px = spx;
        *py = spy;
        *tx = stx;
        *ty = sty;
        *cx = scx;
        *cy = scy;
        *flags = initFlags;


        PRINT_NODEDATA();
        return 0;
}

DEFINE_COMPILATION_BEHAVIOUR(CORE_QRENDER) {
        ADD_NODE(CORE_QRENDER);

        EVAL_SYMBOL();
        auto entity = EVAL_PREF();
        CHECK_REF(entity);

        stdRef* ent = (stdRef*)nodeList[currentNode].access.data;
        *ent = entity;

        PRINT_NODEDATA();
        return 0;
}

DEFINE_COMPILATION_BEHAVIOUR(CORE_ABS_QRENDER) {
        ADD_NODE(CORE_ABS_QRENDER);

        EVAL_SYMBOL();
        auto entity = EVAL_PREF();
        CHECK_REF(entity);

        stdRef* ent = (stdRef*)nodeList[currentNode].access.data;
        *ent = entity;

        PRINT_NODEDATA();
        return 0;
}

DEFINE_COMPILATION_BEHAVIOUR(CORE_CLEAR) {
        ADD_NODE(CORE_CLEAR);

        PRINT_NODEDATA();
        return 0;
}

DEFINE_COMPILATION_BEHAVIOUR(CORE_DISPLAY) {
        ADD_NODE(CORE_DISPLAY);

        PRINT_NODEDATA();
        return 0;
}

DEFINE_COMPILATION_BEHAVIOUR(CORE_SETMAXFPS) {
        ADD_NODE(CORE_SETMAXFPS);

        EVAL_SYMBOL();
        auto fps = EVAL_PREF();
        CHECK_REF(fps);

        stdRef* _fps = (stdRef*)(nodeList[currentNode].access.data);
        *_fps = fps;

        PRINT_NODEDATA();
        return 0;
}

#define cast_stdRef(offset) (stdRef*)(nodeList[currentNode].access.data + (offset))

DEFINE_COMPILATION_BEHAVIOUR(CORE_SETWINDOWRES) {
        ADD_NODE(CORE_SETWINDOWRES);

        EVAL_SYMBOL();
        auto xRes = EVAL_PREF();
        CHECK_REF(xRes);

        EVAL_SYMBOL();
        auto yRes = EVAL_PREF();
        CHECK_REF(yRes);

        stdRef* x = cast_stdRef(0);
        stdRef* y = cast_stdRef(sizeof(stdRef));

        *x = xRes;
        *y = yRes;

        PRINT_NODEDATA();
        return 0;
}

DEFINE_COMPILATION_BEHAVIOUR(CORE_SETCAMERARES) {
        ADD_NODE(CORE_SETCAMERARES);

        EVAL_SYMBOL();
        auto xRes = EVAL_PREF();
        CHECK_REF(xRes);

        EVAL_SYMBOL();
        auto yRes = EVAL_PREF();
        CHECK_REF(yRes);

        stdRef* x = cast_stdRef(0);
        stdRef* y = cast_stdRef(sizeof(stdRef));

        *x = xRes;
        *y = yRes;

        PRINT_NODEDATA();
        return 0;
}

DEFINE_COMPILATION_BEHAVIOUR(CORE_SETCAMERAPOSITION) {
        ADD_NODE(CORE_SETCAMERAPOSITION);

        EVAL_SYMBOL();
        auto xRes = EVAL_PREF();
        CHECK_REF(xRes);

        EVAL_SYMBOL();
        auto yRes = EVAL_PREF();
        CHECK_REF(yRes);

        stdRef* x = cast_stdRef(0);
        stdRef* y = cast_stdRef(sizeof(stdRef));

        *x = xRes;
        *y = yRes;

        PRINT_NODEDATA();
        return 0;
}

DEFINE_COMPILATION_BEHAVIOUR(CORE_MOVECAMERA) {
        ADD_NODE(CORE_MOVECAMERA);

        EVAL_SYMBOL();
        auto xRes = EVAL_PREF();
        CHECK_REF(xRes);

        EVAL_SYMBOL();
        auto yRes = EVAL_PREF();
        CHECK_REF(yRes);

        stdRef* x = cast_stdRef(0);
        stdRef* y = cast_stdRef(sizeof(stdRef));

        *x = xRes;
        *y = yRes;

        PRINT_NODEDATA();
        return 0;
}



DEFINE_COMPILATION_BEHAVIOUR(ACTION_DEFINE) {
        ADD_NODE(ACTION_DEFINE);

        EVAL_SYMBOL();
        auto size = EVAL_PREF();
        CHECK_REF(size);

        stdRef* psize = (stdRef*)(nodeList[currentNode].access.data);

        *psize = size;


        PRINT_NODEDATA();
        return 0;
}


DEFINE_COMPILATION_BEHAVIOUR(ACTION_CLEAR) {
        ADD_NODE(ACTION_CLEAR);


        PRINT_NODEDATA();
        return 0;
}


DEFINE_COMPILATION_BEHAVIOUR(ACTION_BIND) {
        ADD_NODE(ACTION_BIND);

        EVAL_SYMBOL();
        auto target = EVAL_PREF();
        CHECK_REF(target);

        EVAL_SYMBOL();
        auto var = EVAL_PREF(); // Global to bind it to.
        CHECK_REF(var);

        if(!PR_METADATA_GET(var, nthp::script::flagBits::IS_GLOBAL)) {
                PRINT_COMPILER_ERROR("Second argument of ACTION_BIND must be a GLOBAL reference.\n");
                return 1;
        }

        EVAL_SYMBOL();
        int32_t key = fileRead[0]; // should correspond to keycode. idk _

        do {
                
                if(fileRead == "ESCAPE")        { key = SDLK_ESCAPE; break; }
                if(fileRead == "TAB")           { key = SDLK_TAB; break; }
                if(fileRead == "RSHIFT")        { key = SDLK_RSHIFT; break; }
                if(fileRead == "LSHIFT")        { key = SDLK_LSHIFT; break; }
                if(fileRead == "RCTRL")         { key = SDLK_RCTRL; break; }
                if(fileRead == "LCTRL")         { key = SDLK_LCTRL; break; }
                if(fileRead == "RETURN")        { key = SDLK_RETURN; break; }
                if(fileRead == "UP")            { key = SDLK_UP; break; }
                if(fileRead == "DOWN")          { key = SDLK_DOWN; break; }
                if(fileRead == "LEFT")          { key = SDLK_LEFT; break; }
                if(fileRead == "RIGHT")         { key = SDLK_RIGHT; break; }
                if(fileRead == "SPACE")         { key = SDLK_SPACE; break; }

        } while(0);

        stdRef* _target = (stdRef*)(nodeList[currentNode].access.data);
        uint32_t* _varIndex = (uint32_t*)(nodeList[currentNode].access.data + sizeof(stdRef));
        int32_t* _key = (int32_t*)(nodeList[currentNode].access.data + sizeof(stdRef) + sizeof(uint32_t));

        *_target = target;
        *_varIndex = (uint32_t)nthp::fixedToInt(var.value);
        *_key = key;


        PRINT_NODEDATA();
        return 0;
}

DEFINE_COMPILATION_BEHAVIOUR(CORE_STOP) {
        ADD_NODE(CORE_STOP);

        PRINT_NODEDATA();
        return 0;
}

DEFINE_COMPILATION_BEHAVIOUR(STAGE_LOAD) {
        ADD_NODE(STAGE_LOAD);

        EVAL_SYMBOL();
        std::string stageName = fileRead;
        nodeList[currentNode].access.size = stageName.size() + 1;
        nodeList[currentNode].access.data = (char*)malloc(nodeList[currentNode].access.size);

        memcpy(nodeList[currentNode].access.data, stageName.c_str(), stageName.size());
        nodeList[currentNode].access.data[stageName.size()] = '\000';

        PRINT_NODEDATA();
        return 0;
}


DEFINE_COMPILATION_BEHAVIOUR(POLL) {

        EVAL_SYMBOL();
        auto entity = EVAL_PREF();
        CHECK_REF(entity);

        // What to check for
        EVAL_SYMBOL();
        do {
                if(fileRead == "POS")           { ADD_NODE(POLL_ENT_POSITION); break; }
                if(fileRead == "HITBOX")        { ADD_NODE(POLL_ENT_HITBOX); break; }
                if(fileRead == "CURRENTFRAME")  { ADD_NODE(POLL_ENT_CURRENTFRAME); break; }
                if(fileRead == "RENDERSIZE")    { ADD_NODE(POLL_ENT_RENDERSIZE); break; }

                PRINT_COMPILER_ERROR("[%s] Invalid POLL request.", fileRead.c_str());
                return 1;

        } while(0);

        indRef* ent = (indRef*)(nodeList[currentNode].access.data);
        ent->value = nthp::fixedToInt(entity.value);
        ent->metadata = entity.metadata;

        PRINT_NODEDATA();

        return 0;
}



DEFINE_COMPILATION_BEHAVIOUR(DRAW_SETCOLOR) {
        ADD_NODE(DRAW_SETCOLOR);

        EVAL_SYMBOL();
        auto _colorIndex = EVAL_PREF();
        CHECK_REF(_colorIndex);

        if(nthp::getFixedDecimal(_colorIndex.value) > 0) {
                PRINT_COMPILER_WARNING("DRAW_SETCOLOR uses an invalid decimal value. Discarding decimal.\n");
                _colorIndex.value = nthp::getFixedInteger(_colorIndex.value);
        }

        stdRef* colorIndex = (stdRef*)(nodeList[currentNode].access.data);
        *colorIndex = _colorIndex;


        PRINT_NODEDATA();
        return 0;
}


DEFINE_COMPILATION_BEHAVIOUR(DRAW_LINE) {
        ADD_NODE(DRAW_LINE);

        EVAL_SYMBOL();
        auto _x1 = EVAL_PREF();
        CHECK_REF(_x1);

        EVAL_SYMBOL();
        auto _y1 = EVAL_PREF();
        CHECK_REF(_y1);

        EVAL_SYMBOL();
        auto _x2 = EVAL_PREF();
        CHECK_REF(_x2);

        EVAL_SYMBOL();
        auto _y2 = EVAL_PREF();
        CHECK_REF(_y2);

        stdRef* x1 = (stdRef*)(nodeList[currentNode].access.data);
        stdRef* y1 = (stdRef*)(nodeList[currentNode].access.data + sizeof(stdRef));
        stdRef* x2 = (stdRef*)(nodeList[currentNode].access.data + sizeof(stdRef) + sizeof(stdRef));
        stdRef* y2 = (stdRef*)(nodeList[currentNode].access.data + sizeof(stdRef) + sizeof(stdRef) + sizeof(stdRef));

        *x1 = _x1;
        *y1 = _y1;
        *x2 = _x2;
        *y2 = _y2;


        PRINT_NODEDATA();
        return 0;
}


DEFINE_COMPILATION_BEHAVIOUR(SOUND_DEFINE) {
        ADD_NODE(SOUND_DEFINE);

        EVAL_SYMBOL();
        auto size = EVAL_PREF();
        CHECK_REF(size);

        stdRef* s = (stdRef*)(nodeList[currentNode].access.data);

        *s = size;

        PRINT_NODEDATA();
        return 0;
}

DEFINE_COMPILATION_BEHAVIOUR(SOUND_CLEAR) {
        ADD_NODE(SOUND_CLEAR);

        PRINT_NODEDATA();
        return 0;
}


DEFINE_COMPILATION_BEHAVIOUR(MUSIC_DEFINE) {
        ADD_NODE(MUSIC_DEFINE);

        EVAL_SYMBOL();
        auto size = EVAL_PREF();
        CHECK_REF(size);

        stdRef* s = (stdRef*)(nodeList[currentNode].access.data);

        *s = size;

        PRINT_NODEDATA();
        return 0;
}

DEFINE_COMPILATION_BEHAVIOUR(MUSIC_CLEAR) {
        ADD_NODE(MUSIC_CLEAR);


        PRINT_NODEDATA();
        return 0;
}


DEFINE_COMPILATION_BEHAVIOUR(MUSIC_LOAD) {

        EVAL_SYMBOL();
        auto output_i = EVAL_PREF();
        CHECK_REF(output_i);


        EVAL_SYMBOL();
        std::string audioFile = fileRead;

        if(audioFile.size() > UINT8_MAX - sizeof(nthp::script::instructions::stdRef)) {
		PRINT_COMPILER_ERROR("File path name in MUSIC_LOAD at [%zu] too large. Must be less than [%u] characters.", currentNode, UINT8_MAX - sizeof(nthp::script::instructions::stdRef));
		return 1;
	}

        ADD_NODE(MUSIC_LOAD);
        nodeList[currentNode].access.size = sizeof(stdRef) + audioFile.size() + 1;
        nodeList[currentNode].access.data = (char*)(malloc(nodeList[currentNode].access.size));

        stdRef* output = (stdRef*)(nodeList[currentNode].access.data);

        memcpy(nodeList[currentNode].access.data + sizeof(stdRef), audioFile.c_str(), audioFile.size());
        nodeList[currentNode].access.data[nodeList[currentNode].access.size - 1] = '\000';

        *output = output_i;
        output->value = nthp::getFixedInteger(output->value);


        PRINT_NODEDATA();
        return 0;
}


DEFINE_COMPILATION_BEHAVIOUR(SOUND_LOAD) {

        EVAL_SYMBOL();
        auto output_i = EVAL_PREF();
        CHECK_REF(output_i);


        EVAL_SYMBOL();
        std::string audioFile = fileRead;

        if(audioFile.size() > UINT8_MAX - sizeof(nthp::script::instructions::stdRef)) {
		PRINT_COMPILER_ERROR("File path name in SOUND_LOAD at [%zu] too large. Must be less than [%u] characters.", currentNode, UINT8_MAX - sizeof(nthp::script::instructions::stdRef));
		return 1;
	}

        ADD_NODE(SOUND_LOAD);
        nodeList[currentNode].access.size = sizeof(stdRef) + audioFile.size() + 1;
        nodeList[currentNode].access.data = (char*)(malloc(nodeList[currentNode].access.size));

        stdRef* output = (stdRef*)(nodeList[currentNode].access.data);

        memcpy(nodeList[currentNode].access.data + sizeof(stdRef), audioFile.c_str(), audioFile.size());
        nodeList[currentNode].access.data[nodeList[currentNode].access.size - 1] = '\000';

        *output = output_i;
        output->value = nthp::getFixedInteger(output->value);


        PRINT_NODEDATA();
        return 0;
}

DEFINE_COMPILATION_BEHAVIOUR(SOUND_PLAY) {
        ADD_NODE(SOUND_PLAY);

        EVAL_SYMBOL();
        auto index = EVAL_PREF();
        CHECK_REF(index);

        stdRef* output = (stdRef*)nodeList[currentNode].access.data;

        *output = index;
        
        PRINT_NODEDATA();
        return 0;
}

DEFINE_COMPILATION_BEHAVIOUR(MUSIC_START) {
        ADD_NODE(MUSIC_START);

        EVAL_SYMBOL();
        auto index = EVAL_PREF();
        CHECK_REF(index);

        stdRef* output = (stdRef*)nodeList[currentNode].access.data;

        *output = index;
        
        PRINT_NODEDATA();
        return 0;
}


DEFINE_COMPILATION_BEHAVIOUR(MUSIC_STOP) {
        ADD_NODE(MUSIC_STOP);

        
        PRINT_NODEDATA();
        return 0;
}


DEFINE_COMPILATION_BEHAVIOUR(MUSIC_PAUSE) {
        ADD_NODE(MUSIC_PAUSE);

        
        PRINT_NODEDATA();
        return 0;
}


DEFINE_COMPILATION_BEHAVIOUR(MUSIC_RESUME) {
        ADD_NODE(MUSIC_RESUME);

        
        PRINT_NODEDATA();
        return 0;
}


DEFINE_COMPILATION_BEHAVIOUR(CACHE_CREATE) {
        ADD_NODE(CACHE_CREATE);

        EVAL_SYMBOL();
        auto size = EVAL_PREF();
        CHECK_REF(size);

        stdRef* _size = (stdRef*)(nodeList[currentNode].access.data);

        *_size = size;

        PRINT_NODEDATA();
        return 0;
}

DEFINE_COMPILATION_BEHAVIOUR(CACHE_RESIZE) {
        ADD_NODE(CACHE_RESIZE);

        EVAL_SYMBOL();
        auto size = EVAL_PREF();
        CHECK_REF(size);

        stdRef* _size = (stdRef*)(nodeList[currentNode].access.data);

        *_size = size;

        PRINT_NODEDATA();
        return 0;
}

DEFINE_COMPILATION_BEHAVIOUR(CACHE_OPEN) {
        ADD_NODE(CACHE_OPEN);
        EVAL_SYMBOL();


        nodeList[currentNode].access.size = fileRead.size() + 1;
        nodeList[currentNode].access.data = (char*)malloc(nodeList[currentNode].access.size);

        memcpy(nodeList[currentNode].access.data, fileRead.c_str(), fileRead.size());
        nodeList[currentNode].access.data[nodeList[currentNode].access.size - 1] = '\000';


        PRINT_NODEDATA();
        return 0;
}


DEFINE_COMPILATION_BEHAVIOUR(CACHE_CLEAR) {
        ADD_NODE(CACHE_CLEAR);

        PRINT_NODEDATA();
        return 0;
}


DEFINE_COMPILATION_BEHAVIOUR(CACHE_WRITE) {
        ADD_NODE(CACHE_WRITE);

        EVAL_SYMBOL();
        auto cache_target = EVAL_PREF();
        CHECK_REF(cache_target);

        EVAL_SYMBOL();
        auto value = EVAL_PREF();
        CHECK_REF(value);


        stdRef* _cache_target = (stdRef*)(nodeList[currentNode].access.data);
        stdRef* _value = (stdRef*)(nodeList[currentNode].access.data + sizeof(stdRef));

        *_cache_target = cache_target;
        *_value = value;


        PRINT_NODEDATA();
        return 0;        
}


DEFINE_COMPILATION_BEHAVIOUR(CACHE_READ) {
        ADD_NODE(CACHE_READ);


        EVAL_SYMBOL();
        auto cache_target = EVAL_PREF();
        CHECK_REF(cache_target);


        EVAL_SYMBOL();
        auto var_target = EVAL_PREF();
        CHECK_REF(var_target);

        if(!PR_METADATA_GET(var_target, nthp::script::flagBits::IS_REFERENCE)) {
                PRINT_COMPILER_ERROR("Argument 2 CACHE_READ; Must be a reference.\n");
                return 1;
        }

        stdRef* _cache_target = (stdRef*)(nodeList[currentNode].access.data);
        indRef* _var_target = (indRef*)(nodeList[currentNode].access.data + sizeof(stdRef));


        *_cache_target = cache_target;
        _var_target->metadata = var_target.metadata;
        _var_target->value = nthp::fixedToInt(var_target.value);

        PRINT_NODEDATA();
        return 0;
}


DEFINE_COMPILATION_BEHAVIOUR(CACHE_SAVE) {
        ADD_NODE(CACHE_SAVE);
        EVAL_SYMBOL();


        nodeList[currentNode].access.size = fileRead.size() + 1;
        nodeList[currentNode].access.data = (char*)malloc(nodeList[currentNode].access.size);

        memcpy(nodeList[currentNode].access.data, fileRead.c_str(), fileRead.size());
        nodeList[currentNode].access.data[nodeList[currentNode].access.size - 1] = '\000';


        PRINT_NODEDATA();
        return 0;
}


DEFINE_COMPILATION_BEHAVIOUR(PRINT) {
        ADD_NODE(PRINT);

        EVAL_SYMBOL();
        auto output = EVAL_PREF();
        CHECK_REF(output);

        stdRef* out = (stdRef*)(nodeList[currentNode].access.data);
        *out = output;

        PRINT_NODEDATA();
        return 0;
}



// COMPILER INSTANCE BEHAVIOUR GOES HERE                ||
//                                                      VV



int nthp::script::CompilerInstance::compileSourceFile(const char* inputFile, const char* outputFile, bool buildSystemContext, const bool ignoreInstructionData) {
        NOVERB_PRINT_COMPILER("\n\tCompiling Source File [%s]...\n\n", inputFile);
        
        std::fstream file(inputFile, std::ios::in);
        if(file.fail()) {
                PRINT_COMPILER_ERROR("Compilation failed; input source file not found.\n");
                return 1;
        }
        std::string fileRead;
        
        if(!buildSystemContext) { 
                globalList.clear(); 
                macroList.clear();
                constantList.clear();
        }
        labelList.clear();
        gotoList.clear();
        varList.clear();


        nthp::script::cleanNodeSet(nodeList);

        size_t globalAlloc = 0;


        
        std::vector<size_t> ifLocations;
        std::vector<size_t> endLocations;
        std::vector<size_t> skipList;


        struct callStackObj {
                std::string file;
                std::streampos pos;
                bool importing;
        };
        std::vector<callStackObj> callStack;
        std::string currentFile = inputFile;
        bool inCalledFile = false;


        size_t currentMacroPosition = 0;
        size_t targetMacro = 0;
        bool evaluateMacro = false;


        #define COMPILE(instruction) if( instruction ( nodeList, file, fileRead, constantList, macroList, varList, globalList, labelList, gotoList, ifLocations, endLocations, skipList, currentMacroPosition, targetMacro, evaluateMacro, buildSystemContext) ) return 1
        #define CHECK_COMP(instruction) if(fileRead == #instruction) { COMPILE(instruction); continue; }

        bool operationOngoing = true;

        if(!ignoreInstructionData) COMPILE(HEADER);

        while(operationOngoing) {
                COMP_START:
                

                if(skipInstructionCheck) {
                        PRINT_COMPILER_WARNING("Skipping EVAL this pass...\n");

                        skipInstructionCheck = false;
                }
                else {
                        EVAL_SYMBOL();
                }

                if(fileRead == "/") { // Keep cycling arguments until another / is read (for comments)
                       do { EVAL_SYMBOL(); } while(fileRead != "/");
                       continue;
                }
                PRINT_DEBUG("Eval. Symbol; [%s] from [%s]\n", fileRead.c_str(), currentFile.c_str());
                


                if(fileRead == "EXIT") {
                        if(inCalledFile) {
                                if(callStack.size() > 0) {
                                        currentFile = callStack.back().file;
                                        file.close();

                                        file.open(currentFile, std::ios::in);
                                        if(file.fail()) { PRINT_COMPILER_ERROR("Critical Failure returning CALL;\n"); return 1; }
                                        file.seekg(callStack.back().pos);
                                        EVAL_SYMBOL();

                                        callStack.pop_back();
                                        if(callStack.size() == 0) { inCalledFile = false; currentFile = inputFile; }
                                }
                                PRINT_COMPILER("CALL/IMPORT Complete.\n");
                        }
                        else {
                                if(!ignoreInstructionData) COMPILE(EXIT);
                                operationOngoing = false;
                                break;
                        }
                }

                // Evaluate Compiler Definitions first.
                if(fileRead == "VAR") {
                        // Define a new variable.
                        EVAL_SYMBOL();

                        for(size_t i = 0; i < varList.size(); ++i) {
                                if(fileRead == varList[i].varName) {
                                        PRINT_COMPILER_WARNING("VAR [$%s] already declared; Ignoring redefinition.\n", fileRead.c_str());
                                        goto COMP_START; // thank god.
                                }
                        }


                        VAR_DEF newDef;
                        newDef.varName = fileRead;
                        newDef.relativeIndex = varList.size();

                        varList.push_back(newDef);
                        PRINT_COMPILER("Defined VAR [%s].\n", fileRead.c_str());
                }
                if(fileRead == "GLOBAL") {
                        
                        // Define a new variable.
                        EVAL_SYMBOL();
                        bool invalidDefine = false;

                        for(size_t i = 0; i < globalList.size(); ++i) {
                                if(fileRead == globalList[i].varName) {
                                        PRINT_COMPILER_WARNING("GLOBAL [>%s] already declared; Ignoring redefinition.\n", fileRead.c_str());
                                        invalidDefine = true;
                                        break;
                                }
                        }

                        if(invalidDefine) continue;

                        PRINT_COMPILER("Defined GLOBAL [%s].\n", fileRead.c_str());

                        addGlobalDef(fileRead.c_str());
                        ++globalAlloc;
                        continue;
                }


                if(fileRead == "CONST") {
                        // Define new Const sub.

                        EVAL_SYMBOL();  // Name
                        CONST_DEF newDef;

                        fileRead = '#' + fileRead;
                        newDef.constName = fileRead;
                        for(size_t i = 0; i < constantList.size(); ++i) {
                                if(newDef.constName == constantList[i].constName) {
                                        PRINT_COMPILER("Redefinition of CONST [%s];", newDef.constName.c_str());
                                        EVAL_SYMBOL(); // Substitution

                                        constantList[i].value = fileRead;
                                        NOVERB_PRINT_COMPILER(" sub.= %s\n", fileRead.c_str());
                                        goto COMP_START;
                                }
                        }

                        EVAL_SYMBOL(); // Substitution
                        newDef.value = fileRead;


                        constantList.push_back(newDef);
                        PRINT_COMPILER("New CONST Definition; n=%s sub=%s\n", newDef.constName.c_str(), newDef.value.c_str());
                }

                if(fileRead == "UNDEF") {
                        EVAL_SYMBOL();
                        undefConstant(fileRead.c_str(), constantList);
                }

                if(fileRead == "MACRO") {
                        // Define new Macro.
                        EVAL_SYMBOL();          // Name
                        MACRO_DEF newDef;

                        newDef.macroName = '@' + fileRead;


                        for(size_t i = 0; i < macroList.size(); ++i) {
                                if(macroList[i].macroName == newDef.macroName) {
                                        PRINT_COMPILER_WARNING("Duplicate MACRO [%s] at [~%zu]; Ignoring Definition.\n", macroList[i].macroName.c_str(), nodeList.size());
                                        do {file >> fileRead; } while(fileRead != "}");
                                        goto COMP_START;
                                }
                        }

                        PRINT_COMPILER("Defining new MACRO [%s]...", fileRead.c_str());

                        READ_FILE();     // Gets rid of the '{'
                        for(size_t i = 0; fileRead != "}"; ++i) {
                                READ_FILE();
                                if(fileRead == "/") { do { READ_FILE(); } while(fileRead != "/"); EVAL_SYMBOL(); }
                                newDef.macroData.push_back(fileRead);
                                
                                if((i % 5) == 0) { NOVERB_PRINT_COMPILER("\n\t"); }

                                NOVERB_PRINT_COMPILER(" [%s]", fileRead.c_str());
                        }
                        newDef.macroData.pop_back(); // Remove the '}'
                        macroList.push_back(newDef);

                        NOVERB_PRINT_COMPILER("\n");
                        PRINT_COMPILER("Added MACRO [%s] to MACRO list.\n", macroList.back().macroName.c_str());
                        continue;

                }

                if(fileRead == "CALL") {
                        callStackObj newFile;
                        newFile.file = currentFile;
                        newFile.pos = file.tellg();
                        newFile.importing = false;

                        callStack.push_back(newFile);
                        PRINT_COMPILER("Added file [%s] to CallStack.\n", callStack.back().file.c_str());

                        EVAL_SYMBOL(); //filename 
                        file.close();

                        PRINT_COMPILER("Beginning CALL Operation to file [%s]...\n", fileRead.c_str());

                        file.open(fileRead, std::ios::in);
                        if(file.fail()) {
                                PRINT_COMPILER_ERROR("Unable to IMPORT; File not found.\n");
                                return 1;
                        }
                        currentFile = fileRead;
                        inCalledFile = true;

                        continue;
                }

                if(fileRead == "IMPORT") {
                        EVAL_SYMBOL(); // name

                        callStackObj newFile;
                        newFile.file = currentFile;
                        newFile.pos = file.tellg();
                        newFile.importing = true;

                        callStack.push_back(newFile);
                
                        file.close();
                        
                        PRINT_COMPILER("Beginning IMPORT Operation to file [%s]...\n", fileRead.c_str());

                        file.open(fileRead, std::ios::in);
                        if(file.fail()) {
                                PRINT_COMPILER_ERROR("Unable to IMPORT; File not found.\n");
                                return 1;
                        }

                        currentFile = fileRead;
                        inCalledFile = true;

                        continue;
                }
                

                if(fileRead == "DEPEND") {
                        file >> fileRead;
                        size_t i = 0;
                        switch(fileRead.c_str()[0]) {
                                case '$':
                                        {
                                        fileRead.erase(fileRead.begin());
                                        for(; i < varList.size(); ++i) {
                                                if(fileRead == varList[i].varName) break;
                                        }
                                        if(i == varList.size()) {
                                                PRINT_COMPILER_DEPEND_ERROR("VAR Dependency [%s] not declared; Dependency check failed.\n", fileRead.c_str());
                                                return 1;
                                        }
                                        }
                                        break;
                                case '@': 
                                        {
                                        for(; i < macroList.size(); ++i) {
                                                if(fileRead == macroList[i].macroName) break;
                                        }
                                        if(i == varList.size()) {
                                                PRINT_COMPILER_DEPEND_ERROR("MACRO Dependency [%s] not declared; Dependency check failed.\n", fileRead.c_str());
                                                return 1;
                                        }
                                        }
                                        break;
                                case '#':
                                        {
                                        for(; i < constantList.size(); ++i) {
                                                if(fileRead == constantList[i].constName) break;
                                        }
                                        if(i == constantList.size()) {
                                                PRINT_COMPILER_DEPEND_ERROR("CONST Dependency [%s] not declared; Dependency check failed.\n", fileRead.c_str());
                                                return 1;
                                        }
                                        }
                                        break;
                                case '>':
                                        {
                                        fileRead.erase(fileRead.begin());
                                        for(; i < globalList.size(); ++i) {
                                                if(fileRead == globalList[i].varName) break;
                                        }
                                        if(i == globalList.size()) {
                                                PRINT_COMPILER_DEPEND_ERROR("GLOBAL Dependency [%s] not declared; Dependency check failed.\n", fileRead.c_str());
                                                return 1;
                                        }

                                        }
                                        break;


                                default:
                                        PRINT_COMPILER_DEPEND_ERROR("Dependency [%s] is an invalid compiler type; Dependency check failed.\n", fileRead.c_str());
                                        return 1;
                        }
                }



                if(ignoreInstructionData) {
                        continue;
                }
                if(callStack.size() > 0) {
                        if(callStack.back().importing) continue;
                }
                

                CHECK_COMP(LABEL);
                CHECK_COMP(GOTO);
                CHECK_COMP(SUSPEND);
                CHECK_COMP(JUMP);
                CHECK_COMP(GETINDEX);
                CHECK_COMP(RETURN);

                CHECK_COMP(INC);
                CHECK_COMP(DEC);
                CHECK_COMP(RSHIFT);
                CHECK_COMP(LSHIFT);

                CHECK_COMP(ADD);
                CHECK_COMP(SUB);
                CHECK_COMP(MUL);
                CHECK_COMP(DIV);
                CHECK_COMP(SQRT);

                CHECK_COMP(IF);
                CHECK_COMP(END);
                CHECK_COMP(ELSE);
                CHECK_COMP(SKIP);
                CHECK_COMP(SKIP_END);

                CHECK_COMP(SET);
                CHECK_COMP(SET_BINARY);
                CHECK_COMP(CLEAR);
                CHECK_COMP(DEFINE);
                CHECK_COMP(COPY);

		CHECK_COMP(TEXTURE_DEFINE);
		CHECK_COMP(TEXTURE_CLEAR);
		CHECK_COMP(TEXTURE_LOAD);
                CHECK_COMP(SET_ACTIVE_PALETTE);


                CHECK_COMP(FRAME_DEFINE);
                CHECK_COMP(FRAME_CLEAR);
                CHECK_COMP(FRAME_SET);

                CHECK_COMP(GETGPR);
                CHECK_COMP(SM_WRITE);
                CHECK_COMP(SM_READ);

                CHECK_COMP(ENT_DEFINE);
                CHECK_COMP(ENT_CLEAR);
                CHECK_COMP(ENT_SETCURRENTFRAME);
                CHECK_COMP(ENT_SETPOS);
                CHECK_COMP(ENT_MOVE);
                CHECK_COMP(ENT_SETFRAMERANGE);
                CHECK_COMP(ENT_SETHITBOXSIZE);
                CHECK_COMP(ENT_SETHITBOXOFFSET);
                CHECK_COMP(ENT_SETRENDERSIZE);
                CHECK_COMP(ENT_CHECKCOLLISION);

                CHECK_COMP(CORE_INIT);
                CHECK_COMP(CORE_QRENDER);
                CHECK_COMP(CORE_ABS_QRENDER);
                CHECK_COMP(CORE_CLEAR);
                CHECK_COMP(CORE_DISPLAY);
                CHECK_COMP(CORE_SETMAXFPS);
                CHECK_COMP(CORE_SETWINDOWRES);
                CHECK_COMP(CORE_SETCAMERARES);
                CHECK_COMP(CORE_SETCAMERAPOSITION);
                CHECK_COMP(CORE_MOVECAMERA);
                CHECK_COMP(CORE_STOP);

                CHECK_COMP(ACTION_BIND);
                CHECK_COMP(ACTION_DEFINE);
                CHECK_COMP(ACTION_CLEAR);

                CHECK_COMP(STAGE_LOAD);

                CHECK_COMP(POLL);


                CHECK_COMP(DRAW_SETCOLOR);
                CHECK_COMP(DRAW_LINE);


                CHECK_COMP(SOUND_DEFINE);
                CHECK_COMP(SOUND_CLEAR);
                CHECK_COMP(MUSIC_DEFINE);
                CHECK_COMP(MUSIC_CLEAR);
                CHECK_COMP(MUSIC_LOAD);
                CHECK_COMP(SOUND_LOAD);
                CHECK_COMP(SOUND_PLAY);
                CHECK_COMP(MUSIC_START);
                CHECK_COMP(MUSIC_STOP);
                CHECK_COMP(MUSIC_PAUSE);
                CHECK_COMP(MUSIC_RESUME);

                CHECK_COMP(CACHE_CREATE);
                CHECK_COMP(CACHE_RESIZE);
                CHECK_COMP(CACHE_OPEN);
                CHECK_COMP(CACHE_CLEAR);
                CHECK_COMP(CACHE_WRITE);
                CHECK_COMP(CACHE_READ);
                CHECK_COMP(CACHE_SAVE);

                CHECK_COMP(PRINT);

        } // Main loop

        NOVERB_PRINT_COMPILER("\tSuccessfully compiled source file [%s].\n", inputFile);
        file.close();

        if(!ignoreInstructionData) {

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
                                PRINT_DEBUG_ERROR("Failed to link GOTO [%zu] to LABEL block. Broken GOTO created.\n", gotoList[gotoIndex].goto_position);
                                return 1;
                        }
                }

                
                // Match IFs, ENDs, and ELSEs
                {

                unsigned int numberOfIfsFound = 1;
                unsigned int numberOfEndsFound = 0;
                unsigned int finalEndIndex = 0;
                unsigned int finalElseIndex = 0;
                int waitForElse = 0;
                bool matchedElse = false;
                uint32_t* endIndex = nullptr;
                uint32_t* elseIndex = nullptr;

                for (size_t i = 0; i < ifLocations.size(); i++) {
                        NOVERB_PRINT_COMPILER("Checking IF [%zu]\n", ifLocations[i]);
                        for (finalEndIndex = ifLocations[i] + 1; numberOfIfsFound != numberOfEndsFound && finalEndIndex < nodeList.size(); ++finalEndIndex) {

                                // If an IF statement is found before and END statement, the program requires that many more ENDs
                                // to break the loop. The corresponding END will be the one found when there are equal IFs and ENDs found.
                                if (nodeList[finalEndIndex].access.ID == nthp::script::instructions::ID::END) {
                                        ++numberOfEndsFound;
                                        --waitForElse;
                                        continue;
                                }
                                if (    nodeList[finalEndIndex].access.ID == nthp::script::instructions::ID::LOGIC_EQU ||
                                        nodeList[finalEndIndex].access.ID == nthp::script::instructions::ID::LOGIC_NOT ||
                                        nodeList[finalEndIndex].access.ID == nthp::script::instructions::ID::LOGIC_GRT ||
                                        nodeList[finalEndIndex].access.ID == nthp::script::instructions::ID::LOGIC_LST ||
                                        nodeList[finalEndIndex].access.ID == nthp::script::instructions::ID::LOGIC_GRTE ||
                                        nodeList[finalEndIndex].access.ID == nthp::script::instructions::ID::LOGIC_LSTE ||
                                        nodeList[finalEndIndex].access.ID == nthp::script::instructions::ID::LOGIC_IF_TRUE
                                )
                                {
                                        ++numberOfIfsFound;
                                        ++waitForElse;
                                        continue;
                                }
                                if((nodeList[finalEndIndex].access.ID == nthp::script::instructions::ID::ELSE) && (waitForElse == 0)) {
                                        if(matchedElse) { PRINT_COMPILER_ERROR("Duplicate unmatched ELSE found while evaluating IF [%zu].\n", ifLocations[i]); return 1; }

                                        finalElseIndex = finalEndIndex;
                                        matchedElse = true;
                                        continue;
                                }

                        }
                        if(finalEndIndex == nodeList.size()) {
                                PRINT_COMPILER_ERROR("Unable to find matching END. The only reason this would print is if you put an END before an IF. Absolute moron.\n");
                                return 1;
                        }

                        // Important, not a mistake. Don't go blaming this 3 months from now.
                        --finalEndIndex;

                        if(!(matchedElse)) { finalElseIndex = 0; }
                        else {
                                // No need to worry; ELSE data is allocated when the symbol is compiled.
                                (*(uint32_t*)nodeList[finalElseIndex].access.data) = finalEndIndex; // Stores the END location in the ELSE for easy redirection.

                        }


                        
                        // Assigns the pointer to the last 4 bytes of the node to store the end index (Unless a BNE instruction).
                        if(nodeList[ifLocations[i]].access.ID == nthp::script::instructions::ID::LOGIC_IF_TRUE) {
                                endIndex = (uint32_t*)(nodeList[ifLocations[i]].access.data + sizeof(nthp::script::instructions::stdRef));
                                elseIndex = (uint32_t*)(nodeList[ifLocations[i]].access.data + sizeof(nthp::script::instructions::stdRef) + sizeof(uint32_t));

                        }
                        else {
                                endIndex = (uint32_t*)(nodeList[ifLocations[i]].access.data + sizeof(nthp::script::instructions::stdRef) + sizeof(nthp::script::instructions::stdRef));
                                elseIndex = (uint32_t*)(nodeList[ifLocations[i]].access.data + sizeof(nthp::script::instructions::stdRef) + sizeof(nthp::script::instructions::stdRef) + sizeof(uint32_t));
                        }

                        NOVERB_PRINT_COMPILER("Matched IF to END at [%u].\n", finalEndIndex);


                        *endIndex = finalEndIndex;
                        *elseIndex = finalElseIndex;
                        numberOfIfsFound = 1;
                        numberOfEndsFound = 0;
                        waitForElse = 0;
                        matchedElse = false;

                } // For

                }

                {
                        uint32_t skip_endLocation = 0;
                        uint32_t* skipEndWrite = NULL;

                       // Match SKIPs and SKIP_ENDs.
                        for(size_t i = 0; i < skipList.size(); ++i) {
                                for(skip_endLocation = skipList[i]; (nodeList[skip_endLocation].access.ID != nthp::script::instructions::ID::SKIP_END) && (skip_endLocation < nodeList.size()); ++skip_endLocation) 
                                {
                                        continue;
                                }

                                if(skip_endLocation == nodeList.size()) {
                                        PRINT_COMPILER_ERROR("SKIP at [%zu] has no matching SKIP_END flag.\n", skipList[i]);
                                        return 1;
                                }

                                skipEndWrite = (uint32_t*)(nodeList[skipList[i]].access.data);
                                *skipEndWrite = skip_endLocation;

                        }

                }

                // Set up header with:
                //      - Local Memory Budget
                //      - Global Memory Budget (if applicable)
                //      - Label List

                if(nodeList.size() > 0) {
                        nodeList[0].access.size = sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t) + (sizeof(uint32_t) * labelList.size() * 2);
                        nodeList[0].access.data = (char*)malloc(nodeList[0].access.size);

                        if(nodeList[0].access.data == NULL) {
                                FATAL_PRINT(nthp::FATAL_ERROR::Memory_Fault, "Memory Fault in Compiler.\n");
                        }

                        uint32_t* localmem = (decltype(localmem))(nodeList[0].access.data);
                        uint32_t* globalmem = (decltype(globalmem))(nodeList[0].access.data + sizeof(uint32_t));
                        uint32_t* labelstart = (decltype(labelstart))(nodeList[0].access.data + (sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t)));
                        uint32_t* labelSize = (decltype(labelstart))(nodeList[0].access.data + (sizeof(uint32_t) + sizeof(uint32_t)));
                        *labelSize = labelList.size();


                        *localmem = (uint32_t)varList.size();
                        *globalmem = (uint32_t)globalAlloc;

                        // Writes label data to header. For use in JUMP.
                        for(size_t i = 0; i < labelList.size(); ++i) {
                                labelstart[i + i] = labelList[i].ID;
                                labelstart[(i + i) + 1] = labelList[i].label_position;
                        }
                }



                if(!buildSystemContext) {
                        PRINT_COMPILER("Allocating and Copying node data to safe container...");
                        nodeBlockSize = nodeList.size();
                        compiledNodes = (decltype(compiledNodes))malloc(NodeSize * nodeBlockSize);

                        memcpy(compiledNodes, nodeList.data(), NodeSize * nodeBlockSize);
                        nodeList.clear(); // Destroys node headers, but data is untouched and copied to the raw memory above.

                        NOVERB_PRINT_COMPILER("\t\nAllocated & copied [%zu] bytes at [%p].\n", NodeSize * nodeBlockSize, compiledNodes);
                }

                if(outputFile != NULL) {
                        if(buildSystemContext)
                                return exportToFile(outputFile, &nodeList, buildSystemContext);
                        else
                                return exportToFile(outputFile, NULL, buildSystemContext);
                }
        }
        
        return 0;

}

int nthp::script::CompilerInstance::exportToFile(const char* outputFile, std::vector<nthp::script::Node>* nodeList, bool buildSystemContext) {
        std::fstream file;
        file.open(outputFile, std::ios::out | std::ios::binary);

        if(file.fail()) {
                PRINT_COMPILER_ERROR("Unable to write compiled binary to output file.\n");
                return 1;
        }

        nthp::script::Node* target = compiledNodes;
        size_t cont_size = nodeBlockSize;
        if(nodeList != NULL) {
                target = nodeList->data();
                cont_size = nodeList->size();
        }

        for(size_t i = 0; i < cont_size; ++i) {
                file.write((char*)&target[i], sizeof(nthp::script::Node::n_file_t));
                if(target[i].access.size != 0) file.write(target[i].access.data, target[i].access.size);
        }

        file.close();

        return 0;
}



int nthp::script::CompilerInstance::compileStageConfig(const char* stageConfigFile, const char* output, bool forceBuild, const bool ignoreInstructionData) {
        std::fstream file(stageConfigFile, std::ios::in);
        if(file.fail()) {
                PRINT_COMPILER_ERROR("Failed to compile StageConfig [%s]; File not found.\n", stageConfigFile);
                return 1;
        }

        std::string targetOutput = "prog";
        if(output != NULL) targetOutput = output; // Assigned target override if not null.

        std::string fileRead;
        bool operationComplete = false;
        std::vector<nthp::script::stage::scriptConfig> triggerList;
        nthp::script::stage::scriptConfig script;

        while(!operationComplete) {
                file >> fileRead;

                if(fileRead == "SCRIPT_CONFIG") {

                        do {
                                file >> fileRead;

                                if(fileRead == "GPR") {
                                        file >> fileRead;
                                        try {
                                                script.trig.GPR = std::stoul(fileRead);
                                        }
                                        catch(std::invalid_argument) {
                                                PRINT_COMPILER_ERROR("Invalid GPR Argument.\n");
                                                return 1;
                                        }
                                        continue;
                                }
                                if(fileRead == "TYPE") {
                                        file >> fileRead;
                                        if(fileRead == "T_INIT") {
                                                script.trig.ID = TRIG_INIT;
                                                continue;
                                        }
                                        if(fileRead == "T_EXIT"){
                                                script.trig.ID = TRIG_EXIT;
                                                continue;
                                        }
                                        if(fileRead == "T_TICK"){
                                                script.trig.ID = TRIG_TICK;
                                                continue;
                                        }
                                        if(fileRead == "T_LOGIC"){
                                                script.trig.ID = TRIG_LOGIC;
                                                continue;
                                        }
                                        if(fileRead == "T_HIDDEN"){
                                                script.trig.ID = TRIG_HIDDEN;
                                                continue;
                                        }
                                }
                                if(fileRead == "MEMORY") {
                                        file >> fileRead;
                                        try {
                                                script.trig.MEM = std::stoul(fileRead);
                                        }
                                        catch(std::invalid_argument) {
                                                PRINT_COMPILER_ERROR("Invalid MEM Argument.\n");
                                                return 1;
                                        }
                                        continue;
                                }

                                if(fileRead == "FILE") {
                                        file >> fileRead;
                                        script.scriptFile = fileRead;

                                }

                        } while(fileRead != "END");

                        triggerList.push_back(script);

                } // if(fileRead == "SCRIPT_CONFIG")

                if(fileRead == "TARGET") {
                        file >> fileRead;
                        targetOutput = fileRead;

                        PRINT_COMPILER("Output target override; target set to [%s].\n", fileRead.c_str());
                        continue;
                }


                if(fileRead == "BUILD_SYSTEM") {
                        globalList.clear();


                        

                        // Add constant runtime globals.
                        addGlobalDef("mousepos_x");
                        addGlobalDef("mousepos_y");
                        addGlobalDef("deltatime");
                        addGlobalDef("mouse1");
                        addGlobalDef("mouse2");
                        addGlobalDef("mouse3");
                        addGlobalDef("r_poll1");
                        addGlobalDef("r_poll2");
                        addGlobalDef("r_poll3");
                        addGlobalDef("r_poll4");


                        while(!file.eof()) {
                                file >> fileRead;
                                if(fileRead == "END") { break; }

                                std::string output;
                                file >> output;

                                // Ignore output file of compilation; no instructions to write.
                                if(ignoreInstructionData) {
                                        if(compileSourceFile(fileRead.c_str(), NULL, true, ignoreInstructionData)) {
                                                PRINT_DEBUG_ERROR("Compiler failure in source file [%s]; aborting.\n", fileRead.c_str());
                                                return 1;
                                        }
                                }
                                else {
                                        if(compileSourceFile(fileRead.c_str(), output.c_str(), true, false)) {
                                                if(forceBuild) {
                                                        PRINT_DEBUG_WARNING("Compiler failure in source file [%s]; forcing continue...\n", fileRead.c_str());
                                                }
                                                else {
                                                        PRINT_DEBUG_ERROR("Compiler failure in source file [%s]; aborting.\n", fileRead.c_str());
                                                        return 1;
                                                }
                                        }
                                }
                        }
                        


                } // if (BUILD_SYSTEM)

                if(fileRead == "EXIT" || file.eof()) {
                        operationComplete = true;
                }
        } // while(!operationComplete)

        file.close();
        if(output == NULL && ignoreInstructionData) {
                return 0;
        }

        file.open(targetOutput, std::ios::out | std::ios::binary);
        stageOutputTarget = targetOutput;
        
        nthp::script::stage::trigger_w primTrig = 0;
        uint8_t fileLength = 0;
        for(size_t i = 0; i < triggerList.size(); ++i) {
                primTrig = 0;
                fileLength = 0;
                TRIGGER_SETID(primTrig, triggerList[i].trig.ID);
                TRIGGER_SETGPR(primTrig, triggerList[i].trig.GPR);
                TRIGGER_SETMEM(primTrig, triggerList[i].trig.MEM);
                fileLength = triggerList[i].scriptFile.size();

                file.write((char*)&primTrig, sizeof(primTrig));
                file.write((char*)&fileLength, sizeof(fileLength));
                file.write((char*)triggerList[i].scriptFile.c_str(), fileLength);

        }

        primTrig = 0;
        TRIGGER_SETID(primTrig, TRIG_END);
        file.write((char*)&primTrig, sizeof(primTrig));

        file.close();
        return 0;

}






nthp::script::CompilerInstance::~CompilerInstance() {
        // Node tokens are copied into script objects when loaded;
        // the stored symbols of the compiler is purely for debugging.
        // Free to destroy.
        nthp::script::cleanNodeSet(nodeList);

        if(nodeBlockSize > 0) {
                for(size_t i = 0; i < nodeBlockSize; ++i) {
                        if(compiledNodes[i].access.size > 0)
                                free(compiledNodes[i].access.data);
                }
                
                
                free(compiledNodes);
                
        }

        
}
