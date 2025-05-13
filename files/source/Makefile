TARGET = graphicapps-sani

SRCS = $(shell find . -type f -name "*.cpp")
HEADS = $(shell find ./include -type f -name "*.hpp")
OBJS = $(SRCS:.cpp=.o)
DEPS = Makefile.depend

INCLUDES += -I./include -I$(OECORE_TARGET_SYSROOT)/usr/include
CFLAGS = -std=c++17 -O2
LDFLAGS = -lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi -lm

all: $(TARGET)

$(TARGET): $(OBJS) $(HEADS)
	$(CXX) $(LDFLAGS) -o $@ $(OBJS)

run: all
	@./$(TARGET)

.PHONY: depend clean
depend:
	$(CXX) $(CCFLAGS) -MM $(SRCS) > $(DEPS)
	@sed -i -E "s/^(.+?).o: ([^ ]+?)\1/\2\1.o: \2\1/g" $(DEPS)

clean:
	$(RM) $(OBJS) $(TARGET)

-include $(DEPS)
