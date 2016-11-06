all:
	c++ *.cpp *.h -Wunused -std=gnu++11 -o arma3-linux-launcher `pkg-config --cflags --libs gtkmm-3.0`