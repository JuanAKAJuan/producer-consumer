CXX = g++

CXXFLAGS = -std=c++11 -Wall -Wextra -g -pthread

SRCS = producer_consumer.cpp

TARGET = producer_consumer

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) $< -o $@ $(LDFLAGS)
	@echo "Build complete. Executable: $(TARGET)"

clean:
	@echo "Cleaning up..."
	rm -f $(TARGET)
	@echo "Cleanup complete."

.PHONY: all clean
