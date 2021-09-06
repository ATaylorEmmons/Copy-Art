
#define STB_ONLY_PNG

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

struct Color {

    public:

        static Color random() {
            std::minstd_rand gen(std::random_device{}());
            std::uniform_int_distribution<uint8_t> runif(0, 255);

            uint8_t r = runif(gen);
            uint8_t g = runif(gen);
            uint8_t b = runif(gen);

            return Color(r, g, b);
        }

        Color() {
            channels[0] = 0;
            channels[1] = 0;
            channels[2] = 0;
        }

        Color(uint8_t r, uint8_t g, uint8_t b) {
            channels[0] = r;
            channels[1] = g;
            channels[2] = b;
        }


        uint8_t r() {return channels[0];}
        uint8_t g() {return channels[1];}
        uint8_t b() {return channels[2];}

        Color operator-(Color& c) {

            return Color(
                    r() - c.r(),
                    g() - c.g(),
                    b() - c.b()
                    );
        }

        int32_t magSq() {

            int32_t sumSQ = r()*r() + g()*g() + b()*b();

            return sumSQ;
        }

    private:
        uint8_t channels[3];
};

class Image {

    public:
        static const int channels = 3;

        Image(std::string path) {

            int channels;
            m_width = -1;
            m_height = -1;
            memory = (Color*)stbi_load(path.c_str(), &m_width, &m_height, &channels, 3);

        }

        Image(int32_t width, int32_t height) {

            this->m_width = width;
            this->m_height = height;
            size_t size = channels*sizeof(uint8_t)*width*height;

            memory = (Color*)malloc(size);
        }

        void save(std::string path) {
                    //Each pixel contains 'comp' channels of data stored interleaved with 8-bits
            //   per channel, in the following order: 1=Y, 2=YA, 3=RGB, 4=RGBA
            int stride_in_bytes = channels*sizeof(uint8_t)*m_width;
            stbi_write_png(path.c_str(),
                            m_width, m_height, 3, memory, stride_in_bytes);
        }

        ~Image() {

            free(memory);

        }

        int32_t getWidth() { return m_width; }
        int32_t getHeight() { return m_height; }

        Color* getMemoryPtr() { return memory; }

    private:
        Color* memory;
        int32_t m_width;
        int32_t m_height;

};

struct DrawRectCommand {

    int32_t x;
    int32_t y;
    int32_t width;
    int32_t height;
    Color color;

    DrawRectCommand() {}

    DrawRectCommand(int32_t x,
                    int32_t y,
                    int32_t width,
                    int32_t height,
                    Color color) {
                        this->x = x;
                        this->y = y;
                        this->width = width;
                        this->height = height;
                        this->color = color;

                    }

};


class Rasterizer {
    static const int channels = 3;

    public:
        Rasterizer() {}

        void setBuffer(Image* image) {

            this->buffer = image;

        }


        void clear(Color color) {

            Color* memory = buffer->getMemoryPtr();
            uint32_t stop = getWidth()*getHeight();

            for(uint32_t i = 0; i < stop; i++) {
                memory[i] = color;
            }
        }

        void drawRects(DrawRectCommand* cmds, int cmdCount) {

            for(int commandIndex = 0; commandIndex <  cmdCount; commandIndex++) {

                int32_t x = cmds[commandIndex].x;
                int32_t y = cmds[commandIndex].y;
                int32_t width = cmds[commandIndex].width;
                int32_t height = cmds[commandIndex].height;
                Color color = cmds[commandIndex].color;

                std::cout << x << ", " << y << std::endl;
                this->drawRect(x, y, width, height, color);

            }
        }

        void drawRect(int32_t x, int32_t y, int32_t width, int32_t height, Color color) {

            if( x + width < 0) { return; }
            if( y + height < 0) { return; }
            if( x > getWidth()) { return; }
            if( y > getHeight()) { return; }

            int32_t clippedX = x;
            int32_t clippedY = y;
            int32_t clippedWidth = width;
            int32_t clippedHeight = height;

            if(x < 0) { clippedX = 0; }
            if(y < 0) { clippedY = 0; }
            if(x + width > getWidth()) { clippedWidth = getWidth(); }
            if(y + height > getHeight()) { clippedHeight = getHeight(); }



            Color* memory = buffer->getMemoryPtr();
            for(int32_t j = 0; j < clippedHeight; j++) {

                int32_t start = (clippedY + j)*getWidth() + clippedX;

                for(int32_t i = start; i < start + clippedWidth; i++) {
                    memory[i] = color;
                }
            }


        }

        void saveImage(std::string fileName) {
            buffer->save(fileName);
        }


        int32_t getWidth() {return buffer->getWidth();}
        int32_t getHeight() {return buffer->getHeight();}

        ~Rasterizer() {

        }

    private:
        Image* buffer;
};
