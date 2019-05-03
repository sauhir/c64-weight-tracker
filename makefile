##############################################################################################
# C64 lotto randomizer
##############################################################################################

all:
	cl65 --standard c89 -Osir wtrack64.c -o wtrack64.prg

run:
	cl65 --standard c89 -Osir wtrack64.c -o wtrack64.prg && open wtrack64.prg

clean:
	$(RM) *.prg *.dat *.cfg
