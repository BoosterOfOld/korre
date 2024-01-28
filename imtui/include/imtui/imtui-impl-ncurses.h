/*! \file imtui-impl-ncurses.h
 *  \brief Enter description here.
 */

#pragma once

#define D_HORT "\x80"
#define D_VERT "\x81"
#define D_TOP_LEFT "\x82"
#define D_TOP_RIGHT "\x83"
#define D_BOTTOM_LEFT "\x84"
#define D_BOTTOM_RIGHT "\x85"
#define D_VERT_D_LEFT_SPLIT "\x86"
#define D_VERT_D_RIGHT_SPLIT "\x87"
#define D_HORT_D_BOTTOM_SPLIT "\x88"
#define D_HORT_D_TOP_SPLIT "\x89"
#define D_CROSS "\x8A"



namespace ImTui {
struct TScreen;
}

// the interface below allows the user to decide when the application is active or not
// this can be used to reduce the redraw rate, and thus the CPU usage, when suitable
// for example - there is no user input, or the displayed content hasn't changed significantly

// fps_active - specify the redraw rate when the application is active
// fps_idle - specify the redraw rate when the application is not active
ImTui::TScreen * ImTui_ImplNcurses_Init(bool mouseSupport, float fps_active = 60.0, float fps_idle = -1.0);

void ImTui_ImplNcurses_Shutdown();

// returns true if there is any user input from the keyboard/mouse
bool ImTui_ImplNcurses_NewFrame();

// active - specify which redraw rate to use: fps_active or fps_idle
void ImTui_ImplNcurses_DrawScreen(bool active = true);
void ImTui_ImplNcurses_Clear();

bool ImTui_ImplNcurses_ProcessEvent();
