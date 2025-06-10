TARGET=graphicapps-sani
SRCS += $(shell find ./* -type f -name "*.c")
HEADS += $(shell find ./include -type f -name "*.h")

OBJS = $(SRCS:.c=.o)
DEPS = Makefile.depend

CFLAGS ?=
LFLAGS ?=

CXXFLAGS += $(CFLAGS)
LFLAGS += $(LDFLAGS)

all: $(TARGET)

$(TARGET): $(OBJS) $(HEADS)
	$(CC) $(CXXFLAGS) $(LFLAGS) $(OBJS) -o $@

%.o: %.c
	$(CC) $(CXXFLAGS) -c -o $@ $<

run: all
	@./$(TARGET)

.PHONY: depend clean
depend:
	$(CC) $(CXXFLAGS) -MM $(SRCS) > $(DEPS)
	@sed -i -E "s/^(.+?).o: ([^ ]+?)\1/\2\1.o: \2\1/g" $(DEPS)

clean:
	$(RM) $(OBJS) $(TARGET)

-include $(DEPS)
