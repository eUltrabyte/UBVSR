#include "UBVSR.hpp"

#include "Buffer.hpp"
#include "FpsCounter.hpp"
#include "TextRenderer.hpp"

#include <future>
#include <string_view>

void zzz()
{
	// ubv::StencilBuffer color_buffer(640, 480);
	// color_buffer.at(40, 50) = true;
	// const auto &bb = color_buffer;
	// auto &z = bb.at(40, 50);
	// color_buffer.clear();
	// ubv::DepthBuffer depth_buffer(ubv::u16vec2{640,480});
	// ubv::StencilBuffer stencil_buffer(ubv::u16vec2{640,480});
	// stencil_buffer.at(ubv::u16vec2{ 3,4 }) = true;
	// std::vector<float> yaa{std::size_t(5000) * uint16_t(3), std::numeric_limits<float>::infinity() };
}

ubv::fvec2 rotate(ubv::fvec2 t_point, ubv::fvec2 t_origin, float t_angle)
{
	float s = std::sin(t_angle);
	float c = std::cos(t_angle);

	// translate point back to origin
	t_point.x -= t_origin.x;
	t_point.y -= t_origin.y;

	// rotate point
	float xnew = t_point.x * c - t_point.y * s;
	float ynew = t_point.x * s + t_point.y * c;

	// translate point back
	t_point.x = xnew + t_origin.x;
	t_point.y = ynew + t_origin.y;
	return t_point;
}

