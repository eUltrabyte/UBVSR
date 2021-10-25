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

void draw_loop(ubv::Window* window, ubv::Texture& texture1, glm::mat4& projection, glm::mat4& view) noexcept {
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

		// const float uNear = 0.1F;
		// const float uFar = 1000.0F;
		const float uFov = 90.0F;
		const float uAspect = static_cast<float>(frame_buffer.get_width()) / static_cast<float>(frame_buffer.get_height());
		// const float uFov_in_radians = 1.0F / std::tanf(uFov * 0.5F / 180.0F * 3.14159F);
		// projection.set_mat_at(ubv::u16vec2(0, 0), uAspect * uFov_in_radians);
		// projection.set_mat_at(ubv::u16vec2(1, 1), uFov_in_radians);
		// projection.set_mat_at(ubv::u16vec2(2, 2), uFar / (uFar - uNear));
		// projection.set_mat_at(ubv::u16vec2(3, 2), (-uFar * uNear) / (uFar - uNear));
		// projection.set_mat_at(ubv::u16vec2(2, 3), 1.0F);
		// projection.set_mat_at(ubv::u16vec2(3, 3), 0.0F);
		
		projection = glm::perspective(glm::radians(uFov), uAspect, 0.1F, 1000.0F);

		glm::vec3 cameraPos = { 0.0f, 0.0f, 3.0f };
		glm::vec3 cameraFront = { 0.0f, 0.0f, -1.0f };
		glm::vec3 cameraUp = { 0.0f, 1.0f, 0.0f };
		view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);


		static ubv::fvec3 offset{ 3.5F, 3.5F, 3.5F };

		if (GetAsyncKeyState(VK_LEFT))
		{
			offset.x -= 0.1F;
		}

		if (GetAsyncKeyState(VK_RIGHT))
		{
			offset.x += 0.1F;
		}

		if (GetAsyncKeyState(VK_UP))
		{
			offset.z -= 0.1F;
		}

		if (GetAsyncKeyState(VK_DOWN))
		{
			offset.z += 0.1F;
		}

		if (GetAsyncKeyState(0x41))
		{
			offset.y -= 0.1F;
		}

		if (GetAsyncKeyState(0x44))
		{
			offset.y += 0.1F;
		}

		glm::mat4 transform = glm::mat4(1);
		transform = glm::translate(transform, {0, 0, 0});

		ubv::Vertex c0a{ /* projection.multiply( */ ubv::fvec3{ 0, 0, 0 } + offset /* ) */, ubv::fvec2{ 0, 0 } };
		ubv::Vertex c0b{ /* projection.multiply( */ ubv::fvec3{ 1, 0, 0 } + offset /* ) */, ubv::fvec2{ 1, 0 } };
		ubv::Vertex c0c{ /* projection.multiply( */ ubv::fvec3{ 0, 1, 0 } + offset /* ) */, ubv::fvec2{ 0, 1 } };
		ubv::Vertex c0d{ /* projection.multiply( */ ubv::fvec3{ 1, 1, 0 } + offset /* ) */, ubv::fvec2{ 1, 1 } };
		ubv::Vertex c1a{ /* projection.multiply( */ ubv::fvec3{ 0, 0, 1 } + offset /* ) */, ubv::fvec2{ 0, 0 } };
		ubv::Vertex c1b{ /* projection.multiply( */ ubv::fvec3{ 1, 0, 1 } + offset /* ) */, ubv::fvec2{ 1, 0 } };
		ubv::Vertex c1c{ /* projection.multiply( */ ubv::fvec3{ 0, 1, 1 } + offset /* ) */, ubv::fvec2{ 0, 1 } };
		ubv::Vertex c1d{ /* projection.multiply( */ ubv::fvec3{ 1, 1, 1 } + offset /* ) */, ubv::fvec2{ 1, 1 } };
		ubv::Vertex c2a{ /* projection.multiply( */ ubv::fvec3{ 0, 0, 0 } + offset /* ) */, ubv::fvec2{ 0, 0 } };
		ubv::Vertex c2b{ /* projection.multiply( */ ubv::fvec3{ 0, 1, 0 } + offset /* ) */, ubv::fvec2{ 1, 0 } };
		ubv::Vertex c2c{ /* projection.multiply( */ ubv::fvec3{ 0, 0, 1 } + offset /* ) */, ubv::fvec2{ 0, 1 } };
		ubv::Vertex c2d{ /* projection.multiply( */ ubv::fvec3{ 0, 1, 1 } + offset /* ) */, ubv::fvec2{ 1, 1 } };
		ubv::Vertex c3a{ /* projection.multiply( */ ubv::fvec3{ 1, 0, 0 } + offset /* ) */, ubv::fvec2{ 0, 0 } };
		ubv::Vertex c3b{ /* projection.multiply( */ ubv::fvec3{ 1, 1, 0 } + offset /* ) */, ubv::fvec2{ 1, 0 } };
		ubv::Vertex c3c{ /* projection.multiply( */ ubv::fvec3{ 1, 0, 1 } + offset /* ) */, ubv::fvec2{ 0, 1 } };
		ubv::Vertex c3d{ /* projection.multiply( */ ubv::fvec3{ 1, 1, 1 } + offset /* ) */, ubv::fvec2{ 1, 1 } };
		ubv::Vertex c4a{ /* projection.multiply( */ ubv::fvec3{ 0, 0, 0 } + offset /* ) */, ubv::fvec2{ 0, 0 } };
		ubv::Vertex c4b{ /* projection.multiply( */ ubv::fvec3{ 0, 0, 1 } + offset /* ) */, ubv::fvec2{ 1, 0 } };
		ubv::Vertex c4c{ /* projection.multiply( */ ubv::fvec3{ 1, 0, 0 } + offset /* ) */, ubv::fvec2{ 0, 1 } };
		ubv::Vertex c4d{ /* projection.multiply( */ ubv::fvec3{ 1, 0, 1 } + offset /* ) */, ubv::fvec2{ 1, 1 } };
		ubv::Vertex c5a{ /* projection.multiply( */ ubv::fvec3{ 0, 1, 0 } + offset /* ) */, ubv::fvec2{ 0, 0 } };
		ubv::Vertex c5b{ /* projection.multiply( */ ubv::fvec3{ 0, 1, 1 } + offset /* ) */, ubv::fvec2{ 1, 0 } };
		ubv::Vertex c5c{ /* projection.multiply( */ ubv::fvec3{ 1, 1, 0 } + offset /* ) */, ubv::fvec2{ 0, 1 } };
		ubv::Vertex c5d{ /* projection.multiply( */ ubv::fvec3{ 1, 1, 1 } + offset /* ) */, ubv::fvec2{ 1, 1 } };

		std::array<ubv::Vertex*, 24> wierzcholki = {&c0a, &c0b, &c0c, &c0d,
													&c1a, &c1b, &c1c, &c1d,
													&c2a, &c2b, &c2c, &c2d,
													&c3a, &c3b, &c3c, &c3d,
													&c4a, &c4b, &c4c, &c4d,
													&c5a, &c5b, &c5c, &c5d
		};

		for (auto wierzcholek : wierzcholki) {
			auto zzz = projection * view * transform * glm::vec4(glm::vec3(wierzcholek->position.x, wierzcholek->position.y, wierzcholek->position.z), 1.0F );
			wierzcholek->position.x = zzz.x;
			wierzcholek->position.y = zzz.y;
			wierzcholek->position.z = zzz.z;
		}

		const std::array<ubv::Vertex, 3> t0a{ c0b, c0c, c0a };
		const std::array<ubv::Vertex, 3> t0b{ c0b, c0c, c0d };

		const std::array<ubv::Vertex, 3> t1a{ c1b, c1c, c1a };
		const std::array<ubv::Vertex, 3> t1b{ c1b, c1c, c1d };

		const std::array<ubv::Vertex, 3> t2a{ c2b, c2c, c2a };
		const std::array<ubv::Vertex, 3> t2b{ c2b, c2c, c2d };

		const std::array<ubv::Vertex, 3> t3a{ c3b, c3c, c3a };
		const std::array<ubv::Vertex, 3> t3b{ c3b, c3c, c3d };

		const std::array<ubv::Vertex, 3> t4a{ c4b, c4c, c4a };
		const std::array<ubv::Vertex, 3> t4b{ c4b, c4c, c4d };

		const std::array<ubv::Vertex, 3> t5a{ c5b, c5c, c5a };
		const std::array<ubv::Vertex, 3> t5b{ c5b, c5c, c5d };

		std::vector<std::future<void>> tasks;

		frame_buffer.draw_triangle(t0a, texture1);
		frame_buffer.draw_triangle(t0b, texture1);
		frame_buffer.draw_triangle(t1a, texture1);
		frame_buffer.draw_triangle(t1b, texture1);
		frame_buffer.draw_triangle(t2a, texture1);
		frame_buffer.draw_triangle(t2b, texture1);
		frame_buffer.draw_triangle(t3a, texture1);
		frame_buffer.draw_triangle(t3b, texture1);
		frame_buffer.draw_triangle(t4a, texture1);
		frame_buffer.draw_triangle(t4b, texture1);
		frame_buffer.draw_triangle(t5a, texture1);
		frame_buffer.draw_triangle(t5b, texture1);

		/*tasks.emplace_back(std::async(std::launch::async, &ubv::FrameBuffer::draw_triangle, &frame_buffer, t0a, texture1));
		tasks.emplace_back(std::async(std::launch::async, &ubv::FrameBuffer::draw_triangle, &frame_buffer, t0b, texture1));

		tasks.emplace_back(std::async(std::launch::async, &ubv::FrameBuffer::draw_triangle, &frame_buffer, t1a, texture1));
		tasks.emplace_back(std::async(std::launch::async, &ubv::FrameBuffer::draw_triangle, &frame_buffer, t1b, texture1));

		tasks.emplace_back(std::async(std::launch::async, &ubv::FrameBuffer::draw_triangle, &frame_buffer, t2a, texture1));
		tasks.emplace_back(std::async(std::launch::async, &ubv::FrameBuffer::draw_triangle, &frame_buffer, t2b, texture1));

		tasks.emplace_back(std::async(std::launch::async, &ubv::FrameBuffer::draw_triangle, &frame_buffer, t3a, texture1));
		tasks.emplace_back(std::async(std::launch::async, &ubv::FrameBuffer::draw_triangle, &frame_buffer, t3b, texture1));

		tasks.emplace_back(std::async(std::launch::async, &ubv::FrameBuffer::draw_triangle, &frame_buffer, t4a, texture1));
		tasks.emplace_back(std::async(std::launch::async, &ubv::FrameBuffer::draw_triangle, &frame_buffer, t4b, texture1));

		tasks.emplace_back(std::async(std::launch::async, &ubv::FrameBuffer::draw_triangle, &frame_buffer, t5a, texture1));
		tasks.emplace_back(std::async(std::launch::async, &ubv::FrameBuffer::draw_triangle, &frame_buffer, t5b, texture1));*/

		/*const ubv::Vertex vertex_a{projection.multiply(ubv::fvec3(rotate(ubv::fvec2{0.5, -0.5}, ubv::fvec2{0.0, 0.0}, 0), 1.0F)), ubv::fvec2{-1, 0}}; // 1 0
		const ubv::Vertex vertex_b{ projection.multiply(ubv::fvec3(rotate(ubv::fvec2{  0.5,  0.5 }, ubv::fvec2{ 0.0, 0.0 }, 0), 1.0F)), ubv::fvec2{ -1, -1 } };
		const ubv::Vertex vertex_c{ projection.multiply(ubv::fvec3(rotate(ubv::fvec2{ -0.5,  0.5 }, ubv::fvec2{ 0.0, 0.0 }, 0), 1.0F)), ubv::fvec2{ 0, -1 } };
		const ubv::Vertex vertex_d{ projection.multiply(ubv::fvec3(rotate(ubv::fvec2{ -0.5, -0.5 }, ubv::fvec2{ 0.0, 0.0 }, 0), 1.0F)), ubv::fvec2{ 0, 0 } };*/

		//const std::array<ubv::Vertex, 3> triangle1{ vertex_b, vertex_d, vertex_c };
		//const std::array<ubv::Vertex, 3> triangle2{	vertex_b, vertex_d, vertex_a };

		//tasks.emplace_back(std::async(std::launch::async, &ubv::FrameBuffer::draw_triangle, &frame_buffer, triangle1, texture1));
		//tasks.emplace_back(std::async(std::launch::async, &ubv::FrameBuffer::draw_triangle, &frame_buffer, triangle2, texture1));

		for(const auto& task : tasks) {
			task.wait();
		}

		if (GetAsyncKeyState(0x5A))
			frame_buffer.draw_z_buffer();
		window->display(frame_buffer);
		window->update();
	}
}

namespace ubv {
	Sandbox::Sandbox(int t_argc, char** t_argv) : texture1{ "box.tga", Texture::FilteringType::NEAREST } /*, texture2{ "test2.tga", Texture::FilteringType::NEAREST }*/ {
		for(auto i = 0; i < t_argc; ++i) {
			std::cout << "Program Input: " << t_argv[i] << "\n";
		}
	}
     
	Sandbox::~Sandbox() {
	}

    void Sandbox::start() {
		std::cout << "Hello UBVSR.\n";

		projection = glm::mat4(1);

		#if defined(_WIN32)
			ubv::WindowWin32 window(ubv::WindowProps{1280, 720, "Test UBVSR"});
		#elif defined(__unix__)
			ubv::WindowX11 window(ubv::WindowProps{1280, 720, "Test UBVSR"});
		#endif


		draw_loop(&window, texture1, projection, view);

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
