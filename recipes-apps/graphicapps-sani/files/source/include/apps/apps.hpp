#ifndef __GRAPHIC_APPS_H__
#define __GRAPHIC_APPS_H__

class Apps {
public:
    virtual void init(void) = 0;
    virtual void run(void) = 0;
    virtual void cleanup(void) = 0;
};

#endif
