#include <iostream>
#include "Image.h"
#include "Color.h"

int main()
{
    const int width = 800;
    const int height = 600;

    Image my_image(width, height); // img obj

    // single red pixel on the mid
    int x = width / 2;
    int y = height / 2;
    my_image.set_pixel(x, y, red);

    // draw 10x10 white square
    for (int i = 0; i < 10; i++)
    {
        for (int j = 0; j < 10; j++)
        {
            my_image.set_pixel(100+i, 100+j, white);
        }
    }

    // save
    const std::string filename = "output.tga";
    if (my_image.write_tga_file(filename)) std::cout << "image saved to " << filename << std::endl;
    else std::cerr << "error: cant save the image" << std::endl;

    return 0;
}