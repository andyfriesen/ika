#ifndef GRAPH_H
#define GRAPH_H

namespace Video
{
    struct Exception{};

    class Image;

    class Driver
    {
    public:
        Driver(int xres, int yres, int bpp, bool fullscreen)
        {}

        template <class T>
        Image* CreateImage(const T& t) { return 0; }

        Image* CreateImage() { return 0; }

        void DrawImage(Image* i, int x, int y){}
        void ShowPage() {}

        int XRes() { return 0; }
        int YRes() { return 0; }
    };

    class Image
    {
    public:
        int getWidth() { return 0; }
        int getHeight() { return 0; }
    };
}

#endif
