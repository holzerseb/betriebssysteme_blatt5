CFLAGS = -Wall -Werror -Wextra -Wpedantic -std=c99 -O2
POSIXFLAGS = -Wall -Werror -Wextra -Wpedantic -std=gnu99 -O2
	#had to use POSIX-Standard for the function fileno(), which is not included when used c99
CC = gcc

# Since this is the first target, it will be executed by default.
.PHONY: all
all: Task1 Task2 #Task3 Task4 #include as many as you've done


#PHONY's are just to ensure, that the rule 'clean' is used, rather then the file 'clean'
.PHONY: clean
clean:
	rm -rf Task1 Task2 Task3 Task4 pipes
	clear

#Here comes the tasks
Task1:
	$(CC) $(POSIXFLAGS) Exercise1/Task1.c Exercise1/loggerService.c -o $@

Task2:
	$(CC) $(CFLAGS) Exercise2/Task2.c -o $@

Task3:
	$(CC) $(CFLAGS) Task3.c -o $@

Task4:
	$(CC) $(CFLAGS) Task4.c -o $@
