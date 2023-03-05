INCLUDE = source/common.h
SOURCE = source/error.c source/generator.c source/lexer.c source/main.c source/parser.c
EXECUTABLE = bcc
TEST = test.sh

.PHONY: test clean

$(EXECUTABLE): $(INCLUDE) $(SOURCE)
	gcc -Wall -Isource $(SOURCE) -o $(EXECUTABLE)

test: $(EXECUTABLE) $(TEST)
	./$(TEST)

clean:
	rm -f $(EXECUTABLE)
