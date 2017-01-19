// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#define _CRT_SECURE_NO_WARNINGS

#include "targetver.h"

#include <limits.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>

#include <tchar.h>

// I am so, SO sorry!
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

// Everything is included here...
#include "types.h"
#include "main.h"
#include "util.h"
#include "bsp.h"
#include "bsprand.h"
#include "file.h"
#include "vpk.h"
