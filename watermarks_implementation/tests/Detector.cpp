#include <boost/test/unit_test.hpp>

#include "Utils.h"
#include "VideoFrame.h"
#include "WatermarkReference.h"
#include "Detector.h"

BOOST_AUTO_TEST_SUITE(watermark_detector);

BOOST_AUTO_TEST_CASE(linear_correlation)
{
  std::shared_ptr<VideoFrame> pframe = std::make_shared<VideoFrame>(getSourceDir(__FILE__) + "images/sea_640.jpg");
  std::shared_ptr<VideoFrame> pframeTrue = std::make_shared<VideoFrame>(getSourceDir(__FILE__) + "images/sea_640.jpg");
  std::shared_ptr<VideoFrame> pframeFalse = std::make_shared<VideoFrame>(getSourceDir(__FILE__) + "images/sea_640.jpg");

  uint8_t threahold = 50;
  auto preference = WR::createRandom(pframe->width(), pframe->height(), threahold);


  pframeTrue->applyWR(preference, 0.1, true);
  pframeFalse->applyWR(preference, 0.1, false);
  
/*  pframeTrue->save(getSourceDir(__FILE__) + "out/true.jpg");
  pframeFalse->save(getSourceDir(__FILE__) + "out/false.jpg");
  preference->save(getSourceDir(__FILE__) + "out/reference.png");*/

/*  std::shared_ptr<VideoFrame> pframe = std::make_shared<VideoFrame>(getSourceDir(__FILE__) + "out/sea_640_transcodec.jpg");
  std::shared_ptr<VideoFrame> pframeTrue = std::make_shared<VideoFrame>(getSourceDir(__FILE__) + "out/true.jpg");
  std::shared_ptr<VideoFrame> pframeFalse = std::make_shared<VideoFrame>(getSourceDir(__FILE__) + "out/false.jpg");
  std::shared_ptr<VideoFrame> preference = std::make_shared<VideoFrame>(getSourceDir(__FILE__) + "out/reference.png");
  */

  Detector::Result resTrue = Detector::CorrelationCoefficientRGB(pframeTrue, preference, 0.01);
  Detector::Result resFalse = Detector::CorrelationCoefficientRGB(pframeFalse, preference, 0.01);
  Detector::Result resNoWatermark = Detector::CorrelationCoefficientRGB(pframe, preference, 0.01);

  BOOST_CHECK_EQUAL(resTrue, Detector::TRUE);
  BOOST_CHECK_EQUAL(resFalse, Detector::FALSE);
  BOOST_CHECK_EQUAL(resNoWatermark, Detector::NO_WATERMARK);
}

BOOST_AUTO_TEST_CASE(linear_correlation_grayscale)
{
  std::shared_ptr<VideoFrame> pframe = std::make_shared<VideoFrame>(getSourceDir(__FILE__) + "images/sea_640.jpg", VideoFrame::Grayscale);
  std::shared_ptr<VideoFrame> pframeTrue = std::make_shared<VideoFrame>(getSourceDir(__FILE__) + "images/sea_640.jpg", VideoFrame::Grayscale);
  std::shared_ptr<VideoFrame> pframeFalse = std::make_shared<VideoFrame>(getSourceDir(__FILE__) + "images/sea_640.jpg", VideoFrame::Grayscale);

  uint8_t threahold = 50;
  auto preference = WR::createRandom(pframe->width(), pframe->height(), threahold, VideoFrame::Grayscale);

  pframeTrue->applyWR(preference, 0.1, true);
  pframeFalse->applyWR(preference, 0.1, false);

  Detector::Result resTrue = Detector::CorrelationCoefficientGrayscale(pframeTrue, preference, 0.01);
  Detector::Result resFalse = Detector::CorrelationCoefficientGrayscale(pframeFalse, preference, 0.01);
  Detector::Result resNoWatermark = Detector::CorrelationCoefficientGrayscale(pframe, preference, 0.01);

  BOOST_CHECK_EQUAL(resTrue, Detector::TRUE);
  BOOST_CHECK_EQUAL(resFalse, Detector::FALSE);
  BOOST_CHECK_EQUAL(resNoWatermark, Detector::NO_WATERMARK);
}

BOOST_AUTO_TEST_CASE(pearson_correlation_grayscale)
{
  std::shared_ptr<VideoFrame> pframe = std::make_shared<VideoFrame>(getSourceDir(__FILE__) + "images/sea_640.jpg", VideoFrame::Grayscale);
  std::shared_ptr<VideoFrame> pframeTrue = std::make_shared<VideoFrame>(getSourceDir(__FILE__) + "images/sea_640.jpg", VideoFrame::Grayscale);

  uint8_t threahold = 50;
  auto preference = WR::createRandom(pframe->width(), pframe->height(), threahold, VideoFrame::Grayscale);

  pframeTrue->applyWR(preference, 0.5, true);

  const uint8_t* pdata = pframeTrue->data(0);
  std::vector<int> dataFrame(pdata, pdata + pframeTrue->stride(0) * pframeTrue->height());
  pdata = preference->data(0);
  std::vector<int> dataNoise(pdata, pdata + pframeTrue->stride(0) * pframeTrue->height());
  pdata = pframe->data(0);
  std::vector<int> dataNoWatermark(pdata, pdata + pframeTrue->stride(0) * pframeTrue->height());


  Detector::Result resTrue = Detector::PearsonCorrelation(dataFrame, dataNoise, 0.01);
  Detector::Result resNoWatermark = Detector::PearsonCorrelation(dataNoWatermark, dataNoise, 0.01);

  BOOST_CHECK_EQUAL(resTrue, Detector::TRUE);
  BOOST_CHECK_EQUAL(resNoWatermark, Detector::NO_WATERMARK);
}

BOOST_AUTO_TEST_SUITE_END();
