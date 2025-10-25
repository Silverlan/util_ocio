// SPDX-FileCopyrightText: (c) 2025 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

#include <cinttypes>
#include <memory>
#include <string>
#include <optional>

export module pragma.ocio;

export import pragma.image;

export namespace pragma::ocio {
	class ColorProcessor : public std::enable_shared_from_this<ColorProcessor> {
	  public:
		struct CreateInfo {
			enum class BitDepth : uint8_t { Float32 = 0, Float16, UInt8 };
			std::string config;
			std::string configLocation;
			std::optional<std::string> lookName {};
			BitDepth bitDepth = BitDepth::Float32;
		};
		static std::shared_ptr<ColorProcessor> Create(const CreateInfo &createInfo, std::string &outErr, float exposure = 0.f, float gamma = 2.2f);
		bool Apply(uimg::ImageBuffer &imgBuf, std::string &outErr);
	  private:
		std::shared_ptr<void> m_processorData = nullptr;
	};
	bool apply_color_transform(uimg::ImageBuffer &imgBuf, const ColorProcessor::CreateInfo &createInfo, std::string &outErr, float exposure = 0.f, float gamma = 2.2f);
};
