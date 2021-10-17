﻿#include "UBVSR.hpp"

#include "FpsCounter.hpp"

void draw_loop(ubv::WindowWin32* window) noexcept
{
	const auto t1 = std::chrono::steady_clock::now();
	while (true)
	{
		FrameBuffer frame_buffer(window->get_win_width(), window->get_win_height());
		static FpsCounter fps_counter;
		// InvalidateRect(window.get_hwnd(), 0, true);

		const auto t2 = std::chrono::steady_clock::now();
		double elapsed_time = std::chrono::duration<double>(t2 - t1).count() * 8.0L;
		printf("FPS: %i \n", fps_counter.update());
		//std::vector<std::thread> threads;
		//for (unsigned i = 0; i < threads_n; ++i)
		//{
			//threads.emplace_back([&]() {

		for (int x = 0; x < window->get_win_width(); ++x)
		{
			for (int y = 0; y < window->get_win_height(); ++y)
			{

				std::array<unsigned, 3> colors;
				colors[0] = (x) / 4 + elapsed_time * 8.0;
				colors[1] = (y + x) / 4 + elapsed_time * 8.0;
				colors[2] = (y) / 4 + elapsed_time * 8.0;

				for (auto& color : colors)
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

				//tasks.emplace_back(std::async(std::launch::async, &ubv::WindowWin32::set_pixel, &window, x, y, colors[0], colors[1], colors[2]));

				//window->set_pixel(x, y, colors[0], colors[1], colors[2]);
				frame_buffer.set_pixel(x, y, Pixel( colors[0], colors[1], colors[2] ));

				// std::async(std::launch::async, [&]() { window.set_pixel(x, y, x + elapsed_time * 8.0, y + elapsed_time * 8.0, (x + y) + elapsed_time * 8.0); });
			}
		}
		//});
		//threads[i].join();


		window->display(frame_buffer);
	}
	//threads.back().join();
}

void loop()
{
	ubv::WindowWin32 window(ubv::WindowProps{ 1280, 720, "Test UBVSR" });
	std::thread draw_thread{ draw_loop, &window };
	draw_thread.detach();

	while (true)
	{
		window.update();
		std::this_thread::yield();
	}
}

int main(int, char **)
try
{
	std::cout << "Hello UBVSR.\n";

	std::thread t1(loop);
	t1.join();
	//loop();

	std::cout << "Goodbye UBVSR\n";
	std::cin.get();
	return 0;
}
catch (const std::exception &exception)
{
	std::cout << "Fatal error!\n" << exception.what() << std::flush;
}
