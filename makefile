# compiler options
MODULES := core debug io graphics

#serenity-gl files
SRC_DIR   := src include $(addprefix include/firefly/,$(MODULES))
BUILD     := src include $(addprefix include/firefly/,$(MODULES))
BUILD_DIR := $(addprefix build/,$(BUILD))
INC       := $(addprefix -I,$(SRC_DIR)) -Iexternal
LIB       := -Lexternal
SRC       := $(foreach sdir,$(SRC_DIR),$(wildcard $(sdir)/*.cpp))
OBJ       := $(SRC:%.cpp=build/%.o)

vpath %.cpp $(SRC_DIR)

CC         := g++ -g
PROJECT    := -fno-exceptions -pthread
STATIC_LIB := external/libglfw.a external/libGLEW.a
CFLAGS     := $(PROJECT) $(INC) -Wall -Werror -O -std=c++0x
LDFLAGS-S  := $(PROJECT) $(LIB) -lGL
LDFLAGS    := $(LDFLAGS-S) -lGLEW -lglfw
EXECUTABLE := demo

#build macro
define make-goal
$1/%.o: %.cpp
	@$(CC) $(CFLAGS) -c $$< -o $$@
	@echo $$@
endef

.PHONY: all checkdirs clean

#targets
all: legacy

test:
	@echo src: $(SRC)
	@echo obj: $(OBJ)
	@echo obj: $(BUILD_DIR)

firefly: checkdirs $(EXECUTABLE).exe-static

legacy: checkdirs $(EXECUTABLE).exe

$(EXECUTABLE).exe-static: $(OBJ)
	@$(CC) $(LDFLAGS-S) $^ $(STATIC_LIB) -o $(EXECUTABLE)
	@echo firefly done.

$(EXECUTABLE).exe: $(OBJ)
	@$(CC) $(LDFLAGS) $^ -lglfw -lGLEW -o $(EXECUTABLE)
	@echo firefly-legacy done.

checkdirs: $(BUILD_DIR)

$(BUILD_DIR):
	@mkdir -p $@

clean:
	@rm -rf $(BUILD_DIR)
	@rm -rf $(EXECUTABLE)

$(foreach bdir,$(BUILD_DIR),$(eval $(call make-goal,$(bdir))))