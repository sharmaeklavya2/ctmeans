SOURCES = src/*.cpp
OUTPUT = ctmeans
COMMON_FLAGS = -std=c++11 -Wall -Wpedantic #-Wno-sign-compare
DEBUG_FLAGS = -D DEBUG -g $(COMMON_FLAGS)
RELEASE_FLAGS = -D NDEBUG -O2 $(COMMON_FLAGS)

debug:
	g++ $(DEBUG_FLAGS) $(SOURCES) -o $(OUTPUT)
release:
	g++ $(RELEASE_FLAGS) $(SOURCES) -o $(OUTPUT)
clean:
	rm $(OUTPUT)
	rm -r var
	find -name "*.pyc" -type f -delete
	find -name "__pycache__" -type d -delete
