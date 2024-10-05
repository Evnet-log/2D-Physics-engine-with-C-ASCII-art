#ifndef THING_H
#define THING_H

#define MAX_X 160
#define MAX_Y 70

void c()
{
  CONSOLE_CURSOR_INFO cursorInfo = { 0, };

  cursorInfo.dwSize = 1;

  cursorInfo.bVisible = FALSE;

  SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
}

void gotoxy(int x, int y)
{

    COORD Pos;
    
    Pos.X = x-1;
    Pos.Y = y-1;

    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), Pos);

}

void setColor(int color, int bgcolor)
{
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), ((bgcolor&0xf)<<4) | (color&0xf));
}

enum ConsolColor{
    BLACK,
    DARKBLUE,
    DARKGREEN,
    DARKCYAN,
    DARKRED,
    DARKMAGENTA,
    DARKYELLOW,
    GRAY,
    DARKGRAY,
    BLUE,
    GREEN,
    CYAN,
    RED,
    MAGENTA,
    YELLOW,
    WHITE,
    LIGHTGRAY = 7,
    ORIGINAL = 7,
    ORIGINALFONT = 7,
    ORIGINALBG = 0
};

struct Vector2f
{
	public:
		float x;
		float y;
	
	public:
		Vector2f() = default;
		Vector2f(float x, float y) :x(x), y(y) {}
		Vector2f(const Vector2f &v)
		{
			x = v.x;
			y = v.y;
		}
		
		Vector2f& operator+=(const Vector2f &target) {
			x += target.x;
			y += target.y;
			
			return *this;
		}
		
		Vector2f& operator-=(const Vector2f &target) {
			x -= target.x;
			y -= target.y;
			
			return *this;
		}
		
		Vector2f& operator*=(float val) {
			x *= val;
			y *= val;
			
			return *this;
		}
		
		Vector2f operator+(const Vector2f &target) const {
			return {x + target.x, y + target.y};
		}
		
		Vector2f operator-(const Vector2f &target) const {
			return {x - target.x, y - target.y};
		}
		
		Vector2f operator*(float val) const {
			return {x * val, y * val};
		}
		
		bool operator==(const Vector2f &target) const {
			return x == target.x && y == target.y;
		}
};

struct COOR
{
	public:
		int x;
		int y;
	
	public:
		COOR() = default;
		COOR(int x, int y) :x(x), y(y) {}
		COOR(float x, float y) { this->x = (int)round(x), this->y = (int)round(y); }
		COOR(Vector2f &v)  { x = (int)round(v.x), y = (int)round(v.y); }
		
		bool operator==(const COOR &target) const {
			return x == target.x && y == target.y;
		}
};

std::vector<COOR> bresenham_line(COOR a, COOR b)
{
	int mx = a.x < b.x ? 1 : -1;
	int my = a.y < b.y ? 1 : -1;
	
	int width = abs(b.x - a.x);
	int height = abs(b.y - a.y);
	
	int val = width - height;
	
	std::vector<COOR> coors;
	
	while (1)
	{
		coors.emplace_back(a);
		
		if (a == b)
		  break;
		
		int ex = 2*val;
		
		if (ex > -height)
		{
			a.x += mx;
			val -= height;
		}
		
		if (ex < width)
		{
			a.y += my;
			val += width;
		}
	}
	
	return coors;
}

struct Point
{
	public:
		Vector2f pos{0.0f, 0.0f};
		Vector2f last_pos{0.0f, 0.0f};
		
		Vector2f accel{0.0f, 0.0f};
		
		float radius{0.0f};
		
	
	public:
		Point() = default;
		Point(Vector2f pos, float radius) :pos(pos), last_pos(pos), radius(radius) {}
		
		void update(float dt)
		{
			Vector2f new_pos = pos*2.0f - last_pos + accel * dt * dt;
			
			last_pos = pos;
			pos = new_pos;
			
			accel = {0.0f, 0.0f};
		}
		
		void add_vel(Vector2f vel)
		{
			last_pos -= vel;
		}
		
		void add_accel(Vector2f vel)
		{
			accel += vel;
		}
};

struct LINKED
{
	public:
		Point *p1;
		Point *p2;
		
		float dist;
	
	public:
		LINKED() = default;
		LINKED(Point *p1, Point *p2, float dist) :p1(p1), p2(p2), dist(dist) {}
};

struct SHAPE
{
	public:
		std::vector<Point*> points;
		
		Vector2f max;
		Vector2f min;
		
		Vector2f origin;
		
		float radius;
	
	public:
		SHAPE() = default;
		SHAPE(float radius) :radius(radius) {}
		
		void update()
		{
			if (points.size() > 1)
			{
				max.x = std::numeric_limits<float>::lowest();
				max.y = std::numeric_limits<float>::lowest();
				
				min.x = std::numeric_limits<float>::max();
				min.y = std::numeric_limits<float>::max();
				
				origin = {0.0f, 0.0f};
				
				for (const auto &po : points)
				{
					max.x = std::max(max.x, po->pos.x);
					max.y = std::max(max.y, po->pos.y);
					
					min.x = std::min(min.x, po->pos.x);
					min.y = std::min(min.y, po->pos.y);
					
					origin += po->pos;
				}
				
				origin *= 1.0f / static_cast<float>(points.size());
			}
			
			else
			{
				max.x = points[0]->pos.x + points[0]->radius;
				max.y = points[0]->pos.y + points[0]->radius;
				min.x = points[0]->pos.x - points[0]->radius;
				min.y = points[0]->pos.y - points[0]->radius;
				
				origin = points[0]->pos;
			}
		}
};

struct PIN
{
	public:
		SHAPE *shape;
		Vector2f pos;
	
	public:
		PIN() = default;
		PIN(SHAPE *shape, Vector2f &pos) :shape(shape), pos(pos) {}
};

#endif






