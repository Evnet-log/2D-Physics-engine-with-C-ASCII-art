#ifndef DELAUNAY_H
#define DELAUNAY_H

struct VecPoint
{
	public:
		Vector2f pos;
		Point *point;
	
	public:
		VecPoint() = default;
		VecPoint(float x, float y) :point(nullptr) {pos.x = x, pos.y = y;}
		VecPoint(Vector2f pos, Point *point=nullptr) :pos(pos), point(point) {}
		
		bool operator==(const VecPoint &target) const {
			return pos == target.pos;
		}
};

struct EDGE
{
	public:
		VecPoint p1;
		VecPoint p2;
	
	public:
		EDGE() = default;
		EDGE(const EDGE &edge) {p1 = edge.p1, p2 = edge.p2;}
		EDGE(VecPoint p1, VecPoint p2) :p1(p1), p2(p2) {}
		
		bool operator==(const EDGE &ed) const {
			return p1 == ed.p1 && p2 == ed.p2 ||
			       p2 == ed.p1 && p1 == ed.p2;
		}
};

struct TRIANGLE
{
	public:
		VecPoint po1, po2, po3;
		EDGE e1, e2, e3;
		
		Vector2f circle;
		float radius;
	
	public:
		TRIANGLE() = default;
		TRIANGLE(VecPoint po1, VecPoint po2, VecPoint po3)
		  :po1(po1), po2(po2), po3(po3)
		{
			e1 = {po1, po2};
			e2 = {po2, po3};
			e3 = {po3, po1};
			
			Vector2f p1 = po1.pos;
			Vector2f p2 = po2.pos;
			Vector2f p3 = po3.pos;
			
			float ax = p2.x - p1.x;
			float ay = p2.y - p1.y;
			
			float bx = p3.x - p1.x;
			float by = p3.y - p1.y;
			
			float e = 1.0f / (ax*by - ay*bx);
			
			float v1 = (p2.x*p2.x - p1.x*p1.x + p2.y*p2.y - p1.y*p1.y) / 2.0f;
			float v2 = (p3.x*p3.x - p1.x*p1.x + p3.y*p3.y - p1.y*p1.y) / 2.0f;
			
			circle.x = e * (by*v1 + (-ay)*v2);
			circle.y = e * ((-bx)*v1 + (ax)*v2);
			
			Vector2f v = circle - p1;
			radius = sqrt(v.x*v.x + v.y*v.y);
		}
};

void remove_same_edge(std::vector<EDGE> &all_edge, bool all=false)
{
	size_t size = all_edge.size();
	
	std::vector<bool> del(size, false);
	
	for (int n = 0; n < size; ++n)
	{
		for (int m = n+1; m < size; ++m)
		{
			if (all_edge[n] == all_edge[m])
			{
				del[m] = true;
				
				if (all)
				  del[n] = true;
			}
		}
	}
	
	all_edge.erase(std::remove_if(all_edge.begin(), all_edge.end(), [&](const EDGE &ed){
		return del[&ed - &all_edge[0]];
	}), all_edge.end());
}

std::vector<EDGE> delaunay_triangulate(std::vector<VecPoint> &points)
{
	Vector2f max{std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest()};
	Vector2f min{std::numeric_limits<float>::max(), std::numeric_limits<float>::max()};
	
	for (const auto &point : points)
	{
		max.x = std::max(max.x, point.pos.x);
		max.y = std::max(max.y, point.pos.y);
		
		min.x = std::min(min.x, point.pos.x);
		min.y = std::min(min.y, point.pos.y);
	}
	
	float width = std::abs(max.x - min.x);
	float height = std::abs(max.y - min.y);
	
	float big = std::max(width, height);
	
	VecPoint bp1{min.x - big*20, min.y - big};
	VecPoint bp2{max.x + big*20, min.y - big};
	VecPoint bp3{max.x, max.y + big*20};
	
	std::vector<TRIANGLE> triangles;
	triangles.emplace_back(TRIANGLE{bp1, bp2, bp3});
	
	for (int n = 0; n < points.size(); ++n)
	{
		std::vector<EDGE> all_edge;
		std::vector<TRIANGLE> all_tri;
		
		for (const auto &tri : triangles)
		{
			Vector2f v = tri.circle - points[n].pos;
			float dist = sqrt(v.x*v.x + v.y*v.y);
			
			if (dist < tri.radius + 4e-4) // in circle
			{
				all_edge.push_back(tri.e1);
				all_edge.push_back(tri.e2);
				all_edge.push_back(tri.e3);
			}
			
			else
			  all_tri.push_back(tri);
		}
		
		remove_same_edge(all_edge, true);
		
		for (const auto &ed : all_edge)
		  all_tri.emplace_back(TRIANGLE{ed.p1, ed.p2, points[n]});
		
		triangles = all_tri;
	}
	
	triangles.erase(std::remove_if(triangles.begin(), triangles.end(), [&](const TRIANGLE &tri){
		return tri.po1 == bp1 || tri.po1 == bp2 || tri.po1 == bp3 ||
		       tri.po2 == bp1 || tri.po2 == bp2 || tri.po2 == bp3 ||
		       tri.po3 == bp1 || tri.po3 == bp2 || tri.po3 == bp3;
	}), triangles.end());
	
	std::vector<EDGE> edges;
	
	for (const auto &tri : triangles)
	{
		edges.push_back(tri.e1);
		edges.push_back(tri.e2);
		edges.push_back(tri.e3);
	}
	
	remove_same_edge(edges);
	
	return edges;
}



#endif







