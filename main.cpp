#include <iostream>
#include <random>

#include "Rasterizer.h"
#include "utils.h"


uint32_t fitness(Image& image, Image& target) {

    uint32_t ret = 0;

    Color* imagePixels = image.getMemoryPtr();
    Color* targetPixels = target.getMemoryPtr();

    for(int32_t i = 0; i < image.getHeight(); i++) {
        for(int32_t j = 0; j < image.getWidth(); j++) {
            int32_t index = i*image.getWidth() + j;

            ret += (imagePixels[index].magSq() - targetPixels[index].magSq());

        }
    }


    return ret;
}


int main()
{
    Timer timer;

    Color c(10, 123, 0);
    Color c2(204, 124, 234);

    Image target("ring.png");
    Image buffer("ring.png");

    Rasterizer canvas;
    canvas.setBuffer(&buffer);


    uint32_t drawCount = 6*4;

    timer.start();

    DrawRectCommand cmds[drawCount];

    for(int32_t i = 0; i < drawCount; i++) {


        Color color = Color::random();
        //cmds[i] = DrawRectCommand(x, y, width, height, color);
        canvas.drawRect(100*(i % 6), 100*(i / 6), 100, 100, color);

    }

    //canvas.drawRects(cmds, drawCount);
    std::cout << fitness(buffer, target) << std::endl;
    std::cout << timer.stop() << " ms" << std::endl;

    canvas.saveImage("canvas_out.png");

    return 0;
}
