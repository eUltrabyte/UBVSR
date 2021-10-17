#include "UBVSR.hpp"

#include "FpsCounter.hpp"

void draw_loop(ubv::WindowWin32 *window) noexcept
{
	const Timepoint t1;
	FrameBuffer frame_buffer(window->get_win_width(), window->get_win_height());
	while (true)
	{
		static FpsCounter fps_counter;

		const Timepoint t2;
		double elapsed_time = (t2 - t1) * 8.0L;
		printf("FPS: %i \n", fps_counter.update(t2));

		for (int x = 0; x < window->get_win_width(); ++x)
		{
			for (int y = 0; y < window->get_win_height(); ++y)
			{
				std::array<unsigned, 3> colors;
				colors[0] = (x) / 4 + elapsed_time * 8.0;
				colors[1] = (y + x) / 4 + elapsed_time * 8.0;
				colors[2] = (y) / 4 + elapsed_time * 8.0;

				for (auto &color : colors)
				{
					while (color >= 512)
					{
						color -= 512;
					}
					if (color > 255 && color < 512)
					{
						color = 255 - (color - 256);
					}
				}
				frame_buffer.set_pixel(x, y, Pixel(colors[0], colors[1], colors[2]));
			}
		}
		frame_buffer.draw_fancy_glowy_circle(500, 500, std::abs(500.0 * sin(elapsed_time / 4.0)));
		frame_buffer.draw_triangle({20,20}, {170,670}, {800,300}, Pixel(0, 0, std::abs(sin(elapsed_time / 8.0)) * 255.0F));
		frame_buffer.draw_line({0, 720}, {1280, 0}, Pixel{255, 0, 0});
		frame_buffer.draw_line({0, 0}, {1280, 720}, Pixel{255, 0, 0});
		window->display(frame_buffer);
	}
}

void loop()
{
	ubv::WindowWin32 window(ubv::WindowProps{1280, 720, "Test UBVSR"});
	std::thread draw_thread{draw_loop, &window};
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