void draw_loop(ubv::Window *window, ubv::Texture &texture1, ubv::fmat4x4 &projection, std::vector<ubv::Model *> models,
			   const ubv::Texture &grass_texture, const ubv::Texture &skybox1_texture,
			   const ubv::Texture &skybox2_texture, const ubv::Texture &skybox3_texture,
			   const ubv::Texture &skybox4_texture, const ubv::Texture &skybox_top_texture,
			   const ubv::Texture &skybox_bottom_texture, ubv::TextRenderer& t_text_renderer) noexcept
{
	const Timepoint t1;
	ubv::FrameBuffer frame_buffer(window->get_win_width(), window->get_win_height());
	frame_buffer.fog_params = ubv::FrameBuffer::FogParams{
		0.1F,                     // fog start
		100.0F,                    // fog end
		ubv::Pixel{100, 150, 200}, // fog color
		1.0F,                     // fog destiny
		true                      // fog enabled
	};

	/*frame_buffer.fog_params = ubv::FrameBuffer::FogParams{
		9.0F,                     // fog start
		10.0F,                    // fog end
		ubv::Pixel{255, 0, 0}, // fog color
		1.0F,                     // fog destiny
		true                      // fog enabled
	};*/

	frame_buffer.set_multisample(1);
	bool rotateMode = false;
	bool renderZBuffer = false;
	while (true)
	{
		static bool debug_info = false;
		ubv::Log::log("[NEW FRAME]");
		frame_buffer.clear();
		static FpsCounter fps_counter;
		static Timepoint t3 = t1;
		const Timepoint t2;
		float elapsed_time = (t2 - t1);
		float delta_time = (t2 - t3) * 4.0F;
		//std::cout << "FPS: " << fps_counter.update(t2) << '\n';
		//std::printf("FPS: %u\n", fps_counter.update(t2));
		t3 = t2;
		static ubv::fvec3 camera_position{-10.2F, 3.6F, -8.2F};
		static ubv::fvec3 camera_pitch_yaw_roll{ubv::degrees_to_radians(0.0F), ubv::degrees_to_radians(-180.0F), 0.0F};

		static unsigned frame_number = 0;

		auto yaw_dir = ubv::rotate_2d(ubv::fvec2(0, 1), camera_pitch_yaw_roll.y);

		const auto w_pressed = window->IsKeyPressed(ubv::keys.at(ubv::Keys::W));
		const auto s_pressed = window->IsKeyPressed(ubv::keys.at(ubv::Keys::S));
		const auto a_pressed = window->IsKeyPressed(ubv::keys.at(ubv::Keys::A));
		const auto d_pressed = window->IsKeyPressed(ubv::keys.at(ubv::Keys::D));

		const auto space_pressed = window->IsKeyPressed(ubv::keys.at(ubv::Keys::Space));
		const auto shift_pressed = window->IsKeyPressed(ubv::keys.at(ubv::Keys::Shift));

		const auto p_up_down = w_pressed ^ s_pressed;
		const auto p_left_right = a_pressed ^ d_pressed;

		const auto p_space_shift = space_pressed ^ shift_pressed;

		if (p_up_down || p_left_right || p_space_shift)
		{
			ubv::fvec3 dir_to_move{ 0.0F,0.0F,0.0F };
			if (p_up_down)
			{
				dir_to_move.z = w_pressed ? 1 : -1;
			}
			if (p_left_right)
			{
				dir_to_move.x = a_pressed ? 1 : -1;
			}
			if (p_space_shift)
			{
				dir_to_move.y = space_pressed ? 1 : -1;
			}
			dir_to_move = ubv::normalize(dir_to_move);
			auto yaw_dir = ubv::rotate_2d(ubv::fvec2(dir_to_move.x, dir_to_move.z), camera_pitch_yaw_roll.y);
			dir_to_move = ubv::fvec3(yaw_dir.x, dir_to_move.y, yaw_dir.y);

			camera_position = camera_position + dir_to_move * ubv::fvec3(delta_time, delta_time, delta_time);
		}

		if (window->IsKeyPressed(ubv::keys.at(ubv::Keys::E)))
		{
			camera_pitch_yaw_roll.z -= delta_time / 4.0F;
		}
		if (window->IsKeyPressed(ubv::keys.at(ubv::Keys::Q)))
		{
			camera_pitch_yaw_roll.z += delta_time / 4.0F;
		}
		if (window->IsKeyPressed(ubv::keys.at(ubv::Keys::Up)))
		{
			camera_pitch_yaw_roll.x -= delta_time / 4.0F;
		}
		if (window->IsKeyPressed(ubv::keys.at(ubv::Keys::Down)))
		{
			camera_pitch_yaw_roll.x += delta_time / 4.0F;
		}
		if (window->IsKeyPressed(ubv::keys.at(ubv::Keys::Left)))
		{
			camera_pitch_yaw_roll.y -= delta_time / 4.0F;
		}
		if (window->IsKeyPressed(ubv::keys.at(ubv::Keys::Right)))
		{
			camera_pitch_yaw_roll.y += delta_time / 4.0F;
		}

		/*if (window->IsKeyPressed(ubv::keys.at(ubv::Keys::Enter)))
		{
			rotateMode = !rotateMode;
		}

		if (window->IsKeyPressed(ubv::keys.at(ubv::Keys::Left)))
		{
			if (rotateMode)
			{
				camera_pitch_yaw_roll.x -= delta_time / 3.0F;
			}
			else
			{
				camera_position.x += delta_time;
			}
		}

		if (window->IsKeyPressed(ubv::keys.at(ubv::Keys::Right)))
		{
			if (rotateMode)
			{
				camera_pitch_yaw_roll.x += delta_time / 3.0F;
			}
			else
			{
				camera_position.x += delta_time;
			}
		}

		if (window->IsKeyPressed(ubv::keys.at(ubv::Keys::Up)))
		{
			if (rotateMode)
			{
				camera_pitch_yaw_roll.z += delta_time / 3.0F;
			}
			else
			{
				camera_position.z += delta_time;
			}
		}

		if (window->IsKeyPressed(ubv::keys.at(ubv::Keys::Down)))
		{
			if (rotateMode)
			{
				camera_pitch_yaw_roll.z -= delta_time / 3.0F;
			}
			else
			{
				camera_position.z -= delta_time;
			}
		}

		if (window->IsKeyPressed(ubv::keys.at(ubv::Keys::W)))
		{
			camera_position.x += yaw_dir.x * delta_time;
			camera_position.z += yaw_dir.y * delta_time;
		}

		if (window->IsKeyPressed(ubv::keys.at(ubv::Keys::S)))
		{
			if (rotateMode)
			{
				camera_pitch_yaw_roll.y -= delta_time / 3.0F;
			}
			else
			{
				camera_position.y -= delta_time;
			}
		}*/

		const auto pitch = camera_pitch_yaw_roll.x;
		const auto yaw   = camera_pitch_yaw_roll.y;
		const auto roll  = camera_pitch_yaw_roll.z;

		//std::cout << "pitch: " << ubv::radians_to_degrees(pitch) << "   yaw: " << ubv::radians_to_degrees(yaw) << "   roll: " << ubv::radians_to_degrees(roll) << std::endl;

		const auto front = ubv::rotate_3d(ubv::fvec3(0, 0, 1), pitch, yaw, roll);
		const auto up = ubv::rotate_3d(ubv::fvec3(0, 1, 0), pitch, yaw, roll);

		const auto view = ubv::look_at(camera_position, camera_position + front, up);

		const auto pv_matrix = projection * view ;

		frame_buffer.fog_params.enable = false;
		frame_buffer.depth_test = false;

		ubv::Vertex skyboxline1_a{ubv::fvec4(-1, -1, -1, 1.0F), ubv::fvec2(1.0F, 0.0F)};
		ubv::Vertex skyboxline1_b{ubv::fvec4(1, -1, -1, 1.0F), ubv::fvec2(0.0F, 0.0F)};
		ubv::Vertex skyboxline1_c{ubv::fvec4(1, 1, -1, 1.0F), ubv::fvec2(0.0F, 1.0F)};
		ubv::Vertex skyboxline1_d{ubv::fvec4(-1, 1, -1, 1.0F), ubv::fvec2(1.0F, 1.0F)};

		ubv::Vertex skyboxline2_a{ubv::fvec4(-1, -1, 1, 1.0F), ubv::fvec2(0.0F, 0.0F)};
		ubv::Vertex skyboxline2_b{ubv::fvec4(1, -1, 1, 1.0F), ubv::fvec2(1.0F, 0.0F)};
		ubv::Vertex skyboxline2_c{ubv::fvec4(1, 1, 1, 1.0F), ubv::fvec2(1.0F, 1.0F)};
		ubv::Vertex skyboxline2_d{ubv::fvec4(-1, 1, 1, 1.0F), ubv::fvec2(0.0F, 1.0F)};

		ubv::Vertex skyboxline3_a{ubv::fvec4(-1, -1, -1, 1.0F), ubv::fvec2(0.0F, 0.0F)};
		ubv::Vertex skyboxline3_b{ubv::fvec4(-1, -1, 1, 1.0F), ubv::fvec2(1.0F, 0.0F)};
		ubv::Vertex skyboxline3_c{ubv::fvec4(-1, 1, 1, 1.0F), ubv::fvec2(1.0F, 1.0F)};
		ubv::Vertex skyboxline3_d{ubv::fvec4(-1, 1, -1, 1.0F), ubv::fvec2(0.0F, 1.0F)};

		ubv::Vertex skyboxline4_a{ubv::fvec4(1, -1, -1, 1.0F), ubv::fvec2(1.0F, 0.0F)};
		ubv::Vertex skyboxline4_b{ubv::fvec4(1, -1, 1, 1.0F), ubv::fvec2(0.0F, 0.0F)};
		ubv::Vertex skyboxline4_c{ubv::fvec4(1, 1, 1, 1.0F), ubv::fvec2(0.0F, 1.0F)};
		ubv::Vertex skyboxline4_d{ubv::fvec4(1, 1, -1, 1.0F), ubv::fvec2(1.0F, 1.0F)};

		ubv::Vertex skyboxlineT_a{ubv::fvec4(-1, 1, -1, 1.0F), ubv::fvec2(0.0F, 0.0F)};
		ubv::Vertex skyboxlineT_b{ubv::fvec4(1, 1, -1, 1.0F), ubv::fvec2(0.0F, 1.0F)};
		ubv::Vertex skyboxlineT_c{ubv::fvec4(-1, 1, 1, 1.0F), ubv::fvec2(1.0F, 0.0F)};
		ubv::Vertex skyboxlineT_d{ubv::fvec4(1, 1, 1, 1.0F), ubv::fvec2(1.0F, 1.0F)};

		ubv::Vertex skyboxlineB_a{ubv::fvec4(-1, -1, -1, 1.0F), ubv::fvec2(0.0F, 1.0F)};
		ubv::Vertex skyboxlineB_b{ubv::fvec4(1, -1, -1, 1.0F), ubv::fvec2(0.0F, 0.0F)};
		ubv::Vertex skyboxlineB_d{ubv::fvec4(1, -1, 1, 1.0F), ubv::fvec2(1.0F, 0.0F)};
		ubv::Vertex skyboxlineB_c{ubv::fvec4(-1, -1, 1, 1.0F), ubv::fvec2(1.0F, 1.0F)};

		ubv::Vertex *skybox_vertices[] = {
			&skyboxline1_a, &skyboxline1_b, &skyboxline1_c, &skyboxline1_d, &skyboxline2_a, &skyboxline2_b,
			&skyboxline2_c, &skyboxline2_d, &skyboxline3_a, &skyboxline3_b, &skyboxline3_c, &skyboxline3_d,
			&skyboxline4_a, &skyboxline4_b, &skyboxline4_c, &skyboxline4_d, &skyboxlineT_a, &skyboxlineT_b,
			&skyboxlineT_c, &skyboxlineT_d, &skyboxlineB_a, &skyboxlineB_b, &skyboxlineB_c, &skyboxlineB_d};

		for (auto skybox_vertex : skybox_vertices)
		{
			skybox_vertex->position =
				pv_matrix * ubv::fvec4(ubv::fvec3(skybox_vertex->position) + camera_position, 1.0F);
		}

		std::array<ubv::Vertex, 3> skybox_triangle1_a{skyboxline1_a, skyboxline1_b, skyboxline1_c};
		std::array<ubv::Vertex, 3> skybox_triangle1_b{skyboxline1_a, skyboxline1_d, skyboxline1_c};

		std::array<ubv::Vertex, 3> skybox_triangle2_a{skyboxline2_a, skyboxline2_b, skyboxline2_c};
		std::array<ubv::Vertex, 3> skybox_triangle2_b{skyboxline2_a, skyboxline2_d, skyboxline2_c};

		std::array<ubv::Vertex, 3> skybox_triangle3_a{skyboxline3_a, skyboxline3_b, skyboxline3_c};
		std::array<ubv::Vertex, 3> skybox_triangle3_b{skyboxline3_a, skyboxline3_d, skyboxline3_c};

		std::array<ubv::Vertex, 3> skybox_triangle4_a{skyboxline4_a, skyboxline4_b, skyboxline4_c};
		std::array<ubv::Vertex, 3> skybox_triangle4_b{skyboxline4_a, skyboxline4_d, skyboxline4_c};

		std::array<ubv::Vertex, 3> skybox_triangleT_a{skyboxlineT_a, skyboxlineT_d, skyboxlineT_b};
		std::array<ubv::Vertex, 3> skybox_triangleT_b{skyboxlineT_a, skyboxlineT_d, skyboxlineT_c};

		std::array<ubv::Vertex, 3> skybox_triangleB_a{skyboxlineB_a, skyboxlineB_d, skyboxlineB_b};
		std::array<ubv::Vertex, 3> skybox_triangleB_b{skyboxlineB_a, skyboxlineB_d, skyboxlineB_c};

		frame_buffer.prepare_triangle(skybox_triangle1_a, skybox3_texture);
		frame_buffer.prepare_triangle(skybox_triangle1_b, skybox3_texture);

		frame_buffer.prepare_triangle(skybox_triangle2_a, skybox1_texture);
		frame_buffer.prepare_triangle(skybox_triangle2_b, skybox1_texture);

		frame_buffer.prepare_triangle(skybox_triangle3_a, skybox4_texture);
		frame_buffer.prepare_triangle(skybox_triangle3_b, skybox4_texture);

		frame_buffer.prepare_triangle(skybox_triangle4_a, skybox2_texture);
		frame_buffer.prepare_triangle(skybox_triangle4_b, skybox2_texture);

		frame_buffer.prepare_triangle(skybox_triangleT_a, skybox_top_texture);
		frame_buffer.prepare_triangle(skybox_triangleT_b, skybox_top_texture);

		frame_buffer.prepare_triangle(skybox_triangleB_a, skybox_bottom_texture);
		frame_buffer.prepare_triangle(skybox_triangleB_b, skybox_bottom_texture);
		
		frame_buffer.draw_prepared_triangles();

		frame_buffer.fog_params.enable = true;
		frame_buffer.depth_test = true;

		for (const auto &model : models)
		{
			auto pvm_matrix = pv_matrix * model->model_matrix;
			for (const auto &[texture, triangles] : model->m_triangles)
			{
				auto triangles_to_draw = triangles;

				for (auto &triangle : triangles_to_draw)
				{
					for (auto &vertex : triangle)
					{
						vertex.position = pvm_matrix * vertex.position;
					}
					frame_buffer.prepare_triangle(triangle, *model->m_textures[texture]);
				}
			}
		}

		frame_buffer.draw_prepared_triangles();

		constexpr float grass_y_offset = -3.16F;

		ubv::Vertex grass_a{ubv::fvec4(-50.0F, grass_y_offset, -50.0F, 1.0F), ubv::fvec2(0.0F, 0.0F)};
		ubv::Vertex grass_b{ubv::fvec4(50.0F, grass_y_offset, -50.0F, 1.0F), ubv::fvec2(8.0F, 0.0F)};
		ubv::Vertex grass_c{ubv::fvec4(50.0F, grass_y_offset, 50.0F, 1.0F), ubv::fvec2(8.0F, 8.0F)};
		ubv::Vertex grass_d{ubv::fvec4(-50.0F, grass_y_offset, 50.0F, 1.0F), ubv::fvec2(0.0F, 8.0F)};

		grass_a.position = pv_matrix * grass_a.position;
		grass_b.position = pv_matrix * grass_b.position;
		grass_c.position = pv_matrix * grass_c.position;
		grass_d.position = pv_matrix * grass_d.position;

		std::array<ubv::Vertex, 3> grass_triangle_a{grass_a, grass_b, grass_c};
		std::array<ubv::Vertex, 3> grass_triangle_b{grass_a, grass_d, grass_c};

		std::string s_fps_num = "FPS: " +  std::to_string(fps_counter.update(t2));

		std::string s_triangles = "Triangles: " + std::to_string(frame_buffer.triangles);

		std::string s_pos_x = "X: " + std::to_string(camera_position.x);
		std::string s_pos_y = "Y: " + std::to_string(camera_position.y);
		std::string s_pos_z = "Z: " + std::to_string(camera_position.z);

		std::string s_angle_p = "Pitch: " + std::to_string(ubv::radians_to_degrees(camera_pitch_yaw_roll.x));
		std::string s_angle_y = "Yaw: " + std::to_string(ubv::radians_to_degrees(camera_pitch_yaw_roll.y));
		std::string s_angle_r = "Roll: " + std::to_string(ubv::radians_to_degrees(camera_pitch_yaw_roll.z));

		std::string s_width = "Width: " + std::to_string(frame_buffer.get_width());
		std::string s_height = "Height: " + std::to_string(frame_buffer.get_height());

		std::string s_multisampling = "Multisampling: " + (frame_buffer.get_multisample() > 1 ? std::to_string(static_cast<unsigned>(frame_buffer.get_multisample()) * frame_buffer.get_multisample()) + "x" : std::string("off"));

		std::string s_sampled_pixels = "Sampled pixels: " + std::to_string(frame_buffer.sampled_pixels);

		std::string s_info = "Ultrabyte & Volian Software Rasterizer";

		//frame_buffer.draw_z_buffer();

		frame_buffer.depth_test = false;
		frame_buffer.fog_params.enable = false;

		//t_text_renderer.RenderTextLine(frame_buffer, s_info, 0, frame_buffer.get_height() - 18);

		if (window->IsKeyPressed(ubv::keys.at(ubv::Keys::Escape)))
		{
			debug_info = !debug_info;
		}

		if (debug_info)
		{
			t_text_renderer.RenderTextLine(frame_buffer, s_info, 0, frame_buffer.get_height() - 18);

			t_text_renderer.RenderTextLine(frame_buffer, s_fps_num, 0, 0 * 18);

			t_text_renderer.RenderTextLine(frame_buffer, s_triangles, 0, 2 * 18);

			t_text_renderer.RenderTextLine(frame_buffer, s_pos_x, 0, 4 * 18);
			t_text_renderer.RenderTextLine(frame_buffer, s_pos_y, 0, 5 * 18);
			t_text_renderer.RenderTextLine(frame_buffer, s_pos_z, 0, 6 * 18);

			t_text_renderer.RenderTextLine(frame_buffer, s_angle_p, 0, 8 * 18);
			t_text_renderer.RenderTextLine(frame_buffer, s_angle_y, 0, 9 * 18);
			t_text_renderer.RenderTextLine(frame_buffer, s_angle_r, 0, 10 * 18);

			t_text_renderer.RenderTextLine(frame_buffer, s_width, 0, 12 * 18);
			t_text_renderer.RenderTextLine(frame_buffer, s_height, 0, 13 * 18);

			t_text_renderer.RenderTextLine(frame_buffer, s_multisampling, 0, 15 * 18);

		}
		frame_buffer.draw_prepared_triangles();
		//t_text_renderer.RenderTextLine(frame_buffer, s_sampled_pixels, 0, 17 * 18);
		
		frame_buffer.sample();
		//frame_buffer.render_to_file("yacieyacie9292929.tga");
		window->display(frame_buffer);
		window->update();
		//while (true)
		//{
			//std::this_thread::yield();
		//}
	}
}

