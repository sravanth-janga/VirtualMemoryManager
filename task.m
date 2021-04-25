CC = gcc
T1 = SimplePageTable.c 
T2 = PageTablewithTlb.c


O1 = 041.Task1.out
O2 = 041.Task2.out

runall:
	@echo "*********************************************"
	@echo "No argument is specified running both tasks!"
	@echo "*********************************************"
	@echo "Task  1:"
	@make --makefile=041.m $(O1)
	@echo "Task  2:"
	@make --makefile=041.m $(O2)

041.Task1.out: $(T1)
	@$(CC) $^ -o $@
	@./$@

041.Task2.out: $(T2)
	@$(CC) $^ -o $@
	@./$@

clean: 
	@rm -f *.out *.o