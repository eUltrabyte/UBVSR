#include "UBVSR.hpp"

#include "FpsCounter.hpp"

ubv::fvec2 rotate(ubv::fvec2 t_point, ubv::fvec2 t_origin, float t_angle) {
	float s = sin(t_angle);
	float c = cos(t_angle);

	//translate point back to origin
	t_point.x -= t_origin.x;
	t_point.y -= t_origin.y;

	//rotate point
	float xnew = t_point.x * c - t_point.y * s;
	float ynew = t_point.x * s + t_point.y * c;

	//translate point back
	t_point.x = xnew + t_origin.x;
	t_point.y = ynew + t_origin.y;
	return t_point;
}

void draw_loop(ubv::Window* window, ubv::Texture& texture) noexcept {
	const Timepoint t1;
	ubv::FrameBuffer frame_buffer(window->get_win_width(), window->get_win_height());
	while(true) {
		frame_buffer.clear(ubv::Pixel(0, 0, 0));
		static FpsCounter fps_counter;

		const Timepoint t2;
		float elapsed_time = (t2 - t1) * 2.0L;
		std::cout << "FPS: " << fps_counter.update(t2) << '\n';

		const float angle = sin(elapsed_time) * 0.25F;

		/*const ubv::Vertex vertex_a{ubv::fvec2{0, 0}, ubv::Pixel{255, 0, 0}};
		const ubv::Vertex vertex_b{ ubv::fvec2{ 1280, 0 }, ubv::Pixel{ 0, 255, 0 } };
		const ubv::Vertex vertex_c{ ubv::fvec2{ 1280, 720 }, ubv::Pixel{ 0, 0, 255 } };
		const ubv::Vertex vertex_d{ ubv::fvec2{ 0, 720 }, ubv::Pixel{ 255, 255, 0 } };
		const ubv::Vertex vertex_e{ rotate(ubv::fvec2{ 110, 207 }, ubv::fvec2{ 640, 360 }, angle), ubv::Pixel{ 255, 0, 255 } };
		const ubv::Vertex vertex_f{ rotate(ubv::fvec2{ 870, 342 }, ubv::fvec2{ 640, 360 }, angle), ubv::Pixel{ 0, 255, 255 } };
		const ubv::Vertex vertex_g{ rotate(ubv::fvec2{ 909, 585 }, ubv::fvec2{ 640, 360 }, angle), ubv::Pixel{ 63, 63, 63 } };
		const ubv::Vertex vertex_h{ rotate(ubv::fvec2{ 487, 513 }, ubv::fvec2{ 640, 360 }, angle), ubv::Pixel{ 255, 255, 255 } };

		const std::array<ubv::Vertex, 3> triangle1{	vertex_a,vertex_b,vertex_e };
		const std::array<ubv::Vertex, 3> triangle2{	vertex_f,vertex_b,vertex_e };
		const std::array<ubv::Vertex, 3> triangle3{	vertex_f,vertex_b,vertex_c };
		const std::array<ubv::Vertex, 3> triangle4{	vertex_a,vertex_e,vertex_d };
		const std::array<ubv::Vertex, 3> triangle5{	vertex_d,vertex_e,vertex_h };
		const std::array<ubv::Vertex, 3> triangle6{ vertex_f,vertex_h,vertex_e };
		const std::array<ubv::Vertex, 3> triangle7{ vertex_f,vertex_h,vertex_g };
		const std::array<ubv::Vertex, 3> triangle8{ vertex_d,vertex_h,vertex_g };
		const std::array<ubv::Vertex, 3> triangle9{ vertex_f,vertex_c,vertex_g };
		const std::array<ubv::Vertex, 3> triangle10{ vertex_c,vertex_d,vertex_g };

		ubv::vec2<float> point_a = rotate(ubv::fvec2{ 640.0F - 400.0F, 360.0F + 346.0F }, ubv::fvec2{ 640.0F, 360.0F }, angle);
		ubv::vec2<float> point_b = rotate(ubv::fvec2{ 640.0F - 400.0F, 360.0F - 346.0F }, ubv::fvec2{ 640.0F, 360.0F }, angle);
		ubv::vec2<float> point_c = rotate(ubv::fvec2{ 640.0F + 400.0F, 360.0F + 346.0F }, ubv::fvec2{ 640.0F, 360.0F }, angle);

		std::vector<std::future<void>> tasks;
		tasks.emplace_back(std::async(std::launch::async, &ubv::FrameBuffer::draw_triangle, &frame_buffer, triangle1));
		tasks.emplace_back(std::async(std::launch::async, &ubv::FrameBuffer::draw_triangle, &frame_buffer, triangle2));
		tasks.emplace_back(std::async(std::launch::async, &ubv::FrameBuffer::draw_triangle, &frame_buffer, triangle3));
		tasks.emplace_back(std::async(std::launch::async, &ubv::FrameBuffer::draw_triangle, &frame_buffer, triangle4));
		tasks.emplace_back(std::async(std::launch::async, &ubv::FrameBuffer::draw_triangle, &frame_buffer, triangle5));
		tasks.emplace_back(std::async(std::launch::async, &ubv::FrameBuffer::draw_triangle, &frame_buffer, triangle6));
		tasks.emplace_back(std::async(std::launch::async, &ubv::FrameBuffer::draw_triangle, &frame_buffer, triangle7));
		tasks.emplace_back(std::async(std::launch::async, &ubv::FrameBuffer::draw_triangle, &frame_buffer, triangle8));
		tasks.emplace_back(std::async(std::launch::async, &ubv::FrameBuffer::draw_triangle, &frame_buffer, triangle9));
		tasks.emplace_back(std::async(std::launch::async, &ubv::FrameBuffer::draw_triangle, &frame_buffer, triangle10));

		for(const auto& task : tasks) {
			task.wait();
		}

		for (unsigned x = 0; x < tga_file.get_width(); ++x) {
			for (unsigned y = 0; y < tga_file.get_height(); ++y) {
				frame_buffer.at(x, y) = tga_file.get_pixel(ubv::u16vec2{uint16_t(x),uint16_t(y)});
			}
		}*/

		const ubv::Vertex vertex_a{ rotate(ubv::fvec2{ 640 + 100, 360 - 100 }, ubv::fvec2{ 640, 360 }, elapsed_time), ubv::fvec2{ 1, 0 } }; // 1 0
		const ubv::Vertex vertex_b{ rotate(ubv::fvec2{ 640 + 100, 360 + 100 }, ubv::fvec2{ 640, 360 }, elapsed_time), ubv::fvec2{ 1, 1 } };
		const ubv::Vertex vertex_c{ rotate(ubv::fvec2{ 640 - 100, 360 + 100 }, ubv::fvec2{ 640, 360 }, elapsed_time), ubv::fvec2{ 0, 1 } };
		const ubv::Vertex vertex_d{ rotate(ubv::fvec2{ 640 - 100, 360 - 100 }, ubv::fvec2{ 640, 360 }, elapsed_time), ubv::fvec2{ 0, 0 } };

		const std::array<ubv::Vertex, 3> triangle1{	vertex_b, vertex_d, vertex_a };
		const std::array<ubv::Vertex, 3> triangle2{	vertex_b, vertex_d, vertex_c };

		std::vector<std::future<void>> tasks;

		tasks.emplace_back(std::async(std::launch::async, &ubv::FrameBuffer::draw_triangle, &frame_buffer, triangle1, texture));
		tasks.emplace_back(std::async(std::launch::async, &ubv::FrameBuffer::draw_triangle, &frame_buffer, triangle2, texture));

		//frame_buffer.draw_triangle(triangle1, texture);
		//frame_buffer.draw_triangle(triangle2, texture);

		for(const auto& task : tasks) {
			task.wait();
		}

		window->display(frame_buffer);
		window->update();
	}
}

namespace ubv {
	Sandbox::Sandbox(int t_argc, char** t_argv) : texture{ "test2.tga", Texture::FilteringType::NEAREST } {
		for(auto i = 0; i < t_argc; ++i) {
			std::cout << "Program Input: " << t_argv[i] << "\n";
		}
	}
     
	Sandbox::~Sandbox() {
		//delete this;
	}

    void Sandbox::start() {
		std::cout << "Hello UBVSR.\n";

		#if defined(_WIN32)
			ubv::WindowWin32 window(ubv::WindowProps{1280, 720, "Test UBVSR"});
		#elif defined(__unix__)
			ubv::WindowX11 window(ubv::WindowProps{1280, 720, "Test UBVSR"});
		#endif

		draw_loop(&window, texture);

		std::cout << "Goodbye UBVSR\n";
		std::cin.get();
	}
};

int main(int argc, char** argv)
try
{
	ubv::Sandbox app(argc, argv);
	app.start();
	return 0;
}
catch (const std::exception &exception)
{
	std::cout << "Fatal error!\n" << exception.what() << std::flush;
}
