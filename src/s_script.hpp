#pragma once

#include "s_node.hpp"
#include "s_instructions.hpp"


namespace nthp { 
namespace script {


        class Script {




        private:
                nthp::script::Node* nodeSet;
                size_t nodeSetSize;

                nthp::fixed_t* varSet;
                size_t varSetSize;

                
        };

}
}