all:
	c++ *.cpp *.h -Wunused -std=gnu++11 -o arma3_linux_launcher `pkg-config --cflags --libs gtk+-3.0` 