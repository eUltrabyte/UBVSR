﻿#include "UBVSR.hpp"

#include "FpsCounter.hpp"

void loop()
{
	ubv::WindowWin32 window(ubv::WindowProps{1280, 720, "Test UBVSR"});

	auto t1 = std::chrono::steady_clock::now();

	while (true)
	{
		SwapBuffers(window.get_hdc());

		static FpsCounter fps_counter;
		// InvalidateRect(window.get_hwnd(), 0, true);

		auto t2 = std::chrono::steady_clock::now();
		double elapsed_time = std::chrono::duration<double>(t2 - t1).count();
		std::cout << "FPS: " << fps_counter.update() << std::endl;

		const auto threads_n = std::thread::hardware_concurrency();
		std::cout << threads_n << "\n";

		std::vector<std::thread> threads;
		for (unsigned i = 0; i < threads_n; ++i)
		{
			threads.emplace_back([&]() {
				for (int x = i; x < window.get_win_width(); x += threads_n)
				{
					for (int y = i; y < window.get_win_height(); y += threads_n)
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

						window.set_pixel(x, y, colors[0], colors[1], colors[2]);

						// std::async(std::launch::async, [&]() { window.set_pixel(x, y, x + elapsed_time * 8.0, y + elapsed_time * 8.0, (x + y) + elapsed_time * 8.0); });
					}
				}
			});
			threads[i].join();
		}


		window.update();
	}
}

int main(int, char **)
try
{
	std::cout << "Hello UBVSR.\n";

	//std::thread t1(loop);
	//t1.join();
	loop();

	std::cout << "Goodbye UBVSR\n";

	std::cin.get();

	return 0;
}
catch (const std::exception &exception)
{
	std::cout << "Fatal error!\n" << exception.what() << std::flush;
}
