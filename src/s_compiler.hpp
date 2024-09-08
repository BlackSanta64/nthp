#pragma once

#include "s_script.hpp"



#ifdef DEBUG 

        extern void     PRINT_COMPILER                  (const char* format, ...);
        extern void     PRINT_COMPILER_ERROR            (const char* format, ...);
        extern void     PRINT_COMPILER_WARNING          (const char* format, ...);

        #define         NOVERB_PRINT_COMPILER(...)      fprintf(NTHP_debug_output, __VA_ARGS__)

#else
        #define         PRINT_COMPILER(...)
        #define         PRINT_COMPILER_ERROR(...)
        #define         PRINT_COMPILER_WARNING(...)
        #define         NOVERB_PRINT_COMPILER(...)
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
                int compileSourceFile(const char* inputFile, const char* outputFile);
                
                // Writes stored Node data to the target output file.
                // Is called by 'compileSourceFile' if the 'outputfile' is non-NULL.
                int exportToFile(const char* outputFile);

                private:

                        nthp::script::Node* compiledNodes;
                        size_t nodeBlockSize;

                };

        }
}