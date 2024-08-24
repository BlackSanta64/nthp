#pragma once


#include "global.hpp"
#include "softwaretexture.hpp"


// A compression node's blockSize width in BYTES; The maximum number of 'C_DATA_WIDTH's
// that can be compressed into a single node.
#define C_BLOCK_WIDTH   1

// A compression node's datasize denomination; Each node contains this many bytes of
// data.
#define C_DATA_WIDTH    2




namespace nthp {
        namespace texture {
                namespace compression {
#if C_BLOCK_WIDTH == 1
                                typedef uint8_t c_BlockWidth;
                                constexpr c_BlockWidth c_maxBlockSize = UINT8_MAX;
#else
        #if C_BLOCK_WIDTH == 2
                                typedef uint16_t c_BlockWidth;
                                constexpr c_BlockWidth c_maxBlockSize = UINT16_MAX;
        #else
                #if C_BLOCK_WIDTH == 4
                                typedef uint32_t c_BlockWidth;
                                constexpr c_BlockWidth c_maxBlockSize = UINT32_MAX;
                #else
                        #if C_BLOCK_WIDTH == 8
                                typedef uint64_t c_BlockWidth;
                                constexpr c_BlockWidth c_maxBlockSize = UINT64_MAX;
                        
                        #else
                                typedef uint8_t c_BlockWidth;
                                constexpr c_BlockWidth c_maxBlockSize = UINT8_MAX;
                        #endif
                #endif
        #endif
#endif


#if C_DATA_WIDTH == 1
                                typedef uint8_t c_DataWidth;
#else
        #if C_DATA_WIDTH == 2
                                typedef uint16_t c_DataWidth;
        #else
                #if C_DATA_WIDTH == 4
                                typedef uint32_t c_DataWidth;
                #else
                        #if C_DATA_WIDTH == 8
                                typedef uint64_t c_DataWidth;
                        #else
                                typedef uint8_t c_DataWidth;
                        #endif
                #endif
        #endif
#endif

       
                struct Node {
                        Node(c_BlockWidth w, c_DataWidth d) { blockSize = w; data = d; }

                        c_BlockWidth blockSize;
                        c_DataWidth data;
                };

                extern int compressSoftwareTextureFile(const char* inputFile, const char* output);
                extern int compressSoftwareTexture(nthp::texture::SoftwareTexture* texture, const char* output);

                extern nthp::texture::SoftwareTexture* decompressTexture(const char* file);

        }
        }

}
