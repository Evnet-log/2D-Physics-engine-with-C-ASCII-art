#ifndef SAT_COLL_H
#define SAT_COLL_H


class SAT_COLLISION
{
	public:
		float min_dist{std::numeric_limits<float>::max()};
		Vector2f mv{0.0f, 0.0f};
		
		std::set<Point*> main_points;
		std::set<Point*> sub_points;
		
	public:
		SAT_COLLISION() = default;
		
		void reset()
		{
			min_dist = std::numeric_limits<float>::max();
			mv = {0.0f, 0.0f};
		}
		
		float check_collision(SHAPE &shape1, SHAPE &shape2)
		{
			if (shape2.min.x < shape1.max.x && shape1.min.x < shape2.max.x)
			{
				if (shape2.min.y < shape1.max.y && shape1.min.y < shape2.max.y)
				{
					bool check = true;

					std::vector<std::pair<Vector2f, Point*>> v1;
					std::vector<std::pair<Vector2f, Point*>> v2;
					
					for (const auto &po : shape1.points)
					  v1.emplace_back(std::pair<Vector2f, Point*>(po->pos, po));
					for (const auto &po : shape2.points)
					  v2.emplace_back(std::pair<Vector2f, Point*>(po->pos, po));
					
					if (shape1.points.size() > 1 && shape2.points.size() > 1) // p to p
					{
						check = polygon_to_polygon(v1, v2);
						
						if (check)
						  check = polygon_to_polygon(v2, v1);
					}
					
					else if (shape1.points.size() > 1 && shape2.points.size() == 1) // p to c
					{
						check = polygon_to_circle(v1, shape2.points[0]);
						
						if (check)
						  check = circle_to_polygon(shape2.points[0], v1);
					}
					
					else if (shape1.points.size() == 1 && shape2.points.size() > 1) // c to p
					{
						check = circle_to_polygon(shape1.points[0], v2);
						
						if (check)
						  check = polygon_to_circle(v2, shape1.points[0]);
					}
					
					if (check)
					{
						// slide
						if (main_points.size() + sub_points.size() == 3)
						{
							Point *mp = nullptr;
							Point *target = *sub_points.begin();
							
							float c_dist = std::numeric_limits<float>::max();
							
							for (const auto& po : main_points)
							{
								Vector2f v = target->pos - po->pos;
								float dist = v.x*v.x + v.y*v.y;
								
								if (c_dist > dist)
								  c_dist = dist, mp = po;
							}
							
							main_points.clear();
							main_points.insert(mp);
						}
						
						for (const auto &po : main_points)
						  po->pos += mv*0.1f;
						for (const auto& po : sub_points)
						  po->pos -= mv*0.1f;
						
						
						return min_dist;
					}
				}
			}
			
			return 0.0f;
		}
		
		bool polygon_to_polygon(std::vector<std::pair<Vector2f, Point*>> &main_target, std::vector<std::pair<Vector2f, Point*>> &sub_target)
		{
			size_t size = main_target.size();
			
			for (int n = 0; n < size; ++n)
			{
				int l = n+1 < size ? n+1 : 0;
				
				if (!line_check(main_target[n].first, main_target[l].first, main_target, sub_target))
				  return false;
			}
			
			return true;
		}
		
		bool polygon_to_circle(std::vector<std::pair<Vector2f, Point*>> &main_target, Point *circle)
		{
			std::vector<std::pair<Vector2f, Point*>> circle_target;
			size_t size = main_target.size();
			
			for (int n = 0; n < size; ++n)
			{
				int l = n+1 < size ? n+1 : 0;
				
				Vector2f v = main_target[n].first - main_target[l].first;
				float dist = sqrt(v.x*v.x + v.y*v.y);
				
				v *= 1.0f / dist;
				v *= circle->radius;
				
				float x = v.x * static_cast<float>(std::cos(M_PI/2.0f)) - v.y * static_cast<float>(std::sin(M_PI/2.0f));
				float y = v.y * static_cast<float>(std::cos(M_PI/2.0f)) + v.x * static_cast<float>(std::sin(M_PI/2.0f));
				
				v.x = x;
				v.y = y;
				
				circle_target.clear();
				circle_target.emplace_back(std::pair<Vector2f, Point*>(circle->pos+v, circle));
				circle_target.emplace_back(std::pair<Vector2f, Point*>(circle->pos-v, circle));
				
				if (!line_check(main_target[n].first, main_target[l].first, main_target, circle_target))
				  return false;
			}
			
			return true;
		}
		
