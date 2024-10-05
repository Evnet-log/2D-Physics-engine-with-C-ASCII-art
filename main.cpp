#include <stdio.h>
#include <windows.h>
#include <cmath>
#include <chrono>
#include <limits.h>
#include <algorithm>
#include <vector>
#include <set>

#include "thing.h"
#include "sat_collision.h"
#include "delaunay_triangulate.h"
#include "renderer.h"
#include "container.h"
#include "control_screen.h"

int main()
{
	system("mode con: cols=200 lines=70");
	c();
	
	RENDERER renderer(MAX_X, MAX_Y);
	CONTAINER container;
	
	CONTROLLER controller(container, renderer);
	controller.off();
	
	const std::chrono::milliseconds frame_duration(1000 / 250);
	
	auto start_time = std::chrono::high_resolution_clock::now();
    auto end_time = std::chrono::high_resolution_clock::now();
    auto elapsed_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
	
	float dt = 1.0f/60.0f;
	
	container.set_accel({0.0f, 20.0f});
	container.set_dt(dt);
	container.set_repeat(10);
	
	bool return_btn = false;
	bool space_btn = false;
	bool menu_btn = true;
	
	int menu = 0;
	
	bool print_window = true;

	while (1)
	{
		if (GetAsyncKeyState(VK_SPACE) && !menu_btn)
		{
			if (menu == 1)
			  controller.off(), menu = 0;
			else
			  controller.on(), menu = 1;
			
			menu_btn = true;
		}
		
		if (!GetAsyncKeyState(VK_SPACE))
		  menu_btn = false;

		if (menu == 0)
		{
			if (elapsed_time > frame_duration)
			{
				start_time = std::chrono::high_resolution_clock::now();
				
				container.update();
				
				if (GetAsyncKeyState(VK_RETURN) && !return_btn)
				  container.change_gravity(), return_btn = true;
				if (!GetAsyncKeyState(VK_RETURN))
				  return_btn = false;
				
				print_window = true;
			}
	
		    end_time = std::chrono::high_resolution_clock::now();
		    elapsed_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
		}
		
		else if (menu == 1)
		  print_window = controller.use();
		
		
		if (print_window)
		{
			renderer.clear();
			
			controller.draw_constraint();
			
			renderer.draw(container.all_shapes);
			renderer.window();
			
			print_window = false;
		}
	}

    return 0;
}








