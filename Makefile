TARGET = c.exe
.PHONY: all clean run

SOURCES := src/main.c \
	src/tokenize.c \
	src/parse.c \
	src/codegen.c \

all: $(TARGET)

$(TARGET):
	clang -Wno-deprecated-declarations $(SOURCES) -o $(TARGET)

run: $(TARGET)
	$(TARGET) test.txt

clean:
	rm -f *.exe
	rm -f *.pdb 
	rm -f *.ilk
	rm -f *.asm
	rm -f *.o