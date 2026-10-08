#pragma once


#include <nn/nn_Assert.h>
#include <nn/nn_Log.h>
#if defined(NN_BUILD_CONFIG_OS_HORIZON)
#include <nn/oe.h>
#endif
#include <nn/os.h>
#include <nn/hid.h>
#include <nn/fs.h>

#include <nn/gll.h>
#include "GraphicsHelper.h"

#include <nn/util/util_Vector.h>
#include <nn/util/util_Color.h>
#include <nn/util/util_MathTypes.h>

#include "GraphicsHelper.h"

using namespace nn::util;



GLuint GetShaderProgramId();

void InitSystem();
void UninitSystem();

void SwapBuffers();


