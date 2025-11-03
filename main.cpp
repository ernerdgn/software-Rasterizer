#include <iostream>
#include "Image.h"
#include "Color.h"
#include "Vec.h"

int main()
{
    const int width = 800;
    const int height = 600;

    Image my_image(width, height); // img obj

    // test 1, simple shallow line
	my_image.drawLine(20, 20, 300, 80, white);

	// test 2, steep line
	my_image.drawLine(50, 50, 100, 300, red);

	// test 3, reverse line
	my_image.drawLine(400, 100, 200, 120, green);

	// test 4, steep line going backwards and down
	my_image.drawLine(350, 400, 300, 100, blue);

	// test 5, star burst lines from center
    int center_x = width / 2;
    int center_y = height / 2;

    for (int i = 0; i < 360; i += 10)
    {
        double angle_rad = i * 3.14159f / 180.0f; // deg to rad
        int x1 = static_cast<int>(center_x + 200 * std::cos(angle_rad));
        int y1 = static_cast<int>(center_y + 200 * std::sin(angle_rad));
        my_image.drawLine(center_x, center_y, x1, y1, white);
	}

    // save
    const std::string filename = "output.tga";
    if (my_image.write_tga_file(filename)) std::cout << "image saved to " << filename << std::endl;
    else std::cerr << "error: cant save the image" << std::endl;

    return 0;
}