#pragma once

#include "Pixel.hpp"

#include <cstdint>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

namespace ubv {
    class TGA {
		friend class FrameBuffer;

	public:
        struct Header {
	    	std::uint8_t idlength;
	    	std::uint8_t colourmaptype;
	    	std::uint8_t datatypecode;
	    	std::uint16_t colourmaporigin;
	    	std::uint16_t colourmaplength;
	    	std::uint8_t colourmapdepth;
	    	std::uint16_t x_origin;
	    	std::uint16_t y_origin;
	    	std::uint16_t width;
	    	std::uint16_t height;
	    	std::uint8_t bitsperpixel;
	    	std::uint8_t imagedescriptor;
	    } header;

		inline TGA(std::uint16_t t_width, std::uint16_t t_height)
		{
			header.idlength = 0;
			header.colourmaptype = 0;
			header.datatypecode = 2;
			header.x_origin = 0;
			header.y_origin = 0;
			header.width = t_width;
			header.height = t_height;
			header.bitsperpixel = 24;
			const auto raw_data_size = (static_cast<std::size_t>(get_width()) * get_height() * (3U));
	    	m_pixel_data.resize(raw_data_size);
		}


	    inline explicit TGA(std::string_view t_filename) {
            std::ifstream file(t_filename.data(), std::ios::in | std::ios::binary);

		    if(!file.is_open()) {
				throw std::runtime_error("Cannot open TGA");
		    }

	    	file.read(reinterpret_cast<char*>(&header.idlength), sizeof(header.idlength));
	    	file.read(reinterpret_cast<char*>(&header.colourmaptype), sizeof(header.colourmaptype));
	    	file.read(reinterpret_cast<char*>(&header.datatypecode), sizeof(header.datatypecode));
	    	file.read(reinterpret_cast<char*>(&header.colourmaporigin), sizeof(header.colourmaporigin));
	    	file.read(reinterpret_cast<char*>(&header.colourmaplength), sizeof(header.colourmaplength));
	    	file.read(reinterpret_cast<char*>(&header.colourmapdepth), sizeof(header.colourmapdepth));
	    	file.read(reinterpret_cast<char*>(&header.x_origin), sizeof(header.x_origin));
	    	file.read(reinterpret_cast<char*>(&header.y_origin), sizeof(header.y_origin));
	    	file.read(reinterpret_cast<char*>(&header.width), sizeof(header.width));
	    	file.read(reinterpret_cast<char*>(&header.height), sizeof(header.height));
	    	file.read(reinterpret_cast<char*>(&header.bitsperpixel), sizeof(header.bitsperpixel));
	    	file.read(reinterpret_cast<char*>(&header.imagedescriptor), sizeof(header.imagedescriptor));

	    	const auto raw_data_size = (static_cast<std::size_t>(get_width()) * get_height() * 3U);
	    	m_pixel_data.resize(raw_data_size);
            file.read(reinterpret_cast<char*>(m_pixel_data.data()), raw_data_size);
	    }

		inline Pixel& get_pixel(unsigned x, unsigned y) {
			return (*reinterpret_cast<Pixel*>(m_pixel_data.data() + (y * get_width() + x) * (3U)));
        }

		inline const Pixel& get_pixel(unsigned x, unsigned y) const {
			return (*reinterpret_cast<const Pixel*>(m_pixel_data.data() + (y * get_width() + x) * (3U)));
        }

		constexpr std::uint16_t get_width() const noexcept {
			return header.width;
		}

		constexpr std::uint16_t get_height() const noexcept {
			return header.height;
		}

		inline void to_file(std::string_view t_filename) const
		{
			std::ofstream file(t_filename.data(), std::ios::out | std::ios::binary);

			if (!file.is_open()) {
				std::abort();
			}

			file.write(reinterpret_cast<const char*>(&header.idlength), sizeof(header.idlength));
			file.write(reinterpret_cast<const char*>(&header.colourmaptype), sizeof(header.colourmaptype));
			file.write(reinterpret_cast<const char*>(&header.datatypecode), sizeof(header.datatypecode));
			file.write(reinterpret_cast<const char*>(&header.colourmaporigin), sizeof(header.colourmaporigin));
			file.write(reinterpret_cast<const char*>(&header.colourmaplength), sizeof(header.colourmaplength));
			file.write(reinterpret_cast<const char*>(&header.colourmapdepth), sizeof(header.colourmapdepth));
			file.write(reinterpret_cast<const char*>(&header.x_origin), sizeof(header.x_origin));
			file.write(reinterpret_cast<const char*>(&header.y_origin), sizeof(header.y_origin));
			file.write(reinterpret_cast<const char*>(&header.width), sizeof(header.width));
			file.write(reinterpret_cast<const char*>(&header.height), sizeof(header.height));
			file.write(reinterpret_cast<const char*>(&header.bitsperpixel), sizeof(header.bitsperpixel));
			file.write(reinterpret_cast<const char*>(&header.imagedescriptor), sizeof(header.imagedescriptor));

			file.write(reinterpret_cast<const char*>(m_pixel_data.data()), m_pixel_data.size());
		}

	private:
        //std::uint16_t m_width;
	    //std::uint16_t m_height;
	    std::vector<std::byte> m_pixel_data;
		std::uint8_t m_bytes_per_pixel;
    };
};