##############################################################################################
# Weight Tracker 64
##############################################################################################

MKDIR_P = mkdir -p
BIN_DIR = bin
SRC_DIR = src

SRC = $(SRC_DIR)/menu.c \
      $(SRC_DIR)/entry.c \
      $(SRC_DIR)/input.c \
      $(SRC_DIR)/date.c  \
      $(SRC_DIR)/files.c \
      $(SRC_DIR)/tokens.c \
      $(SRC_DIR)/config.c \
      $(SRC_DIR)/main.c

.PHONY: directories

all: directories

directories: $(BIN_DIR) $(SRC_DIR)

$(BIN_DIR):
	$(MKDIR_P) $(BIN_DIR)

$(SRC_DIR):
	$(MKDIR_P) $(SRC_DIR)

wtrack64: directories
	cl65 --standard c89 -Osir $(SRC) -o $(BIN_DIR)/wtrack64.prg

run: directories
	cl65 --standard c89 -Osir $(SRC) -o $(BIN_DIR)/wtrack64.prg && \
	x64 $(BIN_DIR)/wtrack64.d64

d64: wtrack64
	if ! [ -f $(BIN_DIR)/wtrack.d64 ]; then \
		c1541 -format wtrack,id d64 $(BIN_DIR)/wtrack.d64; \
	fi
	c1541 -attach $(BIN_DIR)/wtrack.d64 \
	      -delete wtrack64 \
	      -write $(BIN_DIR)/wtrack64.prg wtrack64

clean:
	$(RM) $(SRC_DIR)/*.o $(BIN_DIR)/*.prg $(BIN_DIR)/*.d64 $(BIN_DIR)/*.dat $(BIN_DIR)/*.cfg
