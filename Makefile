BUILD = build
EXEC = torflix

default: $(BUILD)/$(EXEC)

SRC_FILES = $(wildcard src/*.cpp)
SRC := src
CPP := g++
CC  := gcc
CPP_FLAGS += -I vendor/crow/include
CPP_FLAGS += -I vendor/curl/include
CPP_FLAGS += -I vendor/rapidjson/include
CPP_FLAGS += -I vendor/sqlite
CPP_FLAGS += -I vendor/sqlite3pp/headeronly_src
CPP_FLAGS += $(shell pkg-config --cflags libtorrent-rasterbar)
LD_FLAGS += -L vendor/curl/lib -lcurl
LD_FLAGS += $(shell pkg-config --libs libtorrent-rasterbar)

$(BUILD):
	@mkdir -p $@

OBJ_FILES = $(patsubst src/%.cpp, $(BUILD)/%.o, $(SRC_FILES))
$(BUILD)/%.o: $(SRC)/%.cpp | $(BUILD)
	$(CPP) -o $@ -c $(CPP_FLAGS) $<

OBJ_FILES += $(BUILD)/sqlite3.o
$(BUILD)/sqlite3.o: vendor/sqlite/sqlite3.c
	$(CC) -o $@ -c $<

$(BUILD)/$(EXEC): $(OBJ_FILES) | $(BUILD)
	$(CPP) -o $@ $(CPP_FLAGS) $^ $(LD_FLAGS)

host: $(BUILD)/$(EXEC)
	$(BUILD)/$(EXEC)
