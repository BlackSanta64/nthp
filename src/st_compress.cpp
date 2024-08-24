#include "st_compress.hpp"


// Compresses SoftwareTexture Data into a file 'output'.
//      First 8 bytes           : node count
//      byte 9 -> end of file    : nodes
int nthp::texture::compression::compressSoftwareTexture(nthp::texture::SoftwareTexture* texture, const char* output) {

        PRINT_DEBUG("Attempting compression of softwareTexture at [%p]...\n", texture);

        auto stData = texture->getPixelData();
        auto stDataSize =  texture->getPixelDataSize();

        if(stDataSize < 0 && stData == NULL) {
                PRINT_DEBUG_ERROR("Unable to compress SoftwareTexture; no data.\n");
                return -1;
        }

        // Check for redundant palette colors, make a list of them.
        std::vector<nthp::texture::compression::Node> nodes;
        
        nthp::texture::compression::c_DataWidth         currentNodeData;
        nthp::texture::compression::c_BlockWidth        currentNodeBlockSize;

        nthp::texture::compression::c_DataWidth         lastNodeData;

        size_t dataIndex = 0;
        size_t loopIndex = 0;

        size_t totalSize = 0;

        bool operationComplete = false;
        for(dataIndex = 0; dataIndex < stDataSize; ++dataIndex) {

                currentNodeData = stData[dataIndex];

                for(loopIndex = 1; loopIndex < c_maxBlockSize; ++loopIndex)
                        if(stData[dataIndex + loopIndex] != currentNodeData) break;

                // By this point, create a new node with 'currentNodeData' and size 'loopIndex'
                dataIndex += loopIndex;
                nodes.push_back(nthp::texture::compression::Node(loopIndex, currentNodeData));

                NOVERB_PRINT_DEBUG("Progress [%lf%%]: Created Node s=%zu d=%u\n", ((double)dataIndex / (double)stDataSize) * (double)100, loopIndex, currentNodeData);
        }

        std::fstream file(output, std::ios::out | std::ios::binary);
        uint64_t byteSize = nodes.size();

        file.write((char*)&byteSize, sizeof(uint64_t));
        file.write((char*)nodes.data(), nodes.size());

        file.close();

        return 0;
}

int nthp::texture::compression::compressSoftwareTextureFile(const char* input, const char* output) {
        PRINT_DEBUG("Compressing SoftwareTexture file [%s]...\n", input);
        nthp::texture::SoftwareTexture texture(input, NULL, NULL);

        return nthp::texture::compression::compressSoftwareTexture(&texture, output);
}





nthp::texture::SoftwareTexture* nthp::texture::compression::decompressTexture(const char* filename) {
        std::fstream file(filename, std::ios::in | std::ios::binary);

        if(file.fail()) {
                PRINT_DEBUG_ERROR("Unable to decompress texture [%s]; Unable to open file.\n", filename);
                return NULL;
        }

        uint64_t size;
        file.read((char*)&size, sizeof(size));

        nthp::texture::SoftwareTexture* texture = new nthp::texture::SoftwareTexture;
        texture->createEmptyTexture(size);

        
        

}