#define SUPPRESS_DEBUG_OUTPUT

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

                for(loopIndex = 1; loopIndex < c_maxBlockSize; ++loopIndex) {
                        if(stData[dataIndex + loopIndex] != currentNodeData) break;
                        else continue;
                }
                totalSize += loopIndex - 1;

                // By this point, create a new node with 'currentNodeData' and size 'loopIndex'
                dataIndex += loopIndex - 1;
                nodes.push_back(nthp::texture::compression::Node(loopIndex, currentNodeData));

                NOVERB_PRINT_DEBUG("\tProgress [%lf%%]: Created Node s=%zu d=%u\n", ((double)dataIndex / (double)stDataSize) * (double)100, loopIndex, currentNodeData);
        }
        PRINT_DEBUG("Complete. Total data width accounted: %zu units.\n\tTarget data width: %u\n", totalSize, texture->getMetaData().x * texture->getMetaData().y);

        std::fstream file(output, std::ios::out | std::ios::binary);

        nthp::texture::compression::CST_header header;
        header.signature = CSTHeaderSignature;
        header.x = texture->getMetaData().x;
        header.y = texture->getMetaData().y;
        header.nodeCount = nodes.size();

        file.write((char*)&header, sizeof(header));
        file.write((char*)nodes.data(), nodes.size() * sizeof(nthp::texture::compression::Node));

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
        PRINT_DEBUG("Attempting decompression of cst file [%s]...\n", filename);

        if(file.fail()) {
                PRINT_DEBUG_ERROR("Unable to decompress texture [%s]; Unable to open file.\n", filename);
                return NULL;
        }

        nthp::texture::compression::CST_header header;
        file.read((char*)&header, sizeof(header));

        if(header.signature != CSTHeaderSignature) {
                PRINT_DEBUG_ERROR("Unable to decompress texture [%s]; Inavlid file format.\n", filename);
                return NULL;
        }

        const size_t pixelDataSize = header.x * header.y;
        PRINT_DEBUG("Detected Header data; NodeCount = %llu\n", header.nodeCount);

        nthp::texture::SoftwareTexture* texture = (nthp::texture::SoftwareTexture*)malloc(sizeof(nthp::texture::SoftwareTexture));
        texture->createEmptyTexture(pixelDataSize);
        {
                nthp::texture::SoftwareTexture::software_texture_header conv_header;
                conv_header.signature = nthp::texture::SoftwareTexture::STheaderSignature;
                conv_header.x = header.x;
                conv_header.y = header.y;
                texture->manual_metadata_override(conv_header);
        }

        nthp::texture::compression::Node currentNode;
        size_t currentAbsolutePosition = 0;

        for(size_t i = 0; i < header.nodeCount; ++i) {
                file.read((char*)&currentNode, sizeof(currentNode));
                PRINT_DEBUG("Expanding node [%zu] : s=%u d=%u...", i, currentNode.blockSize, currentNode.data);

                for(c_BlockWidth nEI = 0; nEI < currentNode.blockSize; ++nEI) {
                        texture->getPixelData()[currentAbsolutePosition] = currentNode.data;
                        ++currentAbsolutePosition;
                }
                NOVERB_PRINT_DEBUG("\t\t\tDone.\n");
        }

        file.close();

        PRINT_DEBUG("Successfully decompressed softwareTexture; Output at [%p].\nTotal iterations: %llu.\n", texture, currentAbsolutePosition);
        
        
        return texture;
}


nthp::texture::SoftwareTexture* nthp::texture::auto_generateTexture(const char* filename) {
        std::fstream file(filename, std::ios::in | std::ios::binary);

        if(file.fail()) {
                PRINT_DEBUG_ERROR("Unable to generate texture [%s]; File not accessible.\n", filename);
                return NULL;
        }

        nthp::texture::SoftwareTexture::software_texture_header header;
        file.read((char*)&header, sizeof(header));

        file.close();

        switch(header.signature) {
                case nthp::texture::SoftwareTexture::STheaderSignature:
                        
                        return new nthp::texture::SoftwareTexture(filename);
                        break;
                
                case nthp::texture::compression::CSTHeaderSignature:
                        return nthp::texture::compression::decompressTexture(filename);
                        break;


                default:
                        PRINT_DEBUG_ERROR("Unable to generate texture [%s]; Invalid file format.\n");
                        return NULL;
                        break;
        }
}