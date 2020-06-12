SRCS = main.c
OBJS = main.o
EXEC = main

$(EXEC) : $(OBJS)
	cc -o $(EXEC) $(OBJS)

$(OBJS) : $(SRCS)
	cc -c $(SRCS)

clean :
	rm -f $(OBJS)

fclean : clean
	rm -f $(EXEC)

re : fclean $(EXEC)

run : re
	./$(EXEC) > output