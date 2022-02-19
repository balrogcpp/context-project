// This source file is part of "glue project". Created by Andrey Vasiliev

#pragma once

namespace Glue {

///
bool GlobalMRTEnabled();

///
bool CPUSupportSSE();

///
bool CPUSupportSSE2();

///
bool CPUSupportSSE3();

///
bool CPUSupportSSE41();

///
bool CPUSupportSSE42();

///
bool CPUSupportNEON();

///
bool RenderSystemGL();

///
bool RenderSystemGL3();

///
bool RenderSystemGLES2();

}  // namespace Glue
