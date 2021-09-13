
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
            channels[3] = 1;
        }

        Color(uint8_t r, uint8_t g, uint8_t b) {
            channels[0] = r;
            channels[1] = g;
            channels[2] = b;
        }


        uint8_t r() {return channels[0];}
        uint8_t g() {return channels[1];}
        uint8_t b() {return channels[2];}

        void setr(uint8_t red) {channels[0] = red; }
        void setg(uint8_t green) {channels[1] = green; }
        void setb(uint8_t blue) {channels[2] = blue; }

        Color operator-(Color& c) {

            return Color(
                    r() - c.r(),
                    g() - c.g(),
                    b() - c.b()
                    );
        }

        bool operator==(Color& c) {

            if(r() == c.r() && g() == c.g() && b() == c.b()) {
                return true;
            } else {
                return false;
            }
        }

        Color operator+ (Color c){
            return Color{r() + c.r(), g() + c.g(), b() + c.b()};
        }

        Color operator/ (int8_t d) {
            return Color{r()/d, g()/d, b()/d};
        }

        Color operator* (int8_t d) {
            return Color{ r()*d, g()*d, b()*d };
        }

        double squareSum() {

            return r()*r() + g()*g() + b()*b();

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

        Image(int32_t width = 0, int32_t height = 0) {

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
            //free is used here rather than delete because
            // STBI uses malloc to allocate memory.
            // I chose to use the same memory allocation scheme for
            // the whole class.
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

struct Rectangle {

    int32_t x;
    int32_t y;
    int32_t width;
    int32_t height;
    Color color;
    uint8_t alpha;

    Rectangle() {}

    Rectangle(  int32_t x,
                int32_t y,
                int32_t width,
                int32_t height,
                Color color,
                int8_t alpha = 255) {
                    this->x = x;
                    this->y = y;
                    this->width = width;
                    this->height = height;
                    this->color = color;
                    this->alpha = alpha;
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

        void drawRects(Rectangle* cmds, int cmdCount) {

            for(int commandIndex = 0; commandIndex <  cmdCount; commandIndex++) {

                int32_t x = cmds[commandIndex].x;
                int32_t y = cmds[commandIndex].y;
                int32_t width = cmds[commandIndex].width;
                int32_t height = cmds[commandIndex].height;
                Color color = cmds[commandIndex].color;
                uint8_t alpha = cmds[commandIndex].alpha;

                this->drawRect(x, y, width, height, color, alpha);

            }
        }

        void drawRect(int32_t x, int32_t y, int32_t width, int32_t height, Color color, int8_t alpha = 255) {

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
            if(x + width > getWidth()) {
                clippedWidth = getWidth() - x;
            }

            if(y + height > getHeight()) {
                clippedHeight = getHeight() - y;
            }



            Color* memory = buffer->getMemoryPtr();
            for(int32_t j = 0; j < clippedHeight; j++) {

                int32_t start = (clippedY + j)*getWidth() + clippedX;

                for(int32_t i = start; i < start + clippedWidth; i++) {

                    Color dest = memory[i];

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
