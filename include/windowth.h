#pragma once

#include <string>
#include <cstring>
#include <iostream>

extern void windowth(int width, int height, const char *name, std::function<void(void)> render_content);