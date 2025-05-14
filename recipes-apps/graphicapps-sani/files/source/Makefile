TARGET = graphicapps-sani
SRCS = $(shell find . -type f -name "*.cpp")
HEADS = $(shell find ./include -type f -name "*.hpp")
OBJS = $(SRCS:.cpp=.o)
DEPS = Makefile.depend

CPPFLAGS ?=
LDFLAGS ?=
INCLUDE ?=

CXXFLAGS += $(CPPFLAGS)

all: $(TARGET)

$(TARGET): $(OBJS) $(HEADS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS) $(LDFLAGS)

run: all
	@./$(TARGET)

.PHONY: depend clean
depend:
	$(CXX) $(CXXFLAGS) -MM $(SRCS) > $(DEPS)
	@sed -i -E "s/^(.+?).o: ([^ ]+?)\1/\2\1.o: \2\1/g" $(DEPS)

clean:
	$(RM) $(OBJS) $(TARGET)

-include $(DEPS)
