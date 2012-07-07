# compiler options
MODULES := core debug io graphics

#serenity-gl files
SRC_DIR   := src include $(addprefix include/firefly/,$(MODULES))
BUILD     := src include $(addprefix include/firefly/,$(MODULES))
BUILD_DIR := $(addprefix build/,$(BUILD))
INCLUDES  := $(addprefix -I,$(SRC_DIR)) -Iexternal
LIB       :=
SRC       := $(foreach sdir,$(SRC_DIR),$(wildcard $(sdir)/*.cpp))
OBJ       := $(SRC:%.cpp=build/%.o)

vpath %.cpp $(SRC_DIR)

CC         := g++ -g
CFLAGS     := $(INCLUDES) -Wall -Werror -O -std=c++0x
LDFLAGS    := $(LIB) -lGL -lglfw -lGLEW
EXECUTABLE := demo

#build macro
define make-goal
$1/%.o: %.cpp
	@$(CC) $(CFLAGS) -c $$< -o $$@
	@echo $$@
endef

.PHONY: all checkdirs clean

#targets
all: checkdirs $(EXECUTABLE).exe

test:
	@echo src: $(SRC)
	@echo obj: $(OBJ)
	@echo obj: $(BUILD_DIR)

firefly: checkdirs $(EXECUTABLE).exe

$(EXECUTABLE).exe: $(OBJ)
	@$(CC) $(LDFLAGS) $^ -o $(EXECUTABLE)
	@echo done.

checkdirs: $(BUILD_DIR)

$(BUILD_DIR):
	@mkdir -p $@

clean:
	@rm -rf $(BUILD_DIR)
	@rm -rf $(EXECUTABLE)

$(foreach bdir,$(BUILD_DIR),$(eval $(call make-goal,$(bdir))))