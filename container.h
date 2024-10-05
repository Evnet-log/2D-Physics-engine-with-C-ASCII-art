#ifndef CONTAINER_H
#define CONTAINER_H

class CONTAINER
{
	public:
		std::vector<Point*> all_points;
		std::vector<SHAPE*> all_shapes;
		
		std::vector<LINKED*> all_linkes;
		std::vector<PIN*> all_pins;
		
		Vector2f accel;
		
		float fric{0.01f};
		
		float dt;
		float elapsed_time{0.0f};
		
		int repeat;
		
		int gravity_type{0};
		int constraint_type{0};
	
	public:
		CONTAINER() = default;
		
		void set_accel(Vector2f n) { accel = n; }
		void set_dt(float n) { dt = n; }
		void set_repeat(int n) { repeat = n; }
		
		void addPoint(Vector2f pos, int radius)
		{
			all_points.push_back(new Point(pos, radius));
		}
		
		void addLink(Point *p1, Point *p2, float dist)
		{
			all_linkes.push_back(new LINKED(p1, p2, dist));
		}
		
		void addPin(SHAPE *shape, Vector2f &pos)
		{
			all_pins.push_back(new PIN(shape, pos));
		}
		
		SHAPE *addShape(int type, Vector2f pos, float radius)
		{
			std::vector<Point*> vec;
			SHAPE *shape = new SHAPE(radius);
			
			if (type == 0) // circle
			  vec.push_back(new Point(pos, radius));
			
			else // polygon
			{
				int p = 2 + type;
				float radian = (M_PI*2.0f) / static_cast<float>(p);
				
				for (int n = 0; n < p; ++n)
				{
					Vector2f v = {std::cos(radian*n), std::sin(radian*n)};
					v *= radius;
					
					vec.push_back(new Point(pos+v, 0.25f));
				}
				
				// link work
//				std::vector<VecPoint> points;
//				std::vector<EDGE> edges;
//				
//				for (const auto &po : vec)
//				  points.emplace_back(VecPoint{po->pos, po});
//				
//				edges = delaunay_triangulate(points);
				
				for (int n = 0; n < p; ++n)
				{
					for (int m = 1; m <= 2; ++m)
					{
						int index = ((n+m) < p) ? (n+m) : ((n+m) - p);
						
						Vector2f v = vec[n]->pos - vec[index]->pos;
						float dist = sqrt(v.x*v.x + v.y*v.y);
						
						addLink(vec[n], vec[index], dist);
					}
				}
				
//				for (const auto &ed : edges)
//				{
//					Vector2f v = ed.p1.pos - ed.p2.pos;
//					float dist = sqrt(v.x*v.x + v.y*v.y);
//					
//					if (ed.p1.point == nullptr || ed.p2.point == nullptr)
//					  exit(919);
//					
//					addLink(ed.p1.point, ed.p2.point, dist);
//				}
			}
			
			for (const auto &po : vec)
			{
				all_points.push_back(po);
				shape->points.push_back(po);
			}
			
			all_shapes.push_back(shape);
			
			for (int n = 0; n < repeat; ++n)
			{
				collision();
				constraint();
				point_link();
				point_pin();
			}
			
			for (auto &po : all_points)
			  po->last_pos = po->pos;
			
			return shape;
		}
		
		void reset()
		{
			for (auto &del : all_points)
			  delete del;
			for (auto &del : all_shapes)
			  delete del;
			for (auto &del : all_linkes)
			  delete del;
			for (auto &del : all_pins)
			  delete del;
			
			all_points.clear();
			all_shapes.clear();
			all_linkes.clear();
			all_pins.clear();
		}
		
		void update()
		{
			for (int n = 0; n < repeat; ++n)
			{
				gravity();
				
				collision();
				constraint();
				
				point_link();
				resistance();
				point_update();
				point_pin();
			}
			
			elapsed_time += dt;
		}
		
		void gravity()
		{
			if (gravity_type == 0)
			{
				for (const auto &po : all_points)
				  po->add_accel(accel);
			}
			
			else if (gravity_type == 1)
			{
				Vector2f origin = {40.0f, 35.0f};
				
				for (const auto &po : all_points)
				{
					Vector2f v = origin - po->pos;
					float dist = sqrt(v.x*v.x + v.y*v.y);
					
					if (dist != 0.0f)
					{
						v *= 1.0f / dist;
						v *= 70.0f;
						
						po->add_accel(v);
					}
				}
			}
		}
		
		void point_update()
		{
			for (const auto &po : all_points)
			  po->update(dt/static_cast<float>(repeat));
		}
		
