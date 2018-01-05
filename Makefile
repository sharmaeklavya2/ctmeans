LIB_SRCDIR := src/lib
LIB_SOURCES := $(wildcard $(LIB_SRCDIR)/*.cpp)
LIB_HEADERS := $(wildcard $(LIB_SRCDIR)/*.h)

COMMON_FLAGS := -std=c++11 -Wall -Wpedantic
DEBUG_FLAGS := -D DEBUG -g $(COMMON_FLAGS)
RELEASE_FLAGS := -D NDEBUG -O2 $(COMMON_FLAGS)

MASTER_BUILDDIR := build
DEBUG_BUILDDIR := $(MASTER_BUILDDIR)/debug
RELEASE_BUILDDIR := $(MASTER_BUILDDIR)/release
MAIN_OUTPUT := ctmeans

ifneq ($(DEBUG),)
	FLAGS := $(DEBUG_FLAGS)
	BUILDDIR := $(DEBUG_BUILDDIR)
else
	FLAGS := $(RELEASE_FLAGS)
	BUILDDIR := $(RELEASE_BUILDDIR)
endif

LIB_BUILDDIR := $(BUILDDIR)/lib
CXX := g++ $(FLAGS)

.PHONY: $(MAIN_OUTPUT)
$(MAIN_OUTPUT): $(BUILDDIR)/$(MAIN_OUTPUT)
	ln -nsf $(BUILDDIR)/$(MAIN_OUTPUT) $(MAIN_OUTPUT)

.PHONY: all
all: $(MAIN_OUTPUT) ;

.PHONY: clean
clean:
	rm -rf var $(MASTER_BUILDDIR)
	rm $(MAIN_OUTPUT)
	find -name "*.pyc" -type f -delete
	find -name "__pycache__" -type d -delete

# Lib files

COMMON_HEADERS := $(LIB_SRCDIR)/matrix.h $(LIB_SRCDIR)/vecio.h

$(LIB_BUILDDIR)/matrix.o: $(LIB_SRCDIR)/matrix.cpp $(COMMON_HEADERS)
	@mkdir -p $(LIB_BUILDDIR)
	$(CXX) -c $< -o $@

$(LIB_BUILDDIR)/kdtree.o: $(LIB_SRCDIR)/kdtree.cpp $(LIB_SRCDIR)/kdtree.h $(COMMON_HEADERS)
	@mkdir -p $(LIB_BUILDDIR)
	$(CXX) -c $< -o $@

$(LIB_BUILDDIR)/nniter.o: $(LIB_SRCDIR)/nniter.cpp $(LIB_SRCDIR)/nniter.h $(COMMON_HEADERS)
	@mkdir -p $(LIB_BUILDDIR)
	$(CXX) -c $< -o $@

$(LIB_BUILDDIR)/nniter_kd.o: $(LIB_SRCDIR)/nniter_kd.cpp $(LIB_SRCDIR)/nniter_kd.h \
	$(LIB_SRCDIR)/kdtree.h $(COMMON_HEADERS)
	@mkdir -p $(LIB_BUILDDIR)
	$(CXX) -c $< -o $@

$(LIB_BUILDDIR)/gen_gmix.o: $(LIB_SRCDIR)/gen_gmix.cpp $(COMMON_HEADERS)
	@mkdir -p $(LIB_BUILDDIR)
	$(CXX) -c $< -o $@

$(LIB_BUILDDIR)/ctmeans.o: $(LIB_SRCDIR)/ctmeans.cpp $(LIB_HEADERS)
	@mkdir -p $(LIB_BUILDDIR)
	$(CXX) -c $< -o $@

# Executable files

MAIN_OBJS := $(LIB_BUILDDIR)/ctmeans.o $(LIB_BUILDDIR)/kdtree.o $(LIB_BUILDDIR)/matrix.o \
	$(LIB_BUILDDIR)/nniter.o $(LIB_BUILDDIR)/nniter_kd.o $(BUILDDIR)/main.o

$(BUILDDIR)/main.o: src/main.cpp $(LIB_HEADERS)
	@mkdir -p $(BUILDDIR)
	$(CXX) -c $< -o $@

$(BUILDDIR)/$(MAIN_OUTPUT): $(MAIN_OBJS)
	$(CXX) $(MAIN_OBJS) -o $(BUILDDIR)/$(MAIN_OUTPUT)
