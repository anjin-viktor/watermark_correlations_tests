#ifndef DCT_H_
#define DCT_H_

#include <vector>
#include <cstdint>

std::vector<float> DCT(std::vector<uint8_t> block, std::size_t blockSize);
std::vector<uint8_t> iDCT(std::vector<float> block, std::size_t blockSize);



#endif