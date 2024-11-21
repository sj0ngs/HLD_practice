#pragma once

#include "framework.h"

#include <Engine\global.h>
#include <Engine\CEngine.h>

// Engine Library
#ifdef _DEBUG
#pragma comment(lib, "Engine//Engine_d")
#else
#pragma comment(lib, "Engine//Engine")
#endif // _DEBUG

// Script Library
#ifdef _DEBUG
#pragma comment(lib, "Script//Script_d")
#else
#pragma comment(lib, "Script//Script")
#endif // _DEBUG
