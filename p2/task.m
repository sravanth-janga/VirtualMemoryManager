CC = gcc
T1 = MultiLevelTlb.c



O1 = 041.Task1.out


runall:
	@echo "*********************************************"
	@echo "No argument is specified running the task!"
	@echo "*********************************************"
	@echo "Task  1:"
	@make --makefile=041.m $(O1)

041.Task1.out: $(T1)
	@$(CC) $^ -o $@
	@./$@


clean: 
	@rm -f *.out *.o