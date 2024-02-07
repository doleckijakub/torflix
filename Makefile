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
CPP_FLAGS += -I vendor/sqlitecpp/include
CPP_FLAGS += $(shell pkg-config --cflags libtorrent-rasterbar)
LD_FLAGS += -L vendor/curl/lib -lcurl
LD_FLAGS += $(shell pkg-config --libs libtorrent-rasterbar)

$(BUILD):
	@mkdir -p $@

OBJ_FILES = $(patsubst src/%.cpp, $(BUILD)/%.o, $(SRC_FILES))
$(BUILD)/%.o: $(SRC)/%.cpp | $(BUILD)
	$(CPP) -o $@ -c $(CPP_FLAGS) $<

OBJ_FILES += $(BUILD)/sqlite3.o
$(BUILD)/sqlite3.o: vendor/sqlitecpp/sqlite3/sqlite3.c | $(BUILD)
	$(CC) -o $@ -c $<

OBJ_FILES += $(BUILD)/sqlitecpp-Backup.o
OBJ_FILES += $(BUILD)/sqlitecpp-Database.o
OBJ_FILES += $(BUILD)/sqlitecpp-Savepoint.o
OBJ_FILES += $(BUILD)/sqlitecpp-Transaction.o
OBJ_FILES += $(BUILD)/sqlitecpp-Column.o
OBJ_FILES += $(BUILD)/sqlitecpp-Exception.o
OBJ_FILES += $(BUILD)/sqlitecpp-Statement.o
$(BUILD)/sqlitecpp-%.o: vendor/sqlitecpp/src/%.cpp | $(BUILD)
	$(CPP) -o $@ -c $(CPP_FLAGS) -I vendor/sqlitecpp/src -I vendor/sqlitecpp/sqlite3 $^

$(BUILD)/$(EXEC): $(OBJ_FILES) | $(BUILD)
	$(CPP) -o $@ $(CPP_FLAGS) $^ $(LD_FLAGS)

host: $(BUILD)/$(EXEC)
	$(BUILD)/$(EXEC)
