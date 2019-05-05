##############################################################################################
# Weight Tracker 64
##############################################################################################

MKDIR_P = mkdir -p
BIN_DIR = bin
SRC_DIR = src

.PHONY: directories

all: directories wtrack64

directories: $(BIN_DIR) $(SRC_DIR)

$(BIN_DIR):
	$(MKDIR_P) $(BIN_DIR)

$(SRC_DIR):
	$(MKDIR_P) $(SRC_DIR)

wtrack64:
	cl65 --standard c89 -Osir $(SRC_DIR)/entry.c $(SRC_DIR)/input.c $(SRC_DIR)/date.c $(SRC_DIR)/files.c $(SRC_DIR)/tokens.c $(SRC_DIR)/config.c $(SRC_DIR)/main.c -o $(BIN_DIR)/wtrack64.prg

run: directories
	cl65 --standard c89 -Osir $(SRC_DIR)/entry.c $(SRC_DIR)/input.c $(SRC_DIR)/date.c $(SRC_DIR)/files.c $(SRC_DIR)/tokens.c $(SRC_DIR)/config.c $(SRC_DIR)/main.c -o $(BIN_DIR)/wtrack64.prg && open $(BIN_DIR)/wtrack64.prg

d64: directories
	cl65 --standard c89 -Osir $(SRC_DIR)/entry.c $(SRC_DIR)/input.c $(SRC_DIR)/date.c $(SRC_DIR)/files.c $(SRC_DIR)/tokens.c $(SRC_DIR)/config.c $(SRC_DIR)/main.c -o $(BIN_DIR)/wtrack64.prg
	# c1541 -format wtrack,id d64 wtrack.d64 -attach wtrack.d64 -write wtrack64.prg wtrack64
	c1541 -attach $(BIN_DIR)/wtrack.d64 -delete wtrack64 -write $(BIN_DIR)/wtrack64.prg wtrack64

clean:
	$(RM) $(SRC_DIR)/*.o $(BIN_DIR)/*.prg $(BIN_DIR)/*.dat $(BIN_DIR)/*.cfg
