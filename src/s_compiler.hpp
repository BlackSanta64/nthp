#pragma once

#include "s_script.hpp"



#ifdef DEBUG 

        extern void     PRINT_COMPILER              (const char* format, ...);
        extern void     PRINT_COMPILER_ERROR        (const char* format, ...);
        extern void     PRINT_COMPILER_WARNING      (const char* format, ...);

#else
        #define         PRINT_COMPILER          (...)
        #define         PRINT_COMPILER_ERROR    (...)
        #define         PRINT_COMPILER_WARNING  (...)

#endif






namespace nthp { 
        namespace script {

                #define DEFINE_COMPILATION_BEHAVIOUR(instruction) nthp::script::Node instruction ()


                class CompilerInstance {
                public:


                // Compiler-Only; Used by the compiler to track variables declared.
                struct VAR_DEF {
                        std::string varName;
                        size_t relativeIndex;
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
                        uint32_t label_position;
                };

                struct LABEL_DEF {
                        uint32_t label_position;
                        uint32_t labelValue;
                };

                inline nthp::script::Node* getCompiledNodes()   { return compiledNodes; }
                inline const size_t getNodeSize()               { return nodeBlockSize; }


                CompilerInstance()                              { compiledNodes = NULL; nodeBlockSize = 0; }

                int compileSourceFile(const char* filename, const char* outputFile);

                private:

                        nthp::script::Node* compiledNodes;
                        size_t nodeBlockSize;

                };

        }
}