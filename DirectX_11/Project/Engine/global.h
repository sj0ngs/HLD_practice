#pragma once

#include <vector>
#include <list>
#include <map>
#include <string>
#include <string_view>
#include <queue>
#include <stack>
#include <set>
#include <algorithm>
#include <limits>

using std::vector;
using std::list;
using std::map;
using std::make_pair;
using std::string;
using std::to_string;
using std::wstring;
using std::wstring_view;
using std::queue;
using std::priority_queue;
using std::stack;
using std::set;
using std::pair;

#include <typeinfo>
#include <assert.h>

#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#include <experimental/filesystem>
using namespace std::experimental;
using namespace std::experimental::filesystem;

#include <commdlg.h>

#include <wrl.h>
using namespace Microsoft::WRL;

#include <d3d11.h>					// DirectX 11
#include <d3dcompiler.h>			// Shader Compile
#include <DirectXMath.h>			// Dx Math
#include <DirectXPackedVector.h>	

using namespace DirectX;
using namespace DirectX::PackedVector;

#pragma comment(lib, "d3d11")
#pragma comment(lib, "d3dcompiler")

#include "SimpleMath.h"
using namespace DirectX::SimpleMath;

typedef Vector2 Vec2;
typedef Vector3 Vec3;
typedef Vector4 Vec4;

#include "define.h"
#include "struct.h"
#include "func.h"

#include "CSingleton.h"
