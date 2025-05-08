#include "VideoFrame.h"

#include <iostream>
#include <iomanip>
#include <boost/program_options.hpp>
#include <chrono>
#include <ctime>

#include "DCTWatermark.h"

//usage:
//dct_watermark.exe --gen_reference -i field-sd-gray.jpg -o reference --reference_max 16
//dct_watermark.exe --embed -i field-sd-gray.jpg -r reference -o field-sd -gray-watermarked.jpg --alpha 0.25 -v true
//dct_watermark.exe --detect -i field-sd-gray-watermarked.jpg -r reference

int main(int argc, char** argv)
{
	namespace po = boost::program_options;

	po::options_description desc("Allowed options");
	desc.add_options()
		("help", "produce help message")
		("gen_reference", "generate reference pattern (call with reference_max, skip_count, input and output files as arguments)")
		("embed", "embed watermark reference pattern (call with alpha and input, reference and output files as arguments)")
		("detect", "detect watermark reference pattern (call with threshold and input and reference files as arguments)")
		("get_correlation", "extract collelation beetwen input and reference (call input and reference files as arguments)")
		("get_average_dct", "get average DCT after embedding (call input and reference files as arguments)")
		("reference_max", po::value<int>()->default_value(50), "reference pattern maximum value (from 0 to 255)")
		("alpha", po::value<double>()->default_value(1.0), "reference pattern embedding multiplier")
		("threshold", po::value<double>()->default_value(0.01), "detector threshold value")
		("value,v", po::value<bool>()->default_value(true), "embedded value")
		("in,i", po::value<std::string>(), "input file")
		("out,o", po::value<std::string>(), "output file")
		("reference,r", po::value<std::string>(), "reference file")
		("skip_count,s", po::value<int>()->default_value(16), "number of first coefficients to skip")
		("barni,b", po::value<bool>()->default_value(false), "Use the Barni version of embedding algorithm")
		("correlation,c", po::value<int>()->default_value(2), "Type of correlation detector used on extracting (0-linear correlation, 1-normalized correlation(not supported) 2 - correlation coefficient(default))")
		("middle_frequncy,m", po::value<bool>()->default_value(false), "Use middle frequency watermarking")

		;

	po::variables_map vm;
	po::store(po::parse_command_line(argc, argv, desc), vm);

	DCTWatermark::EmbeddingType embeddingType = DCTWatermark::SKIP_FIRST;
	if (vm["middle_frequence"].as<bool>())
		embeddingType = DCTWatermark::MIDDLE_FREQUENCY;

	if (vm.count("gen_reference"))
	{
		std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
		srand(ms.count());
		if (!vm.count("in") || !vm.count("out"))
		{
			std::cout << "Arguments `in` and `out` are expected. Use help for additional info";
			return 1;
		}

		std::shared_ptr<VideoFrame> pframe = std::make_shared<VideoFrame>(vm["in"].as<std::string>());
		if (!pframe)
		{
			std::cout << "Error opening input file `" + vm["in"].as<std::string>() + "`";
			return 1;
		}

		bool BarniVersion = vm["barni"].as<bool>();

		std::vector<int> referenceData = DCTWatermark::createReference(pframe, vm["reference_max"].as<int>(), vm["skip_count"].as<int>(), BarniVersion, embeddingType);
		DCTWatermark::referenceStore(referenceData, vm["out"].as<std::string>());
	}
	else if (vm.count("embed"))
	{
		if (!vm.count("in") || !vm.count("out") || !vm.count("reference"))
		{
			std::cout << "Arguments `in`, `out` and `reference` are expected. Use help for additional info";
			return 1;
		}

		std::shared_ptr<VideoFrame> pframe = std::make_shared<VideoFrame>(vm["in"].as<std::string>());
		if (!pframe)
		{
			std::cout << "Error opening input file `" + vm["in"].as<std::string>() + "`";
			return 1;
		}

		std::vector<int> referenceData = DCTWatermark::referenceLoad(vm["reference"].as<std::string>());
		if (referenceData.empty())
		{
			std::cout << "Error opening reference file `" + vm["reference"].as<std::string>() + "`";
			return 1;
		}

		bool BarniVersion = vm["barni"].as<bool>();

		DCTWatermark watermark(vm["alpha"].as<double>(), -1, vm["skip_count"].as<int>(), referenceData, BarniVersion ? DCTWatermark::BARNI : DCTWatermark::DEFAULT, Detector::CORRELATION_COEFFICIENT, embeddingType);

		watermark.embed(pframe, vm["value"].as<bool>());
		pframe->save(vm["out"].as<std::string>());
	}
	else if (vm.count("detect"))
	{
		if (!vm.count("in") || !vm.count("reference"))
		{
			std::cout << "Arguments `in`, `out` and `reference` are expected. Use help for additional info";
			return 1;
		}

		std::shared_ptr<VideoFrame> pframe = std::make_shared<VideoFrame>(vm["in"].as<std::string>());
		if (!pframe)
		{
			std::cout << "Error opening input file `" + vm["in"].as<std::string>() + "`";
			return 1;
		}

		std::vector<int> referenceData = DCTWatermark::referenceLoad(vm["reference"].as<std::string>());
		if (referenceData.empty())
		{
			std::cout << "Error opening reference file `" + vm["reference"].as<std::string>() + "`";
			return 1;
		}

		bool BarniVersion = vm["barni"].as<bool>();
		Detector::Type detectorType = (Detector::Type)vm["correlation"].as<int>();

		DCTWatermark watermark(-1, vm["threshold"].as<double>(), vm["skip_count"].as<int>(), referenceData, BarniVersion ? DCTWatermark::BARNI : DCTWatermark::DEFAULT, detectorType, embeddingType);
		Watermark::DetectResult res = watermark.detect(pframe);

		if (res == Watermark::DetectResult::TRUE)
			std::cout << "TRUE" << std::endl;
		else if (res == Watermark::DetectResult::FALSE)
			std::cout << "FALSE" << std::endl;
		else if (res == Watermark::DetectResult::FAILED)
			std::cout << "DETECTION FAILED" << std::endl;
		else
			std::cout << "NOT DETECTED" << std::endl;
	}
	else if (vm.count("get_correlation"))
	{
		if (!vm.count("in") || !vm.count("reference"))
		{
			std::cout << "Arguments `in`, `out` and `reference` are expected. Use help for additional info";
			return 1;
		}

		std::shared_ptr<VideoFrame> pframe = std::make_shared<VideoFrame>(vm["in"].as<std::string>());
		if (!pframe)
		{
			std::cout << "Error opening input file `" + vm["in"].as<std::string>() + "`";
			return 1;
		}

		std::vector<int> referenceData = DCTWatermark::referenceLoad(vm["reference"].as<std::string>());
		if (referenceData.empty())
		{
			std::cout << "Error opening reference file `" + vm["reference"].as<std::string>() + "`";
			return 1;
		}

		bool BarniVersion = vm["barni"].as<bool>();
		Detector::Type detectorType = (Detector::Type)vm["correlation"].as<int>();

		DCTWatermark watermark(-1, vm["threshold"].as<double>(), vm["skip_count"].as<int>(), referenceData, BarniVersion ? DCTWatermark::BARNI : DCTWatermark::DEFAULT, detectorType, embeddingType);
		double correlation = -1;
		watermark.getCorrelation(pframe, correlation);
		std::cout << std::setprecision(10) << correlation << std::endl;
	}
	else if (vm.count("get_average_dct"))
	{
		if (!vm.count("in") || !vm.count("reference"))
		{
			std::cout << "Arguments `in`, `out` and `reference` are expected. Use help for additional info";
			return 1;
		}

		std::shared_ptr<VideoFrame> pframe = std::make_shared<VideoFrame>(vm["in"].as<std::string>());
		if (!pframe)
		{
			std::cout << "Error opening input file `" + vm["in"].as<std::string>() + "`";
			return 1;
		}

		std::vector<int> referenceData = DCTWatermark::referenceLoad(vm["reference"].as<std::string>());
		if (referenceData.empty())
		{
			std::cout << "Error opening reference file `" + vm["reference"].as<std::string>() + "`";
			return 1;
		}

		bool BarniVersion = vm["barni"].as<bool>();
		Detector::Type detectorType = (Detector::Type)vm["correlation"].as<int>();

		DCTWatermark watermark(-1, vm["threshold"].as<double>(), vm["skip_count"].as<int>(), referenceData, BarniVersion ? DCTWatermark::BARNI : DCTWatermark::DEFAULT, detectorType, embeddingType);
		double averageDCT = -1;
		watermark.getAverageDCT(pframe, averageDCT);
		std::cout << std::setprecision(10) << averageDCT << std::endl;
	}
	else
	{
		std::cout << desc << "\n";
	}

	return 0;
}
