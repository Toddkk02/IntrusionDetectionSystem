CXX = g++
IMGUI_DIR = imgui
CXXFLAGS = -std=c++17 -Wall -Wextra -g
CXXFLAGS += -I$(IMGUI_DIR) -I$(IMGUI_DIR)/backends
CXXFLAGS += -I/usr/include/SDL2
CXXFLAGS += $(shell sdl2-config --cflags)
LDFLAGS = $(shell sdl2-config --libs) -lncurses -lstdc++fs

SRCS = main.cpp \
       core/AlertManager.cpp \
       core/AlertQueue.cpp \
       hids/BruteForceDetector.cpp \
       hids/LogWatcher.cpp \
       hids/ProcessMonitor.cpp \
       nids/PacketSniffer.cpp \
       nids/PortScanDetector.cpp \
       utils/Logger.cpp \
       ui/Dashboard.cpp \
       ui/TuiDashboard.cpp \
       $(IMGUI_DIR)/imgui.cpp \
       $(IMGUI_DIR)/imgui_draw.cpp \
       $(IMGUI_DIR)/imgui_tables.cpp \
       $(IMGUI_DIR)/imgui_widgets.cpp \
       $(IMGUI_DIR)/backends/imgui_impl_sdl2.cpp \
       $(IMGUI_DIR)/backends/imgui_impl_sdlrenderer2.cpp

OBJS = $(SRCS:.cpp=.o)
TARGET = ids

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS) $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

re: clean all

install:
	sudo cp $(TARGET) /usr/local/bin/ids
	sudo cp scripts/ids-start.sh /usr/local/bin/ids-start.sh
	sudo chmod +x /usr/local/bin/ids-start.sh
	sudo cp systemd/ids.service /etc/systemd/system/ids.service
	sudo systemctl daemon-reload
	sudo systemctl enable ids

uninstall:
	sudo systemctl stop ids
	sudo systemctl disable ids
	sudo rm -f /usr/local/bin/ids
	sudo rm -f /usr/local/bin/ids-start.sh
	sudo rm -f /etc/systemd/system/ids.service
	sudo systemctl daemon-reload

.PHONY: all clean re install uninstall
