#ifndef MASKING_H_
#define MASKING_H_

#include <memory>
#include <vector>

#include "VideoFrame.h"

class VisualMasking
{
public:
	typedef std::vector<double> Block;
	static const int BLOCK_SIZE = 8;

	VisualMasking(std::shared_ptr<VideoFrame> pframe);
	std::vector<Block> getSBlocks();

private:
	std::vector<double> luminanceMask(std::size_t k);
	std::vector<double> contrastMask(std::size_t k);

	void calcDCCoefficients();

	std::vector<double>                   m_DCCoefficients;
	double                                m_DCAvg;
	mutable std::weak_ptr<VideoFrame>     m_pframe;

	std::vector<Block>                   m_SBlocks;

};


#endif
