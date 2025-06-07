/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

module;

#include <chrono>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <utility>
#include <vector>
#include <optional>
#include <util_image_buffer.hpp>
#include <sharedutils/util.h>
#include <sharedutils/util_path.hpp>
#include <fsys/filesystem.h>

#include <OpenColorIO/OpenColorIO.h>
#include "ocio_helper.hpp"

#include <OpenImageIO/imageio.h>
#include <OpenImageIO/typedesc.h>

module pragma.ocio;

struct TransformProcessor {
	OCIO::ExposureContrastTransformRcPtr transform = nullptr;
	OCIO::ConstProcessorRcPtr processor = nullptr;
	OCIO::ConstCPUProcessorRcPtr cpuProcessor = nullptr;
};

struct ProcessorData {
	OCIO::ConstConfigRcPtr config = nullptr;

	OCIO::ConstProcessorRcPtr colorTransformProcessor = nullptr;
	OCIO::ConstCPUProcessorRcPtr colorTransformCpuProcessor = nullptr;

	std::shared_ptr<TransformProcessor> exposureTransform = nullptr;
	std::shared_ptr<TransformProcessor> gammaTransform = nullptr;
};

std::shared_ptr<pragma::ocio::ColorProcessor> pragma::ocio::ColorProcessor::Create(const CreateInfo &createInfo, std::string &outErr, float exposure, float gamma)
{
	auto bitDepth = (createInfo.bitDepth == CreateInfo::BitDepth::Float32) ? OCIO::BitDepth::BIT_DEPTH_F32 : (createInfo.bitDepth == CreateInfo::BitDepth::Float16) ? OCIO::BitDepth::BIT_DEPTH_F16 : OCIO::BitDepth::BIT_DEPTH_UINT8;
	auto context = OCIO::Context::Create();
	for(auto &path : filemanager::get_absolute_root_paths())
		context->addSearchPath(util::DirPath(path, "modules/open_color_io").GetString().c_str());

	try {
		auto processorData = std::make_shared<ProcessorData>();
		auto &config = processorData->config;
		auto &processor = processorData->colorTransformProcessor;

		config = OCIO::Config::CreateFromFile((createInfo.configLocation + createInfo.config + "/config.ocio").c_str());
		if(createInfo.lookName.has_value()) {
			auto look = config->getLook(createInfo.lookName->c_str());
			if(look)
				processor = config->getProcessor(look->getTransform());
		}
		if(processor == nullptr)
			processor = config->getProcessor("scene_linear", "Filmic sRGB");
		if(processor == nullptr) {
			outErr = "Unable to retrieve processor for config type " + createInfo.config;
			return nullptr;
		}
		processorData->colorTransformCpuProcessor = processor->getOptimizedCPUProcessor(bitDepth, bitDepth, OCIO::OPTIMIZATION_DEFAULT);

		auto createTransformProcessor = [&config, bitDepth](std::shared_ptr<TransformProcessor> &processor, const std::function<void(OCIO::ExposureContrastTransform &)> &init) {
			processor = std::make_shared<TransformProcessor>();
			auto t = OCIO::ExposureContrastTransform::Create();
			init(*t);
			t->setStyle(OCIO::ExposureContrastStyle::EXPOSURE_CONTRAST_LINEAR);
			t->setDirection(OCIO::TransformDirection::TRANSFORM_DIR_FORWARD);
			t->setPivot(1.0);

			processor->transform = t;
			processor->processor = config->getProcessor(t);
			processor->cpuProcessor = processor->processor->getOptimizedCPUProcessor(bitDepth, bitDepth, OCIO::OPTIMIZATION_DEFAULT);
		};
		if(exposure != 0.f) {
			createTransformProcessor(processorData->exposureTransform, [exposure](OCIO::ExposureContrastTransform &t) { t.setExposure(exposure); });
		}
		if(gamma != 1.f && gamma != 0.f) {
			createTransformProcessor(processorData->gammaTransform, [gamma](OCIO::ExposureContrastTransform &t) { t.setGamma(1.0 / static_cast<double>(gamma)); });
		}

		auto oProcessor = std::shared_ptr<ColorProcessor> {new ColorProcessor {}};
		oProcessor->m_processorData = processorData;
		return oProcessor;
	}
	catch(OCIO::Exception &exception) {
		outErr = exception.what();
		return nullptr;
	}
	catch(...) {
		outErr = "Unknown error";
		return nullptr;
	}
	// Unreachable
	return nullptr;
}
bool pragma::ocio::ColorProcessor::Apply(uimg::ImageBuffer &imgBuf, std::string &outErr)
{
	auto bitDepth = imgBuf.IsFloatFormat() ? OCIO::BitDepth::BIT_DEPTH_F32 : imgBuf.IsHDRFormat() ? OCIO::BitDepth::BIT_DEPTH_F16 : OCIO::BitDepth::BIT_DEPTH_UINT8;
	OCIO::PackedImageDesc img {imgBuf.GetData(), static_cast<long>(imgBuf.GetWidth()), static_cast<long>(imgBuf.GetHeight()), imgBuf.GetChannelCount(), bitDepth, imgBuf.GetChannelSize(), static_cast<ptrdiff_t>(imgBuf.GetPixelStride()),
	  static_cast<ptrdiff_t>(imgBuf.GetPixelStride() * imgBuf.GetWidth())};

	try {
		auto &processorData = *static_cast<ProcessorData *>(m_processorData.get());
		if(processorData.exposureTransform)
			processorData.exposureTransform->cpuProcessor->apply(img);

		processorData.colorTransformCpuProcessor->apply(img);

		if(processorData.gammaTransform)
			processorData.gammaTransform->cpuProcessor->apply(img);
	}
	catch(OCIO::Exception &exception) {
		outErr = exception.what();
		return false;
	}
	catch(...) {
		outErr = "Unknown error";
		return false;
	}
	return true;
}

bool pragma::ocio::apply_color_transform(uimg::ImageBuffer &imgBuf, const ColorProcessor::CreateInfo &createInfo, std::string &outErr, float exposure, float gamma)
{
	auto processor = ColorProcessor::Create(createInfo, outErr, exposure, gamma);
	if(processor == nullptr)
		return false;
	return processor->Apply(imgBuf, outErr);
}
