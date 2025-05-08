#include <boost/test/unit_test.hpp>

#include <memory>

#include "Utils.h"
#include "VideoFrame.h"
#include "VisualMasking.h"
#include "EBlindDLC.h"
#include "Detector.h"

BOOST_AUTO_TEST_SUITE(masking);

BOOST_AUTO_TEST_CASE(masking_grayscale)
{
  std::shared_ptr<VideoFrame> pframe = std::make_shared<VideoFrame>(getSourceDir(__FILE__) + "images/sea_640.jpg", VideoFrame::Grayscale);
  BOOST_CHECK_EQUAL(pframe->width(), 480);
  BOOST_CHECK_EQUAL(pframe->height(), 640);

  VisualMasking masking(pframe);

  auto preference = EBlindDLC::createReference(pframe, 255);
//  auto preferenceMasked = masking.mask(preference);
//  preferenceMasked->save(getSourceDir(__FILE__) + "out/sea_640_grayscale_masked.jpg");

//  BOOST_CHECK_EQUAL(Detector::CorrelationCoefficientGrayscale(pframe, preferenceMasked, 0.2), Detector::Result::TRUE);
}

BOOST_AUTO_TEST_CASE(masking_color)
{
  std::shared_ptr<VideoFrame> pframe = std::make_shared<VideoFrame>(getSourceDir(__FILE__) + "images/sea_640.jpg", VideoFrame::Color);
  BOOST_CHECK_EQUAL(pframe->width(), 480);
  BOOST_CHECK_EQUAL(pframe->height(), 640);

  VisualMasking masking(pframe);

  auto preference = EBlindDLC::createReference(pframe, 255);
//  auto preferenceMasked = masking.mask(preference);
//  preferenceMasked->save(getSourceDir(__FILE__) + "out/sea_640_masked.jpg");

//  BOOST_CHECK_EQUAL(Detector::CorrelationCoefficientGrayscale(pframe, preferenceMasked, 0.2), Detector::Result::TRUE);
}

BOOST_AUTO_TEST_SUITE_END();
