build: mybash.c mypwd.c mycd.c myls.c mymkdir.c myrm.c mytail.c myps.c mymv.c myterminal.c utility.c
	@echo
	@echo Compiling all source codes...
	@gcc mypwd.c -o mypwd -g
	@gcc utility.c myterminal.c mybash.c -o mybash -g
	@gcc mycd.c -o mycd -g
	@gcc myls.c -o myls -g
	@gcc mymkdir.c -o mymkdir -g
	@gcc utility.c myrm.c -o myrm -g
	@gcc utility.c mytail.c -o mytail -g
	@gcc utility.c mymv.c -o mymv -g
	@gcc utility.c myps.c -o myps -g
	@echo
	@echo Comilation finished...
	@echo

run:
	@./mybash

debug:
	@gdb mybash

prep-doxy:
	doxygen doxy.config
	firefox html/index.html &
