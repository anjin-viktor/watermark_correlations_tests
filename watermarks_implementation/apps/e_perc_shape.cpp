#include "VideoFrame.h"

#include <iostream>
#include <iomanip>
#include <boost/program_options.hpp>
#include <chrono>
#include <ctime>

#include "EPercShape.h"

//usage:
//e_perc_shape.exe --gen_reference -i field-sd.jpg -o reference.png --strength 0.5
//e_perc_shape.exe --embed -i field-sd.jpg -r reference.png -o field-sd-watermarked.jpg
//e_perc_shape.exe --detect -i field-sd-watermarked.jpg -r reference.png --origin field-sd.jpg

int main(int argc, char** argv)
{
	srand(time(NULL));

	namespace po = boost::program_options;

	po::options_description desc("Allowed options");
	desc.add_options()
		("help", "produce help message")
		("gen_reference", "generate reference pattern (call with reference_max, skip_count, input and output files as arguments)")
		("embed", "embed watermark reference pattern (call with alpha and input, reference and output files as arguments)")
		("detect", "detect watermark reference pattern (call with threshold and input and reference files as arguments)")
		("strength", po::value<double>()->default_value(0.25), "strength of watermark embedding (from 0 to 1 (and more than 1 for higher strength))")
		("threshold", po::value<double>()->default_value(0.01), "detector threshold value")
		("in,i", po::value<std::string>(), "input file")
		("origin", po::value<std::string>(), "origin input file before watermarking (used in watermark detection)")
		("out,o", po::value<std::string>(), "output file")
		("reference,r", po::value<std::string>(), "reference file")
		("get_correlation", "extract collelation beetwen input and reference (call input and reference files as arguments)")
		;

	po::variables_map vm;
	po::store(po::parse_command_line(argc, argv, desc), vm);

	if (vm.count("gen_reference"))
	{
		if (!vm.count("in") || !vm.count("out"))
		{
			std::cout << "Arguments `in` and `out` are expected. Use help for additional info";
			return 1;
		}
		std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
		srand(ms.count());

		std::shared_ptr<VideoFrame> pframe = std::make_shared<VideoFrame>(vm["in"].as<std::string>());
		if (!pframe)
		{
			std::cout << "Error opening input file `" + vm["in"].as<std::string>() + "`";
			return 1;
		}

		std::shared_ptr<VideoFrame> preference = EPercShape::createReference(pframe, vm["strength"].as<double>());
		preference->save(vm["out"].as<std::string>());
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

		std::shared_ptr<VideoFrame> preference = std::make_shared<VideoFrame>(vm["reference"].as<std::string>());
		if (!preference)
		{
			std::cout << "Error opening reference file `" + vm["reference"].as<std::string>() + "`";
			return 1;
		}

		EPercShape watermark(-1, std::weak_ptr<VideoFrame>(), preference);

		watermark.embed(pframe, true);

		pframe->save(vm["out"].as<std::string>());
	}
	else if (vm.count("detect"))
	{
		if (!vm.count("in") || !vm.count("reference") || !vm.count("origin"))
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

		std::shared_ptr<VideoFrame> pframeOrigin = std::make_shared<VideoFrame>(vm["origin"].as<std::string>());
		if (!pframe)
		{
			std::cout << "Error opening origin input file `" + vm["origin"].as<std::string>() + "`";
			return 1;
		}


		std::shared_ptr<VideoFrame> preference = std::make_shared<VideoFrame>(vm["reference"].as<std::string>());
		if (!preference)
		{
			std::cout << "Error opening reference file `" + vm["reference"].as<std::string>() + "`";
			return 1;
		}

		EPercShape watermark(vm["threshold"].as<double>(), pframeOrigin, preference);
		Watermark::DetectResult res = watermark.detect(pframe);

		if (res == Watermark::DetectResult::TRUE)
			std::cout << "TRUE" << std::endl;
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

		std::shared_ptr<VideoFrame> pframeOrigin = std::make_shared<VideoFrame>(vm["origin"].as<std::string>());
		if (!pframe)
		{
			std::cout << "Error opening origin input file `" + vm["origin"].as<std::string>() + "`";
			return 1;
		}


		std::shared_ptr<VideoFrame> preference = std::make_shared<VideoFrame>(vm["reference"].as<std::string>());
		if (!preference)
		{
			std::cout << "Error opening reference file `" + vm["reference"].as<std::string>() + "`";
			return 1;
		}

		EPercShape watermark(vm["threshold"].as<double>(), pframeOrigin, preference);
		double correlation = -1;
		watermark.getCorrelation(pframe, correlation);
		std::cout << std::setprecision(10) << correlation << std::endl;
	}
	else
	{
		std::cout << desc << "\n";
	}

	return 0;
}
