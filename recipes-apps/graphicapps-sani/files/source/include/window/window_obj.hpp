#ifndef __WINDOW_OBJ_H__
#define __WINDOW_OBJ_H__

#define WINDOW_SIZE_WIDTH       (600)
#define WINDOW_SIZE_HEIGHT      (300)

class graphicAppsWindow {
public:
    virtual void createWindow(void) = 0;
    virtual bool isCreated(void) = 0;
    virtual void changeSizeOfWindow(int width, int height) = 0;
    virtual void showWindow(void) = 0;
    virtual void destroyWindow(void) = 0;
};

#endif