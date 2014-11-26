CC = g++
CFLAGS = -ansi -Wall

all: a.out

a.out: main.o stack.o rpn_calc.o shunting_yard.o tokenizer.o
	$(CC) $(CFLAGS) main.o stack.o rpn_calc.o shunting_yard.o tokenizer.o -o a.out

main.o: src/main.c include/rpn_calc.h include/shunting_yard.h include/tokenizer.h
	$(CC) $(CFLAGS) -c src/main.c

stack.o: src/stack.c
	$(CC) $(CFLAGS) -c src/stack.c

rpn_calc.o: src/rpn_calc.c
	$(CC) $(CFLAGS) -c src/rpn_calc.c

shunting_yard.o: src/shunting_yard.c include/stack.h
	$(CC) $(CFLAGS) -c src/shunting_yard.c

tokenizer.o: src/tokenizer.c
	$(CC) $(CFLAGS) -c src/tokenizer.c

clean:
	-@rm *.o ./a.out 2>/dev/null || true
