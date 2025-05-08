#include <boost/test/unit_test.hpp>

#include <memory>

#include "VideoFrame.h"
#include "DCT.h"
#include "VideoFrameBlockProcessor.h"
#include "Utils.h"

BOOST_AUTO_TEST_SUITE(dct);

BOOST_AUTO_TEST_CASE(dct_idct)
{
  std::shared_ptr<VideoFrame> pframe = std::make_shared<VideoFrame>(getSourceDir(__FILE__) + "images/sea_640.jpg", VideoFrame::Grayscale);

  BOOST_CHECK_EQUAL(pframe->width(), 480);
  BOOST_CHECK_EQUAL(pframe->height(), 640);

  VideoFrameBlockProcessor processor(pframe, 8);
  std::vector<uint8_t> blockData = processor.getBlock(0);

  std::vector<float> dctData = DCT(blockData, 8);
  std::vector<uint8_t> idctData = iDCT(dctData, 8);

  BOOST_CHECK_EQUAL(blockData.size(), idctData.size());

  for (std::size_t i = 0; i < blockData.size(); i++)
    BOOST_CHECK(std::abs(blockData[i] - idctData[i]) <= 1);
}

BOOST_AUTO_TEST_SUITE_END();
