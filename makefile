##############################################################################################
# C64 lotto randomizer
##############################################################################################

all:
	cl65 --standard c89 -Osir wtrack64.c -o wtrack64.prg

run:
	cl65 --standard c89 -Osir wtrack64.c -o wtrack64.prg && open wtrack64.prg

d64:
	cl65 --standard c89 -Osir wtrack64.c -o wtrack64.prg
	# c1541 -format wtrack,id d64 wtrack.d64 -attach wtrack.d64 -write wtrack64.prg wtrack64
	c1541 -attach wtrack.d64 -delete wtrack64 -write wtrack64.prg wtrack64

clean:
	$(RM) *.prg *.dat *.cfg
