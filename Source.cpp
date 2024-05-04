#include "tgaimage.h"
#include "model.h"

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);

void FirstAttemptLine(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color)
{
    for (float t = 0.0; t < 1.0; t += 0.1) // You change change the interval 0.01 0.001 or 0.1
    {
        int x = x0 + (x1 - x0) * t;
        int y = y0 + (y1 - y0) * t;

        image.set(x, y, color);
    }
}

void SecondAttemptLine(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color)
{
    for (int x = x0; x <= x1; x++)
    {
        float t = (x - x0) / (float)(x1 - x0);
        int y = y0 * (1.0 - t) + y1 * t; // Linear Interpolation
        image.set(x, y, color);
    }
}

void ThirdAttemptLine(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color)
{
    bool steep = false;

    // If the line is steep, we transpose the image
    if (std::abs(x0 - x1) < std::abs(y0 - y1))
    {
        std::swap(x0, y0);
        std::swap(x1, y1);
        steep = true;
    }

    // Make it left to right
    if (x0 > x1)
    {
        std::swap(x0, x1);
        std::swap(y0, y1);
    }

    for (int x = x0; x <= x1; x++)
    {
        float t = (x - x0) / (float)(x1 - x0);

        int y = y0 * (1.0 - t) + y1 * t;

        if (steep)
        {
            // If transposed, de-transpose
            image.set(y, x, color);
        }
        else
            image.set(x, y, color);
    }
}

void FourthAttemptLine(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color)
{
    bool steep = false;

    if (std::abs(x0 - x1) < std::abs(y0 - y1))
    {
        std::swap(x0, y0);
        std::swap(x1, y1);

        steep = true;
    }

    if (x0 > x1)
    {
        std::swap(x0, x1);
        std::swap(y0, y1);
    }

    int dx = x1 - x0;
    int dy = y1 - y0;

    float dError = std::abs(dy / (float)dx);
    float error = 0;

    int y = y0;

    for (int x = x0; x <= x1; x++)
    {
        if (steep)
        {
            image.set(y, x, color);
        }
        else
        {
            image.set(x, y, color);
        }

        error += dError;

        if (error > 0.5f)
        {
            y += (y1 > y0 ? 1 : -1);

            error -= 1.0f;
        }
    }
}

void FifthAttemptLine(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color)
{
	bool steep = false;

	if (std::abs(x0 - x1) < std::abs(y0 - y1))
	{
		std::swap(x0, y0);
		std::swap(x1, y1);

		steep = true;
	}

	if (x0 > x1)
	{
		std::swap(x0, x1);
		std::swap(y0, y1);
	}

	int dx = x1 - x0;
	int dy = y1 - y0;

	int dError2 = std::abs(dy) * 2;
	int error2 = 0;

	int y = y0;

	for (int x = x0; x <= x1; x++)
	{
		if (steep)
		{
			image.set(y, x, color);
		}
		else
		{
			image.set(x, y, color);
		}

		error2 += dError2;

		if (error2 > 0.5f)
		{
			y += (y1 > y0 ? 1 : -1);

			error2 -= dx * 2;
		}
	}
}


void FinalAttemptLine(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color)
{
	bool steep = false;

	if (std::abs(x0 - x1) < std::abs(y0 - y1))
	{
		std::swap(x0, y0);
		std::swap(x1, y1);

		steep = true;
	}

	if (x0 > x1)
	{
		std::swap(x0, x1);
		std::swap(y0, y1);
	}

	int dx = x1 - x0;
	int dy = y1 - y0;

	int dError2 = std::abs(dy) * 2;
	int error2 = 0;
    const int yincr = (y1 > y0 ? 1 : -1);
	int y = y0;

    if (steep)
    {
		for (int x = x0; x <= x1; x++)
		{
            image.set(y, x, color);
			error2 += dError2;
			if (error2 > 0.5f)
			{
				y += yincr;
				error2 -= dx * 2;
			}
		}
    }
    else
    {
		for (int x = x0; x <= x1; x++)
		{
			image.set(x, y, color);
			error2 += dError2;
			if (error2 > 0.5f)
			{
				y += yincr;
				error2 -= dx * 2;
			}
		}
    }
}


struct BoundingBox
{
    int x;
    int y;
    int w;
    int h;
};

struct Triangle
{
    Vec2i v0;
    Vec2i v1;
    Vec2i v2;
};


Vec3i CrossProduct(Vec3i a, Vec3i b)
{
    Vec3i result = {};

    result.x = a.y * b.z - a.z * b.y;
    result.y = a.z * b.x - a.x * b.z;
    result.z = a.x * b.y - a.y * b.x;

    return result;
}

bool IsInsideTriangle(Vec2i point, Triangle &tri)
{
	Vec2i s1 = tri.v1 - tri.v0;
	Vec2i s2 = tri.v2 - tri.v1;
	Vec2i s3 = tri.v0 - tri.v2;

	Vec2i t1 = point - tri.v0;
	Vec2i t2 = point - tri.v1;
	Vec2i t3 = point - tri.v2;

	Vec3i r1 = CrossProduct(Vec3i{ s1.x, s1.y, 0 }, Vec3i{ t1.x, t1.y, 0 });
	Vec3i r2 = CrossProduct(Vec3i{ s2.x, s2.y, 0 }, Vec3i{ t2.x, t2.y, 0 });
	Vec3i r3 = CrossProduct(Vec3i{ s3.x, s3.y, 0 }, Vec3i{ t3.x, t3.y, 0 });

	// Check if all z-components have the same sign
	return (r1.z >= 0 && r2.z >= 0 && r3.z >= 0) || (r1.z <= 0 && r2.z <= 0 && r3.z <= 0);
}

