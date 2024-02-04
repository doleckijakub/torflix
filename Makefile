BUILD = build
EXEC = torflix

default: $(BUILD)/$(EXEC)

SRC_FILES = $(wildcard src/*.cpp)
OBJ_FILES = $(patsubst src/%.cpp, $(BUILD)/%.o, $(SRC_FILES))
SRC := src
CPP := g++
CPP_FLAGS += -I vendor/crow/include
CPP_FLAGS += -I vendor/curl/include
CPP_FLAGS += -I vendor/rapidjson/include
LD_FLAGS += -L vendor/curl/lib -lcurl

$(BUILD):
	mkdir -p $@

$(BUILD)/%.o: $(SRC)/%.cpp | build
	$(CPP) -o $@ -c $(CPP_FLAGS) $<

$(BUILD)/$(EXEC): $(OBJ_FILES) | build
	$(CPP) -o $@ $(CPP_FLAGS) $^ $(LD_FLAGS)

host: $(BUILD)/$(EXEC)
	$(BUILD)/$(EXEC)
