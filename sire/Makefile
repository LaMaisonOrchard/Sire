


SHELL = /bin/sh

OBJ = adrbuf.o     env.o      jlex.o       rule.o    target.o \
      adrstack.o   exit.o     lex.o        set.o     targetstmplst.o \
      call.o       filedat.o  list.o       state.o   text.o \
      charstack.o  filestk.o  machine.o    strbuf.o  tokenstk.o \
      dep.o        help.o     main.o       str.o     toklist.o \
      deplist.o    jake.o     redundant.o  sys.o     msh.o  \
      mlex.o       conditional.o


.cpp.o:
	$(GCC) $< -c -g

sire: $(OBJ)
	$(GCC) -o sire $(OBJ)

