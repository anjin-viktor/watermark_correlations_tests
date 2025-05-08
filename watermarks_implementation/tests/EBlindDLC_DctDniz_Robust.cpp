#include <boost/test/unit_test.hpp>

#include <memory>

#include "Utils.h"
#include "VideoFrame.h"
#include "EBlindDLC_DctDniz_Robust.h"

BOOST_AUTO_TEST_SUITE(eblind_dlc_dct_dniz_robust);

BOOST_AUTO_TEST_CASE(embed_detect)
{
  std::shared_ptr<VideoFrame> pframe = std::make_shared<VideoFrame>(getSourceDir(__FILE__) + "images/sea_640.jpg", VideoFrame::Grayscale);

  BOOST_CHECK_EQUAL(pframe->width(), 480);
  BOOST_CHECK_EQUAL(pframe->height(), 640);

  std::shared_ptr<VideoFrame> pframeTrue = std::make_shared<VideoFrame>(*pframe);
  std::shared_ptr<VideoFrame> pframeFalse = std::make_shared<VideoFrame>(*pframe);

  auto preference = EBlindDLC_DctDniz_Robust::createReference(pframe, 255, 10);

  EBlindDLC_DctDniz_Robust watermark(0.3, 0.1, preference);
  watermark.embed(pframeTrue, true);
  watermark.embed(pframeFalse, false);

  BOOST_CHECK_EQUAL(watermark.detect(pframe), Watermark::DetectResult::NO_WATERMARK);
  BOOST_CHECK_EQUAL(watermark.detect(pframeTrue), Watermark::DetectResult::TRUE);
  BOOST_CHECK_EQUAL(watermark.detect(pframeFalse), Watermark::DetectResult::FALSE);
}

BOOST_AUTO_TEST_SUITE_END();
