CC = g++
CXXFLAGS = -O2 -Wall -std=c++17 -DUNICODE -D_UNICODE -DWINVER=0x0601 -D_WIN32_WINNT=0x0601
LDFLAGS = -lgdi32 -luser32 -lgdiplus -lcomctl32 -mwindows
TARGET = rc2fish.exe

SOURCES = main.cpp window.cpp screen.cpp detector.cpp input.cpp fisher.cpp selector.cpp calibrator.cpp
HEADERS = config.h resource.h window.h screen.h detector.h input.h fisher.h selector.h calibrator.h

all: $(TARGET)

$(TARGET): $(SOURCES) $(HEADERS)
	$(CC) $(CXXFLAGS) -o $(TARGET) $(SOURCES) $(LDFLAGS)

clean:
	del /Q $(TARGET) 2>nul || true

.PHONY: all clean
