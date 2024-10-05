#ifndef RENDERER_H
#define RENDERER_H


class RENDERER
{
	private:
		int M_X;
		int M_Y;
		
		char *print_map;
		char **map;
	
	public:
		RENDERER(int M_X, int M_Y)
		  :M_X(M_X), M_Y(M_Y)
		{
			print_map = new char[(M_X+1) * M_Y];
			map = new char*[M_Y];
			
			for (int n = 0; n < M_Y; ++n)
			{
				map[n] = print_map + n*(M_X+1);
				print_map[n*(M_X+1) + M_X] = '\n';
			}
			
			print_map[(M_X+1)*M_Y-1] = 0;
			
			clear();
		}
		
		void clear()
		{
			for (int y = 0; y < M_Y; ++y)
			{
				for (int x = 0; x < M_X; ++x)
				  map[y][x] = ' ';
			}
		}
		
		void draw(std::vector<SHAPE*> &all_shapes)
		{
			for (const auto &shape : all_shapes)
			{
				if (shape->points.size() == 1) // circle
				  draw_circle(shape->points[0]->pos, shape->points[0]->radius);
				else // polygon
				  draw_polygon(shape->points);
			}
		}
		
		void draw_line(Vector2f p1, Vector2f p2)
		{
			std::vector<COOR> coors = bresenham_line(p1, p2);
				
			for (const auto &coor : coors)
			{
				if (coor.x >= 0 && coor.x < (M_X)/2 && coor.y >= 0 && coor.y < M_Y)
				{
					map[coor.y][coor.x*2] = '.';
					map[coor.y][coor.x*2+1] = '.';
				}
			}
		}
		
		void draw_polygon(std::vector<Point*> points)
		{
			std::vector<COOR> coors;
			size_t size = points.size();
			
			for (int n = 0; n < size; ++n)
			{
				int l = n+1 < size ? n+1 : 0;
				
				coors = bresenham_line(points[n]->pos, points[l]->pos);
				
				for (const auto &coor : coors)
				{
					if (coor.x >= 0 && coor.x < (M_X)/2 && coor.y >= 0 && coor.y < M_Y)
					{
						map[coor.y][coor.x*2] = '.';
						map[coor.y][coor.x*2+1] = '.';
					}
				}
			}
		}
		
		void draw_circle(Vector2f pos, float radius)
		{
			int x = static_cast<int>(round(pos.x));
			int y = static_cast<int>(round(pos.y));
			
			int count = 8;
			
			if (radius > 2)
			  count += (radius-2) * 8;
			
			float radian = (M_PI*2.0f)/static_cast<float>(count);
			
			for (int n = 0; n < count; ++n)
			{
				int px = x + static_cast<int>(round(std::cos(radian*n)*radius));
				int py = y + static_cast<int>(round(std::sin(radian*n)*radius));
				
				if (px >= 0 && px < (M_X)/2 && py >= 0 && py < M_Y)
				{
					map[py][px*2] = '.';
					map[py][px*2+1] = '.';
				}
			}
		}
		
		void window()
		{
			gotoxy(1,1);
			printf("%s", print_map);
		}
};













#endif
