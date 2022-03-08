// This source file is part of Glue Engine. Created by Andrey Vasiliev
#pragma once

namespace ImGui {

// Taken from https://github.com/ocornut/imgui/issues/1537
void ToggleButton(const char* str_id, bool* v);

// Taken from https://github.com/ocornut/imgui/issues/4722
bool ColoredButtonV1(const char* label, const ImVec2& size_arg, ImU32 text_color, ImU32 bg_color_1, ImU32 bg_color_2);

}  // namespace ImGui
