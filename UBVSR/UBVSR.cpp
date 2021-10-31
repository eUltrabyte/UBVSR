#include "UBVSR.hpp"

#include "FpsCounter.hpp"
#include "Buffer.hpp"

void zzz()
{
	ubv::StencilBuffer color_buffer(640, 480);
	color_buffer.at(40, 50) = true;
	const auto& bb = color_buffer;
	auto& z = bb.at(40, 50);
	color_buffer.clear();
	//ubv::DepthBuffer depth_buffer(ubv::u16vec2{640,480});
	//ubv::StencilBuffer stencil_buffer(ubv::u16vec2{640,480});
	//stencil_buffer.at(ubv::u16vec2{ 3,4 }) = true;
	//std::vector<float> yaa{std::size_t(5000) * uint16_t(3), std::numeric_limits<float>::infinity() };
}

ubv::fvec2 rotate(ubv::fvec2 t_point, ubv::fvec2 t_origin, float t_angle) {
	float s = std::sin(t_angle);
	float c = std::cos(t_angle);

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

void draw_loop(ubv::Window* window, ubv::Texture& texture1, ubv::fmat4x4& projection, ubv::Model& model) noexcept {
	const Timepoint t1;
	ubv::FrameBuffer frame_buffer(window->get_win_width(), window->get_win_height());
	while(true) {
		frame_buffer.clear();
		static FpsCounter fps_counter;
		static Timepoint t3 = t1;
		const Timepoint t2;
		float elapsed_time = (t2 - t1) * 2.0L;
		float delta_time = (t2 - t3) * 20.0F;
		std::cout << "FPS: " << fps_counter.update(t2) << '\n';
		t3 = t2;
		static ubv::fvec3 camera_position{ 0.0F,-1.5F,-2.0F };
		static ubv::fvec3 camera_pitch_yaw_roll{ 0.0F,ubv::degrees_to_radians(-90.0F),0.0F };

		//static const auto camera_front = ubv::fvec3(0.0f, 0.0f, -1.0f);
		static const auto camera_up = ubv::fvec3(0.0f, 1.0f, 0.0f);

		if (window->IsKeyPressed(VK_LEFT))
		{
			if (window->IsKeyPressed(VK_RETURN))
			{
				camera_pitch_yaw_roll.x -= delta_time / 20.0F;
			}
			else
			{
				camera_position.x -= delta_time;
			}
		}

		if (window->IsKeyPressed(VK_RIGHT))
		{
			if (window->IsKeyPressed(VK_RETURN))
			{
				camera_pitch_yaw_roll.x += delta_time / 20.0F;
			}
			else
			{
				camera_position.x += delta_time;
			}
		}

		if (window->IsKeyPressed(VK_UP))
		{
			if (window->IsKeyPressed(VK_RETURN))
			{
				camera_pitch_yaw_roll.z += delta_time / 20.0F;
			}
			else
			{
				camera_position.z += delta_time;
			}
		}

		if (window->IsKeyPressed(VK_DOWN))
		{
			if (window->IsKeyPressed(VK_RETURN))
			{
				camera_pitch_yaw_roll.z -= delta_time / 20.0F;
			}
			else
			{
				camera_position.z -= delta_time;
			}
		}

		if (window->IsKeyPressed(VK_SPACE))
		{
			if (window->IsKeyPressed(VK_RETURN))
			{
				camera_pitch_yaw_roll.y += delta_time / 20.0F;
			}
			else
			{
				camera_position.y += delta_time;
			}
		}

		if (window->IsKeyPressed(VK_SHIFT))
		{
			if (window->IsKeyPressed(VK_RETURN))
			{
				camera_pitch_yaw_roll.y -= delta_time / 20.0F;
			}
			else
			{
				camera_position.y -= delta_time;
			}
		}

		ubv::fvec3 front;
		front.x = std::cos((camera_pitch_yaw_roll.y)) * std::cos((camera_pitch_yaw_roll.x));
		front.y = std::sin((camera_pitch_yaw_roll.x));
		front.z = std::sin((camera_pitch_yaw_roll.y)) * std::cos((camera_pitch_yaw_roll.x));
		const auto camera_front = ubv::normalize(front);
		auto view = ubv::look_at(camera_position, camera_position + camera_front, camera_up);
		auto MVP = view * projection;

		ubv::fvec3 light_direction = ubv::fvec3{ 0.0F, 0.0F, -1.0F };
		float l = std::sqrt(light_direction.x * light_direction.x + light_direction.y * light_direction.y + light_direction.z * light_direction.z);
		light_direction.x /= l;
		light_direction.y /= l;
		light_direction.z /= l;

		/*ubv::Vertex c0a{MVP * ubv::fvec4{ubv::fvec3(0, 0, 0), 1.0F}, ubv::fvec2{0, 0}};
		ubv::Vertex c0b{ MVP * ubv::fvec4{ ubv::fvec3(1, 0, 0), 1.0F }, ubv::fvec2{ 1, 0 } };
		ubv::Vertex c0c{ MVP * ubv::fvec4{ ubv::fvec3(0, 1, 0), 1.0F }, ubv::fvec2{ 0, 1 } };
		ubv::Vertex c0d{ MVP * ubv::fvec4{ ubv::fvec3(1, 1, 0), 1.0F }, ubv::fvec2{ 1, 1 } };
		ubv::Vertex c1a{ MVP * ubv::fvec4{ ubv::fvec3(0, 0, 1), 1.0F }, ubv::fvec2{ 0, 0 } };
		ubv::Vertex c1b{ MVP * ubv::fvec4{ ubv::fvec3(1, 0, 1), 1.0F }, ubv::fvec2{ 1, 0 } };
		ubv::Vertex c1c{ MVP * ubv::fvec4{ ubv::fvec3(0, 1, 1), 1.0F }, ubv::fvec2{ 0, 1 } };
		ubv::Vertex c1d{ MVP * ubv::fvec4{ ubv::fvec3(1, 1, 1), 1.0F }, ubv::fvec2{ 1, 1 } };
		ubv::Vertex c2a{ MVP * ubv::fvec4{ ubv::fvec3(0, 0, 0), 1.0F }, ubv::fvec2{ 0, 0 } };
		ubv::Vertex c2b{ MVP * ubv::fvec4{ ubv::fvec3(0, 1, 0), 1.0F }, ubv::fvec2{ 1, 0 } };
		ubv::Vertex c2c{ MVP * ubv::fvec4{ ubv::fvec3(0, 0, 1), 1.0F }, ubv::fvec2{ 0, 1 } };
		ubv::Vertex c2d{ MVP * ubv::fvec4{ ubv::fvec3(0, 1, 1), 1.0F }, ubv::fvec2{ 1, 1 } };
		ubv::Vertex c3a{ MVP * ubv::fvec4{ ubv::fvec3(1, 0, 0), 1.0F }, ubv::fvec2{ 0, 0 } };
		ubv::Vertex c3b{ MVP * ubv::fvec4{ ubv::fvec3(1, 1, 0), 1.0F }, ubv::fvec2{ 1, 0 } };
		ubv::Vertex c3c{ MVP * ubv::fvec4{ ubv::fvec3(1, 0, 1), 1.0F }, ubv::fvec2{ 0, 1 } };
		ubv::Vertex c3d{ MVP * ubv::fvec4{ ubv::fvec3(1, 1, 1), 1.0F }, ubv::fvec2{ 1, 1 } };
		ubv::Vertex c4a{ MVP * ubv::fvec4{ ubv::fvec3(0, 0, 0), 1.0F }, ubv::fvec2{ 0, 0 } };
		ubv::Vertex c4b{ MVP * ubv::fvec4{ ubv::fvec3(0, 0, 1), 1.0F }, ubv::fvec2{ 1, 0 } };
		ubv::Vertex c4c{ MVP * ubv::fvec4{ ubv::fvec3(1, 0, 0), 1.0F }, ubv::fvec2{ 0, 1 } };
		ubv::Vertex c4d{ MVP * ubv::fvec4{ ubv::fvec3(1, 0, 1), 1.0F }, ubv::fvec2{ 1, 1 } };
		ubv::Vertex c5a{ MVP * ubv::fvec4{ ubv::fvec3(0, 1, 0), 1.0F }, ubv::fvec2{ 0, 0 } };
		ubv::Vertex c5b{ MVP * ubv::fvec4{ ubv::fvec3(0, 1, 1), 1.0F }, ubv::fvec2{ 1, 0 } };
		ubv::Vertex c5c{ MVP * ubv::fvec4{ ubv::fvec3(1, 1, 0), 1.0F }, ubv::fvec2{ 0, 1 } };
		ubv::Vertex c5d{ MVP * ubv::fvec4{ ubv::fvec3(1, 1, 1), 1.0F }, ubv::fvec2{ 1, 1 } };

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
		frame_buffer.draw_triangle(t5b, texture1);*/

		//std::vector<std::array<ubv::Vertex, 3>> triangles_to_draw = model.m_triangles;
		//std::cout << triangles_to_draw.size() << std::endl; 
		for (const auto& [texture, triangles] : model.m_triangles)
		{
			auto triangles_to_draw = triangles;
			for (auto& triangle : triangles_to_draw)
			{
				for (auto& vertex : triangle)
				{
					vertex.position = MVP * vertex.position;
				}
				frame_buffer.draw_triangle(triangle, *model.m_textures[texture]);
			}
		}

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

		//for(const auto& task : tasks) {
		//	task.wait();
		//}

		if (GetAsyncKeyState(0x5A))
			frame_buffer.draw_z_buffer();
		frame_buffer.sample();
		window->display(frame_buffer);
		window->update();
	}
}

namespace ubv {
	Sandbox::Sandbox(int t_argc, char** t_argv) : texture1{ "res/box.tga", Texture::FilteringType::LINEAR }, model{ "res/Dorrie.obj" }
	/*, texture2{ "res/test2.tga", Texture::FilteringType::NEAREST }*/ {
		for(auto i = 0; i < t_argc; ++i) {
			std::cout << "Program Input: " << t_argv[i] << "\n";
		}
	}
     
	Sandbox::~Sandbox() {
	}

    void Sandbox::start() {
		std::cout << "Hello UBVSR.\n";

		#if defined(_WIN32)
			ubv::WindowWin32 window(ubv::WindowProps{1280, 720, "Test UBVSR"});
		#elif defined(__unix__)
			ubv::WindowX11 window(ubv::WindowProps{1280, 720, "Test UBVSR"});
		#endif

		projection = fmat4x4(90.0F, static_cast<float>(window.get_win_width()) / static_cast<float>(window.get_win_height()), 0.1F, 4000.0F);

		draw_loop(&window, texture1, projection, model);

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