		bool circle_to_polygon(Point *circle, std::vector<std::pair<Vector2f, Point*>> &sub_target)
		{
			std::vector<std::pair<Vector2f, Point*>> circle_target;
			Vector2f origin{0.0f, 0.0f};
			
			for (const auto &vec : sub_target)
			  origin += vec.first;
			
			origin *= 1.0f / static_cast<float>(sub_target.size());
			
			Vector2f v = origin - circle->pos;
			float dist = sqrt(v.x*v.x + v.y*v.y);
			
			v *= 1.0f / dist;
			v *= circle->radius;

			circle_target.emplace_back(std::pair<Vector2f, Point*>(circle->pos+v, circle));
			circle_target.emplace_back(std::pair<Vector2f, Point*>(circle->pos-v, circle));

			float x = v.x * static_cast<float>(std::cos(M_PI/2.0f)) - v.y * static_cast<float>(std::sin(M_PI/2.0f));
			float y = v.y * static_cast<float>(std::cos(M_PI/2.0f)) + v.x * static_cast<float>(std::sin(M_PI/2.0f));
			
			v.x = x;
			v.y = y;

			if (!line_check(circle->pos + v, circle->pos - v, circle_target, sub_target))
			  return false;
			
			return true;
		}
		
		bool line_check(Vector2f p1, Vector2f p2, std::vector<std::pair<Vector2f, Point*>> &main_target, std::vector<std::pair<Vector2f, Point*>> &sub_target)
		{
			Vector2f max1{std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest()};
			Vector2f min1{std::numeric_limits<float>::max(), std::numeric_limits<float>::max()};
			
			Vector2f max2{std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest()};
			Vector2f min2{std::numeric_limits<float>::max(), std::numeric_limits<float>::max()};
			
			float ax = p2.x - p1.x;
			float ay = p2.y - p1.y;
			
			float e = 1.0f / (ax*ax - ay*(-ay));
			
			bool big = abs(ax) < abs(ay);
			
			std::vector<std::pair<Vector2f, Point*>> main_p;
			std::vector<std::pair<Vector2f, Point*>> sub_p;
			
			for (const auto &vec : main_target)
			{
				float val = vec.first.y * ax - vec.first.x * ay;
				Vector2f v{e * val * (-ay), e * val * ax};
				
				if (big && v.x > max1.x || !big && v.y > max1.y)
				  max1 = v;
				if (big && v.x < min1.x || !big && v.y < min1.y)
				  min1 = v;
				
				main_p.emplace_back(std::pair<Vector2f, Point*>(v, vec.second));
			}
			
			for (const auto &vec : sub_target)
			{
				float val = vec.first.y * ax - vec.first.x * ay;
				Vector2f v{e * val * (-ay), e * val * ax};
				
				if (big && v.x > max2.x || !big && v.y > max2.y)
				  max2 = v;
				if (big && v.x < min2.x || !big && v.y < min2.y)
				  min2 = v;
				
				sub_p.emplace_back(std::pair<Vector2f, Point*>(v, vec.second));
			}
			
			if (big && min1.x < max2.x && min2.x < max1.x || !big && min1.y < max2.y && min2.y < max1.y) // overlap
			{
				Vector2f v1 = min2 - max1;
				Vector2f v2 = max2 - min1;
				
				float dist1 = sqrt(v1.x*v1.x + v1.y*v1.y);
				float dist2 = sqrt(v2.x*v2.x + v2.y*v2.y);
				
				float dist_check = dist1 < dist2 ? dist1 : dist2;
				
				if (min_dist > dist_check)
				{
					main_points.clear();
					sub_points.clear();
					
					for (const auto &pa : main_p)
					{
						if (dist_check == dist1 && (big && min2.x <= pa.first.x && pa.first.x <= max1.x || !big && min2.y <= pa.first.y && pa.first.y <= max1.y))
						  main_points.insert(pa.second);	  
						  
						else if (dist_check == dist2 && (big && min1.x <= pa.first.x && pa.first.x <= max2.x || !big && min1.y <= pa.first.y && pa.first.y <= max2.y))
						  main_points.insert(pa.second);
					}
					
					for (const auto &pa : sub_p)
					{
						if (dist_check == dist1 && (big && min2.x <= pa.first.x && pa.first.x <= max1.x || !big && min2.y <= pa.first.y && pa.first.y <= max1.y))
						  sub_points.insert(pa.second);
						  
						else if (dist_check == dist2 && (big && min1.x <= pa.first.x && pa.first.x <= max2.x || !big && min1.y <= pa.first.y && pa.first.y <= max2.y))
						  sub_points.insert(pa.second);
					}
					
					mv = dist_check == dist1 ? v1 : v2;
					min_dist = dist_check;
				}
				
				return true;
			}
			
			return false;
		}
};








#endif