		void constraint()
		{
			if (constraint_type == 0)
			{
				Vector2f big_pos{40.0f, 35.0f};
				float big_radi = 35.0f;
				
				for (const auto &po : all_points)
				{
					Vector2f v = big_pos - po->pos;
					
					float max_dist = big_radi - po->radius;
					float dist = sqrt(v.x*v.x + v.y*v.y);
					
					if (dist > max_dist)
					{
						v *= 1.0f / dist;
						v *= dist - max_dist;
						
						po->pos += v;
					}
				}
			}
			
			else if (constraint_type >= 1)
			{
				for (const auto &po : all_points)
				{
					if (po->pos.x - po->radius < 0.0f)
					  po->pos.x = 0.0f + po->radius;
					if (po->pos.x + po->radius > (MAX_X/2)-1)
					  po->pos.x = (MAX_X/2)-1 - po->radius;
					  
					if (po->pos.y + po->radius > MAX_Y-1)
					{
						po->pos.y = MAX_Y-1 - po->radius;
						//po->last_pos.x += (po->pos.x - po->last_pos.x)*0.1f;
					}
				}
				
				if (constraint_type == 2)
				{
					for (const auto &po : all_points)
					{
						float e = 1.0f / 2.5f;
						
						float val1 = 90;
						float val2 = po->pos.y - po->pos.x*2;
						
						Vector2f v = {e * (val1 - val2), e * (2 * val1 + 0.5f * val2)};
						
						Vector2f d = v - po->pos;
						float dist = sqrt(d.x*d.x + d.y*d.y);
						
						d *= 1.0f / dist;
						d *= po->radius;
						
						Vector2f check_v[2] = {po->pos + d, po->pos - d};
						
						if (check_v[0].x < v.x && v.x < check_v[1].x || check_v[1].x < v.x && v.x < check_v[0].x)
						{
							Vector2f c1 = v - check_v[0];
							Vector2f c2 = v - check_v[1];
							
							float d1 = c1.x*c1.x + c1.y*c1.y;
							float d2 = c2.x*c2.x + c2.y*c2.y;
							
							if (d1 < d2)
							  po->pos += c1;
							else
							  po->pos += c2;
						}
					}
				}
			}
		}
		
		void collision()
		{
			SAT_COLLISION sat;
			size_t size = all_shapes.size();
			
			float coll_val = 0.0f;
			
			for (int n = 0; n < size; ++n)
			{
				for (int m = n+1; m < size; ++m)
				{
					coll_val = 0.0f;
					
					sat.reset();
					
					if (all_shapes[n]->points.size() == 1 && all_shapes[m]->points.size() == 1) // c to c
					  coll_val = circle_collision(all_shapes[n]->points[0], all_shapes[m]->points[0]);
					
					else // p to p || p to c || c to p
					{
						all_shapes[n]->update();
						all_shapes[m]->update();
						
						Vector2f v = all_shapes[n]->origin - all_shapes[m]->origin;
						float dist = sqrt(v.x*v.x + v.y*v.y);
						float min_dist = all_shapes[n]->radius + all_shapes[m]->radius;
						
						if (dist < min_dist)
						  coll_val = sat.check_collision(*(all_shapes[n]), *(all_shapes[m]));
					}
					
					// friction force
					if (coll_val != 0.0f)
					{
						for (const auto &po : all_shapes[n]->points)
						{
							Vector2f v = po->pos - po->last_pos;
							v *= fric * coll_val;
							
							po->last_pos += v;
						}
						
						for (const auto &po : all_shapes[m]->points)
						{
							Vector2f v = po->pos - po->last_pos;
							v *= fric * coll_val;
							
							po->last_pos += v;
						}
					}
				}
			}
		}
		
		float circle_collision(Point *circle1, Point *circle2)
		{
			Vector2f v = circle2->pos - circle1->pos;
			
			float min_dist = circle2->radius + circle1->radius;
			float dist = sqrt(v.x*v.x + v.y*v.y);
			
			if (dist != 0.0f && dist < min_dist) // overlap
			{
				float d = min_dist - dist;
				
				float ratio1 = circle1->radius/min_dist;
				float ratio2 = circle2->radius/min_dist;
				
				v *= 1.0f / dist;
				v *= d * 0.2f;
				
				circle1->pos -= v*ratio2;
				circle2->pos += v*ratio1;
				
				return d;
			}
			
			return 0.0f;
		}
		
		void point_link()
		{
			for (const auto &link : all_linkes)
			{
				Vector2f v = link->p2->pos - link->p1->pos;
				float dist = sqrt(v.x*v.x + v.y*v.y);
				
				float dist_ratio = (dist - link->dist) / link->dist;
				
				v *= dist_ratio * 2000.0f;
				
//				float sx1 = 0.0f, sy1 = 0.0f;
//				float sx2 = 0.0f, sy2 = 0.0f;
//				
//				if (abs(v.x) > 0.0001f)
//				{
//					sx1 = v.x - (link->p1->pos.x - link->p1->last_pos.x) / dt * 0.5;
//					sx2 = -v.x - (link->p2->pos.x - link->p2->last_pos.x) / dt * 0.5;
//				}
//				
//				if (abs(v.y) > 0.0001f)
//				{
//					sy1 = v.y - (link->p1->pos.y - link->p1->last_pos.y) / dt * 0.5;
//					sy2 = -v.y - (link->p2->pos.y - link->p2->last_pos.y) / dt * 0.5;
//				}
				
				link->p1->add_accel(v);
				link->p2->add_accel(v * -1.0f);
			}
		}
		
		void resistance()
		{
			for (auto &po : all_points)
			{
				Vector2f v = (po->pos - po->last_pos) * (1.0f / dt);
				v *= -0.7;
				
				po->add_accel(v);
			}
		}
		
		void point_pin()
		{ 
			for (const auto &pin : all_pins)
			{
				if (pin->shape->points.size() > 1)
				{
					for (const auto &po : pin->shape->points)
					{
						Vector2f v = pin->pos - po->pos;
						float dist = sqrt(v.x*v.x + v.y*v.y);
						
						float check = dist - pin->shape->radius;
						
						if (check != 0.0f)
						{
							v *= 1.0f / dist;
							v *= check;
							
							po->pos += v;
						}
					}
				}
				
				else
				  pin->shape->points[0]->pos = pin->pos;
			}
		}
		
		void change_gravity()
		{
			if (gravity_type < 1)
			  ++gravity_type;
			else
			  gravity_type = 0;
		}
		
		void change_constraint()
		{
			if (constraint_type < 3)
			  ++constraint_type;
			else
			  constraint_type = 0;
		}
};





#endif
