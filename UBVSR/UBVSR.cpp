#include "UBVSR.hpp"

#include "FpsCounter.hpp"

ubv::vec2<float> rotate(ubv::vec2<float> t_point, ubv::vec2<float> t_origin, float t_angle)
{
	float s = sin(t_angle);
	float c = cos(t_angle);

	// translate point back to origin:
	t_point.x -= t_origin.x;
	t_point.y -= t_origin.y;

	// rotate point
	float xnew = t_point.x * c - t_point.y * s;
	float ynew = t_point.x * s + t_point.y * c;

	// translate point back:
	t_point.x = xnew + t_origin.x;
	t_point.y = ynew + t_origin.y;
	return t_point;
}

void draw_loop(ubv::WindowWin32* window) noexcept
{
	const Timepoint t1;
	ubv::FrameBuffer frame_buffer(window->get_win_width(), window->get_win_height());
	while (true)
	{
		frame_buffer.clear(ubv::Pixel(0, 0, 0));
		static FpsCounter fps_counter;

		const Timepoint t2;
		double elapsed_time = (t2 - t1) * 8.0L;
		printf("FPS: %i \n", fps_counter.update(t2));

		for (int x = 0; x < window->get_win_width(); ++x)
		{
			for (int y = 0; y < window->get_win_height(); ++y)
			{
				static std::array<unsigned, 3> colors;
				colors[0] = (x) / 4 + elapsed_time * 8.0;
				colors[1] = (y + x) / 4 + elapsed_time * 8.0;
				colors[2] = (y) / 4 + elapsed_time * 8.0;

				for (auto &color : colors)
				{
					if (color >= 512)
					{
						color %= 512;
					}
					if (color > 255 && color < 512)
					{
						color = 255 - (color - 256);
					}
				}
				//frame_buffer.set_pixel(x, y, ubv::Pixel(colors[0], colors[1], colors[2]));
			}
		}
		float zz = std::abs(sin(elapsed_time) * 200.0F);
		/*frame_buffer.draw_triangle({
			ubv::FrameBuffer::Vertex{{0.0F-zz, 0.0F}, ubv::Pixel(255, 255, 255)},
			ubv::FrameBuffer::Vertex{{0.0F-zz, 720.0F}, ubv::Pixel(0, 0, 0)},
			ubv::FrameBuffer::Vertex{{1280.0F+zz, 0.0F}, ubv::Pixel(0, 0, 0)}
			});
		frame_buffer.draw_triangle({
			ubv::FrameBuffer::Vertex{{0.0F, 720.0F}, ubv::Pixel(0, 0, 0)},
			ubv::FrameBuffer::Vertex{{1280.0F+zz, 0.0F}, ubv::Pixel(0, 0, 0)},
			ubv::FrameBuffer::Vertex{{1280.0F+zz, 720.0F}, ubv::Pixel(255, 255, 255)}
			});*/
		ubv::vec2<float> point_a = rotate({ 640-400,360+346 }, { 640.0F,360.0F }, elapsed_time / 8.0);
		ubv::vec2<float> point_b = rotate({ 640,360-346 }, { 640.0F,360.0F }, elapsed_time / 8.0);
		ubv::vec2<float> point_c = rotate({ 640+400,360+346 }, { 640.0F,360.0F }, elapsed_time / 8.0);
		frame_buffer.draw_triangle({
			ubv::FrameBuffer::Vertex{point_a, ubv::Pixel(255, 0, 0)},
			ubv::FrameBuffer::Vertex{point_b, ubv::Pixel(0, 255, 0)},
			ubv::FrameBuffer::Vertex{point_c, ubv::Pixel(0, 0, 255)}
			});
		//frame_buffer.draw_colored_triangle(point_a, point_b, point_c, ubv::Pixel(0, 255, 0), ubv::Pixel(255, 0, 0), ubv::Pixel(0, 0, 255));
		//frame_buffer.draw_fancy_glowy_circle(500, 500, std::abs(500.0 * sin(elapsed_time / 4.0)));
		//frame_buffer.draw_triangle({20,20}, {170,670}, {800,300}, ubv::Pixel(0, 0, std::abs(sin(elapsed_time / 8.0)) * 255.0F));
		//frame_buffer.draw_triangle({20,20}, {170,670}, {0,720}, ubv::Pixel(255, 255, std::abs(cos(elapsed_time / 8.0)) * 255.0F));
		//frame_buffer.draw_line({0, 720}, {1280, 0}, ubv::Pixel(255, 0, 0), ubv::Pixel(0, 255, 0));
		//frame_buffer.draw_line({0, 0}, {1280, 720}, ubv::Pixel(255, 0, 0), ubv::Pixel(0, 0, 255));
		window->display(frame_buffer);
		//window->update();
	}
}

void loop()
{

	ubv::WindowWin32 window(ubv::WindowProps{1280, 720, "Test UBVSR"});
	std::thread draw_thread(draw_loop, &window); //draw_loop(&window);
	draw_thread.detach();

	while (true)
	{
		window.update();
	}
}

int main(int, char **)
try
{
	std::cout << "Hello UBVSR.\n";

	loop();

	std::cout << "Goodbye UBVSR\n";
	std::cin.get();
	return 0;
}
catch (const std::exception &exception)
{
	std::cout << "Fatal error!\n" << exception.what() << std::flush;
}