// Find the smallest axis-aligned bounding box
BoundingBox ComputeTriangleBoundingBox(Triangle &tri)
{
    float min_x = std::min({ tri.v0.x, tri.v1.x, tri.v2.x });
    float max_x = std::max({ tri.v0.x, tri.v1.x, tri.v2.x });

    float min_y = std::min({ tri.v0.y, tri.v1.y, tri.v2.y });
    float max_y = std::max({ tri.v0.y, tri.v1.y, tri.v2.y });

    float width = max_x - min_x;
    float height = max_y - min_y;

    BoundingBox result = { min_x, min_y, width, height };

    return result;
}


// taking from https://erkaman.github.io/posts/fast_triangle_rasterization.html
void TriangleFill(Triangle &tri, TGAImage &image, TGAColor color)
{
    BoundingBox bb = ComputeTriangleBoundingBox(tri);

	std::cout << "Bounding Box: x=" << bb.x << ", y=" << bb.y
		<< ", w=" << bb.w << ", h=" << bb.h << std::endl;

    FinalAttemptLine(bb.x, bb.y, bb.x + bb.w, bb.y, image, {255,0,255,255});
    FinalAttemptLine(bb.x, bb.y, bb.x, bb.y + bb.h, image, { 255,0,255,255 });
    FinalAttemptLine(bb.x + bb.w, bb.y, bb.x + bb.w, bb.y + bb.h, image, { 255,0,255,255 });
    FinalAttemptLine(bb.x, bb.y + bb.h, bb.x + bb.w, bb.y + bb.h, image, { 255,0,255,255 });

	image.set(tri.v0.x, tri.v0.y, red);
	image.set(tri.v1.x, tri.v1.y, red);
	image.set(tri.v2.x, tri.v2.y, red);

    for (int x = bb.x; x < bb.x + bb.w; x++)
    {
        for (int y = bb.y; y < bb.y + bb.h; y++)
        {
            if (IsInsideTriangle({ x,y }, tri))
            {
                image.set(x, y, color);
            }
        }
    }

	
}


void RenderWireFrame(TGAImage &image, int width, int height, TGAColor color = white)
{
    Model * model = new Model("./african_head.obj"); // 

    for (int i = 0; i < model->nfaces(); i++)
    {
        std::vector<int> face = model->face(i);

        for (int j = 0; j < 3; j++)
        {
            Vec3f v0 = model->vert(face[j]);
            Vec3f v1 = model->vert(face[(j + 1) % 3]);

            int x0 = (v0.x + 1.0) * width / 2.0f;
            int y0 = (v0.y + 1.0) * height / 2.0f;

            int x1 = (v1.x + 1.0) * width / 2.0f;
            int y1 = (v1.y + 1.0) * height / 2.0f;

            FinalAttemptLine(x0, y0, x1, y1, image, color);
        }
    }

    image.flip_vertically();
    image.write_tga_file("wireframe.tga");
}


int main(int argc, char **argv) {

    const int width = 600;
    const int height = 600;

    TGAImage image(width, height, TGAImage::RGB);
    // i want to have the origin at the left bottom corner of the image
    //image.set(52, 41, red);
    
    //for (int i = 0; i < 10; i++) image.set(0 + i, 0 + i, (i % 2 == 0) ? white : red);

#pragma region DrawLine
	////FirstAttemptLine(0, 0, 100, 100, image, white);
	//SecondAttemptLine(0, 0, 50, 50, image, red);

	//// Second Attempt Tests
	//auto LineDraw = SecondAttemptLine;

	//// Third Attempt Tests
	//LineDraw = ThirdAttemptLine;

	//// Fourth Attempt Tests
	//LineDraw = FourthAttemptLine;

	//// Fifth and Final Attempt Tests
	//LineDraw = FifthAttemptLine;

	//// Final Test
	//LineDraw = FinalAttemptLine;

	//LineDraw(13, 20, 80, 40, image, white);
	//LineDraw(20, 13, 40, 80, image, red);
	//LineDraw(80, 40, 13, 20, image, red);
#pragma endregion

   
   
    // RenderWireFrame(image, width, height, { 245, 5, 121, 255 });

    
   
    Triangle t0 = { Vec2i(10, 70),   Vec2i(50, 160),  Vec2i(70, 80) };
    Triangle t1 = { Vec2i(180, 50),  Vec2i(150, 1),   Vec2i(70, 180) };
    Triangle t2 = { Vec2i(180, 150), Vec2i(120, 160), Vec2i(130, 180) };

    TriangleFill(t0, image, red);
    TriangleFill(t1, image, white);
    TriangleFill(t2, image, { 0, 255, 0, 255 });
	image.flip_vertically();
	image.write_tga_file("triangles.tga");


    image.flip_vertically();
    image.write_tga_file("output.tga");
    
    return 0;
}