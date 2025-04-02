#include "s_linker.hpp"




nthp::script::LinkerInstance::LinkerInstance(std::vector<std::string>& targets, const char* output) {
        if(linkFiles(targets, output)) {
                PRINT_DEBUG_ERROR("Failed to link compiled source files.\n");

        }


}


int nthp::script::LinkerInstance::linkFiles(std::vector<std::string>& targets, const char* output) {
        const size_t scriptDataSize = targets.size();
        try {
                scriptData = new std::vector<nthp::script::Node>[scriptDataSize];
        } 
        catch(std::bad_alloc x) {
                PRINT_DEBUG_ERROR("Failed to allocate scriptData in linker [%p]; %s\n", this, x.what());
                return 1;
        }


        std::fstream file;
        nthp::script::Node read;
        uint32_t finalSize = 0;
        
        for(size_t cObject = 0; cObject < scriptDataSize; ++cObject) {
                PRINT_DEBUG("Importing file [%s]...\n", targets[cObject].c_str());
                file.open(targets[cObject], std::ios::in | std::ios::binary);

                if(file.fail()) {
                        PRINT_DEBUG_ERROR("Unable to access file [%s] to link.\n", targets[cObject].c_str());
                        return 1;
                }

                
                for(size_t i = 0; read.access.ID != GET_INSTRUCTION_ID(EXIT); ++i) {
                        ++finalSize;

                        file.read((char*)&read, sizeof(nthp::script::Node::n_file_t));
                        scriptData[cObject].push_back(read);
                        NOVERB_PRINT_DEBUG("\t[%zu] ID:%u Size:%u\n", i, read.access.ID, read.access.size);
                        
                        if(scriptData[cObject].back().access.size != 0) {
                                scriptData[cObject].back().access.data = (char*)malloc(scriptData[cObject].back().access.size);
                                file.read(scriptData[cObject].back().access.data, scriptData[cObject].back().access.size);
                        }

                        else scriptData[cObject].back().access.data = nullptr;

                }

                read.access.ID = GET_INSTRUCTION_ID(HEADER);
                file.close();
        }

        nthp::script::LinkerInstance::link_header header;
        header.scriptCount = scriptDataSize;
        header.totalNodeCount = finalSize;

        file.open(output, std::ios::out | std::ios::binary);

        if(file.fail()) {
                PRINT_DEBUG_ERROR("Unable to open file [%s].\n", output);
                return 1;
        }

        file.write((char*)&header, sizeof(header));

        for(size_t cObject = 0; cObject < scriptDataSize; ++cObject) {

                for(size_t i = 0; i < scriptData[cObject].size(); ++i) {
                        file.write((char*)&scriptData[cObject].data()[i], sizeof(nthp::script::Node::n_file_t));
                        if(scriptData[cObject].data()[i].access.size != 0) file.write(scriptData[cObject].data()[i].access.data, scriptData[cObject].data()[i].access.size);
                }
        }

        file.close();
                

        return 0;
}