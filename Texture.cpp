#include "Texture.h"

#pragma pack(push, 1)
struct TGAHeader {
	std::uint8_t id_length;
	std::uint8_t color_map_type;
	std::uint8_t image_type;
	std::uint8_t colormap_spec[5];
	std::uint16_t x_origin;
	std::uint16_t y_origin;
	std::uint16_t width;
	std::uint16_t height;
	std::uint8_t bits_per_pixel;
	std::uint8_t image_descriptor;
};
#pragma pack(pop)



bool Texture::load_tga_file(const std::string& filename)
{
	std::ifstream in(filename, std::ios::binary);
	if (!in)
	{
		std::cerr << "error: cannot open texture file " << filename << std::endl;
		return false;
	}

	TGAHeader header;
	in.read(reinterpret_cast<char*>(&header), sizeof(header));
	if (!in)
	{
		std::cerr << "error: cannot read TGA header from " << filename << std::endl;
		return false;
	}

	// only support uncompressed 24/32 bpp TGA files
	if (header.image_type != 2 || (header.bits_per_pixel != 24 && header.bits_per_pixel != 32))
	{
		std::cerr << "error: unsupported TGA format in " << filename << std::endl;
		return false;
	}

	m_width = header.width;
	m_height = header.height;
	m_bytes_per_pixel = header.bits_per_pixel / 8;

	// skip img id field
	in.ignore(header.id_length);

	std::size_t buffer_size = m_width * m_height * m_bytes_per_pixel;
	m_buffer.resize(buffer_size);
	in.read(reinterpret_cast<char*>(m_buffer.data()), buffer_size);
	if (!in)
	{
		std::cerr << "error: cannot read pixel data from " << filename << std::endl;
		return false;
	}

	bool v_flip = !(header.image_descriptor & 0x20);

	if (v_flip)	std::cerr << "tga img is top-down, no v-flip" << std::endl;
	else
	{
		std::cerr << "tga img is bottom-up, performing v-flip" << std::endl;
		std::vector<std::uint8_t> flipped_buffer(buffer_size);
		for (int y = 0; y < m_height; ++y)
		{
			std::size_t src_line_start = (m_height - 1 - y) * m_width * m_bytes_per_pixel;
			std::size_t dest_line_start = y * m_width * m_bytes_per_pixel;
			std::memcpy(&flipped_buffer[dest_line_start], &m_buffer[src_line_start], m_width * m_bytes_per_pixel);
		}
		m_buffer = std::move(flipped_buffer);
	}
	in.close();
	std::cout << "texture loaded: " << filename
		<< " | size: " << m_width << "x" << m_height
		<< " | bpp: " << m_bytes_per_pixel * 8 << std::endl;
	return true;
}

Color Texture::sample(float u, float v) const
{
	if (m_buffer.empty()) return black; // texture not loaded
	// wrap uv coords
	u = std::max(.0f, std::min(1.0f, u));
	v = std::max(.0f, std::min(1.0f, v));

	// convert uv to pixel coords
	int x = static_cast<int>(u * (m_width - 1));
	//int x = static_cast<int>((1.0f - u) * (m_width - 1)); // flip U coordinate
	int y = static_cast<int>(v * (m_height - 1));
	//int y = static_cast<int>((1.0f - v) * (m_height - 1)); // flip V coordinate

	// calculate the index in the 1d buffer
	int index = (y * m_width + x) * m_bytes_per_pixel;

	// tga stores pixels in bgr(a) order
	std::uint8_t b = m_buffer[index];
	std::uint8_t g = m_buffer[index + 1];
	std::uint8_t r = m_buffer[index + 2];
	std::uint8_t a = (m_bytes_per_pixel == 4) ? m_buffer[index + 3] : 255;

	return Color(r, g, b, a);
}
