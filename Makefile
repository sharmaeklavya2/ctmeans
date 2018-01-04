SOURCES = src/*.cpp
HEADERS = src/*.h

COMMON_FLAGS = -std=c++11 -Wall -Wpedantic #-Wno-sign-compare
DEBUG_FLAGS = -D DEBUG -g $(COMMON_FLAGS)
RELEASE_FLAGS = -D NDEBUG -O2 $(COMMON_FLAGS)

MASTER_BUILDDIR = build
DEBUG_BUILDDIR = $(MASTER_BUILDDIR)/debug
RELEASE_BUILDDIR = $(MASTER_BUILDDIR)/release
MAIN_OUTPUT = ctmeans

ifneq ($(DEBUG),)
	FLAGS = $(DEBUG_FLAGS)
	BUILDDIR = $(DEBUG_BUILDDIR)
else
	FLAGS = $(RELEASE_FLAGS)
	BUILDDIR = $(RELEASE_BUILDDIR)
endif

.PHONY: all
all: $(BUILDDIR)/$(MAIN_OUTPUT)
	ln -nsf $(BUILDDIR)/$(MAIN_OUTPUT) $(MAIN_OUTPUT)

$(BUILDDIR):
	mkdir -p $(BUILDDIR)

$(BUILDDIR)/$(MAIN_OUTPUT): $(SOURCES) $(HEADERS) $(BUILDDIR)
	g++ $(FLAGS) $(SOURCES) -o $(BUILDDIR)/$(MAIN_OUTPUT)

.PHONY: clean
clean:
	rm -rf var $(MASTER_BUILDDIR)
	rm $(MAIN_OUTPUT)
	find -name "*.pyc" -type f -delete
	find -name "__pycache__" -type d -delete
