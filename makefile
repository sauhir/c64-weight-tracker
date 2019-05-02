##############################################################################################
# C64 lotto randomizer
##############################################################################################

all:
	cl65 --standard c89 -O diary.c -o diary.prg

run:
	cl65 --standard c89 -O diary.c -o diary.prg && open diary.prg

clean:
	$(RM) *.prg *.dat *.cfg
