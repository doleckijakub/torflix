BUILD = build
EXEC = torflix

default: $(BUILD)/$(EXEC)

CPP := g++
CPP_FLAGS += -I vendor/crow/include
LD_FLAGS += 

$(BUILD):
	mkdir -p $@

$(BUILD)/$(EXEC): src/*.cpp | build
	$(CPP) -o $@ $(CPP_FLAGS) $^ $(LD_FLAGS)

host: $(BUILD)/$(EXEC)
	$(BUILD)/$(EXEC)
