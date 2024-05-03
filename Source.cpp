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


void RenderWireFrame(TGAImage &image, int width, int height, TGAColor color = white)
{
    Model * model = new Model("./african_head.obj");

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


    

    

    image.flip_vertically();
   
    RenderWireFrame(image, width, height, { 245, 5, 121, 255 });

    image.write_tga_file("output.tga");
    
    return 0;
}