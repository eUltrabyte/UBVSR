#pragma once

#include "Pixel.hpp"

#include <cstdint>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

namespace ubv {
    class TGA {
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

	    inline explicit TGA(std::string_view t_filename) {
            std::ifstream file((std::string)t_filename, std::ios::in | std::ios::binary);

		    if(file.is_open()) {
		    	std::cout << "wo korva\n";
		    } else {
		    	std::cout << "klasycznie\n";
	    		throw 69;
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

	    	const auto raw_data_size = (static_cast<std::size_t>(get_width()) * get_height() * (header.bitsperpixel / 8U));
	    	m_pixel_data.resize(raw_data_size);
            file.read(reinterpret_cast<char*>(m_pixel_data.data()), raw_data_size);
	    }

		inline Pixel& get_pixel(u16vec2 t_position) { //TODO: make it work on linux
			return (*reinterpret_cast<Pixel*>(m_pixel_data.data() + (static_cast<std::size_t>(t_position.y) * get_width() + t_position.x) * (header.bitsperpixel/8U)));
        }

		constexpr std::uint16_t get_width() noexcept {
			return header.width;
		}

		constexpr std::uint16_t get_height() noexcept {
			return header.height;
		}

	private:
        //std::uint16_t m_width;
	    //std::uint16_t m_height;
	    std::vector<std::byte> m_pixel_data;
    };
};