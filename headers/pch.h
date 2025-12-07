#define SDL_MAIN_HANDLED
#include <windows.h>
#include <iostream>
#include <cstdio>
#include <cstdint>
#include <cmath> 
#include <algorithm>
#include <shlobj.h>
#include <map>

#include <SDL.h>
#include <SDL_syswm.h> 

#define IMGUI_IMPL_OPENGL_LOADER_GLAD

#include <glad/glad.h> 

// --- ImGui Includes ---
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include <imgui_impl_opengl3.h> 