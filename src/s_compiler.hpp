#pragma once

#include "s_instructions.hpp"
#include "s_stage.hpp"


#ifdef DEBUG 

        extern void     PRINT_COMPILER                  (const char* format, ...);
        extern void     PRINT_COMPILER_ERROR            (const char* format, ...);
        extern void     PRINT_COMPILER_WARNING          (const char* format, ...);
        extern void     PRINT_COMPILER_DEPEND_ERROR     (const char* format, ...);

        #define         NOVERB_PRINT_COMPILER(...)      fprintf(NTHP_debug_output, __VA_ARGS__)

#else
        #define         PRINT_COMPILER(...)
        #define         PRINT_COMPILER_ERROR(...)
        #define         PRINT_COMPILER_WARNING(...)
        #define         NOVERB_PRINT_COMPILER(...)
        #define         PRINT_COMPILER_DEPEND_ERROR(...)
#endif




namespace nthp { 
        namespace script {

                

                class CompilerInstance {
                public:


                // Compiler-Only; Used by the compiler to track variables declared.
                struct VAR_DEF {
                        std::string varName;
                        uint32_t relativeIndex;
                };

                struct GLOBAL_DEF {
                        std::string varName;
                        uint32_t relativeIndex;
                };

                // Compiler-Only; Represents a macro-like substitution.
                struct CONST_DEF {
                        std::string constName;
                        std::string value;
                };

                // Compiler-Only; Represents a series of macro-like substitutions.
                struct MACRO_DEF {
                        std::string macroName;
                        std::vector<std::string> macroData;
                };

                struct GOTO_DEF {
                        size_t goto_position;
                        uint32_t points_to;
                };

                struct LABEL_DEF {
                        uint32_t label_position;
                        uint32_t ID;
                };


                inline nthp::script::Node* getCompiledNodes()   { return compiledNodes; }
                inline const size_t getNodeSize()               { return nodeBlockSize; }


                CompilerInstance()                              { compiledNodes = NULL; nodeBlockSize = 0; }

                // Takes source script as input, writes compiled script to output.
                // output file can be NULL, where the compiler won't write anything.
                int compileSourceFile(const char* inputFile, const char* outputFile, bool buildSystemContext, const bool ignoreInstructionData);
                
                // Writes stored Node data to the target output file.
                // Is called by 'compileSourceFile' if the 'outputfile' is non-NULL.
                int exportToFile(const char* outputFile, std::vector<nthp::script::Node>* nodeList, bool buildSystemContext);

                int compileStageConfig(const char* stageConfigFile, const char* output, bool forceBuild, const bool ignoreInstructionData);

                std::vector<nthp::script::CompilerInstance::CONST_DEF>  constantList;
                std::vector<nthp::script::CompilerInstance::MACRO_DEF>  macroList;
                std::vector<nthp::script::CompilerInstance::VAR_DEF>    varList;
                std::vector<nthp::script::CompilerInstance::GLOBAL_DEF>    globalList;  

                ~CompilerInstance();
                private:

                        nthp::script::Node* compiledNodes;
                        size_t nodeBlockSize;



                        // Labels and Goto's are matched post-compilation.
                        std::vector<nthp::script::CompilerInstance::LABEL_DEF>  labelList;
                        std::vector<nthp::script::CompilerInstance::GOTO_DEF>   gotoList;

                        std::vector<nthp::script::Node>                         nodeList;


                        inline void addGlobalDef(const char* name) {
                                GLOBAL_DEF def;
                                def.relativeIndex = globalList.size();
                                def.varName = name;

                                globalList.push_back(def);
                        }
                
                
                };

        }
}