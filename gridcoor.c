// Two-dimensional position or direction vector
typedef struct vec2 {
    int x, y;
} Vec2;

// Signum (or sign) for int
int sgn(const int x)
{
    return x > 0 ? 1 : (x < 0 ? -1 : 0);
}

// Rotate vector in 4 directions on 2D grid
// Computer graphics: rotate left, mathematically: rotate right
// (1,0) -> (0,-1) -> (-1,0) -> (0,1)
Vec2 left4(const Vec2 a)
{
    return (Vec2){a.y, -a.x};
}

// Rotate vector in 4 directions on 2D grid
// Computer graphics: rotate right, mathematically: rotate left
// (1,0) -> (0,1) -> (-1,0) -> (0,-1)
Vec2 right4(const Vec2 a)
{
    return (Vec2){-a.y, a.x};
}

// Rotate unit vector in 8 directions on 2D square grid
// Computer graphics: rotate left, mathematically: rotate right
// (1,0) -> (1,-1) -> (0,-1) -> (-1,-1) -> (-1,0) -> (-1,1) -> (0,1) -> (1,1)
Vec2 left8(const Vec2 a)
{
    return (Vec2){sgn(a.x + a.y), sgn(a.y - a.x)};
}

// Rotate unit vector in 8 directions on 2D square grid
// Computer graphics: rotate right, mathematically: rotate left
// (1,0) -> (1,1) -> (0,1) -> (-1,1) -> (-1,0) -> (-1,-1) -> (0,-1) -> (1,-1)
Vec2 right8(const Vec2 a)
{
    return (Vec2){sgn(a.x - a.y), sgn(a.x + a.y)};
}