namespace ubv
{
Sandbox::Sandbox(int t_argc, char **t_argv)
	: texture1{"res/grass1024.tga", Texture::FilteringType::BILINEAR}, model_akwarium{"res/aqua.obj"},
	  model_rekin{"res/Dorrie.obj"}, skybox1{"res/sunny1.tga", Texture::FilteringType::BILINEAR},
	  skybox2{"res/sunny2.tga", Texture::FilteringType::BILINEAR}, skybox3{"res/sunny3.tga",
																		   Texture::FilteringType::BILINEAR},
	  skybox4{"res/sunny4.tga", Texture::FilteringType::BILINEAR}, skybox_top{"res/sunny_top.tga",
																			  Texture::FilteringType::BILINEAR},
	  skybox_bottom{"res/sunny_bottom.tga", Texture::FilteringType::BILINEAR}, model_dust2{"res/Dust2.obj"}
/*, texture2{ "res/test2.tga", Texture::FilteringType::NEAREST }*/ {
	skybox_bottom.clamp = true;
	skybox1.clamp = true;
	skybox2.clamp = true;
	skybox3.clamp = true;
	skybox4.clamp = true;
	skybox_top.clamp = true;
	for (auto i = 0; i < t_argc; ++i)
	{
		std::cout << "Program Input: " << t_argv[i] << "\n";
	}
}

Sandbox::~Sandbox()
{
}

void Sandbox::start()
{
	std::cout << "Hello UBVSR.\n";

#if defined(_WIN32)
	ubv::WindowWin32 window(ubv::WindowProps{ 1280, 720, "Ultrabyte & Volian Software Rasterizer"});
#elif defined(__unix__)
	ubv::WindowX11 window(ubv::WindowProps{1280, 720, "Ultrabyte & Volian Software Rasterizer"});
#endif

	model_dust2.model_matrix = ubv::identity<float>();
	model_dust2.model_matrix.rotate(ubv::degrees_to_radians(90.0F), fvec3(-1.0, 0, 0));
	//model_dust2.model_matrix.scale(ubv::fvec3(0.5, 0.1, 0.5));

	projection = fmat4x4(
		90.0F, static_cast<float>(window.get_win_width()) / static_cast<float>(window.get_win_height()), 0.1F, 175.0F);
	Model model_wf{ "res/WF.obj" };
	model_wf.model_matrix.scale(fvec3(0.1, 0.1, 0.1));
	draw_loop(&window, texture1, projection, {&model_dust2 }, texture1, skybox1, skybox2, skybox3, skybox4,
			  skybox_top, skybox_bottom, text_renderer);

	std::cout << "Goodbye UBVSR\n";
	std::cin.get();
}
}; // namespace ubv

int main(int argc, char **argv)
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
