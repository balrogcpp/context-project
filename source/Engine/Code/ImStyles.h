/// created by Andrey Vasiliev

#pragma once
#include <imgui.h>

inline ImVec2 operator+(const ImVec2 &a, const ImVec2 &b) { return ImVec2(a.x + b.x, a.y + b.y); }
inline ImVec4 operator+(const ImVec4 &a, const ImVec4 &b) { return ImVec4(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w); }

inline ImVec2 operator-(const ImVec2 &a, const ImVec2 &b) { return ImVec2(a.x - b.x, a.y - b.y); }
inline ImVec4 operator-(const ImVec4 &a, const ImVec4 &b) { return ImVec4(a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w); }

inline ImVec2 operator*(const ImVec2 &a, const ImVec2 &b) { return ImVec2(a.x * b.x, a.y * b.y); }
inline ImVec4 operator*(const ImVec4 &a, const ImVec4 &b) { return ImVec4(a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w); }

inline ImVec2 operator/(const ImVec2 &a, const ImVec2 &b) { return ImVec2(a.x / b.x, a.y / b.y); }
inline ImVec4 operator/(const ImVec4 &a, const ImVec4 &b) { return ImVec4(a.x / b.x, a.y / b.y, a.z / b.z, a.w / b.w); }

template <typename T>
inline ImVec2 operator*(const ImVec2 &a, T b) {
  return ImVec2(a.x * b, a.y * b);
}

template <typename T>
inline ImVec4 operator*(const ImVec4 &a, T b) {
  return ImVec4(a.x * b, a.y * b, a.z * b, a.w);
}

template <typename T>
inline ImVec2 operator/(const ImVec2 &a, T b) {
  return ImVec2(a.x / b, a.y / b);
}

template <typename T>
inline ImVec4 operator/(const ImVec4 &a, T b) {
  return ImVec4(a.x / b, a.y / b, a.z / b, a.w);
}

inline float ImGetWidth() { return ImGui::GetContentRegionAvail().x; }
inline float ImGetHeight() { return ImGui::GetContentRegionAvail().y; }
inline ImVec4 RBGA2Vec4(float r, float g, float b, float a) { return ImVec4(r / 255.f, g / 255.f, b / 255.f, a / 255.f); }
inline ImVec4 RGBA2Vec4(ImColor &col) { return col.Value; }
inline ImVec2 GetTextLength(std::string text) { return ImGui::CalcTextSize(text.c_str()); }


namespace ImStyle {
void SetupImGuiStyle_Unreal();
void SetupImGuiStyle_DeepDark();
void SetupImGuiStyle_DiscordDark();
void SetupImGuiStyle_DarkRuda();
void SetupImGuiStyle_Darcula();
void SetupImGuiStyle_RedFont();
void SetupImGuiStyle_MaterialFlat();
void SetupImGuiStyle_GreenFont();
void SetupImGuiStyle_DarkCustom();
void SetupImGuiStyle_RedCustom();
void SetupImGuiStyle_Enemymouse();
void SetupImGuiStyle_SpectrumLight();
void SetupImGuiStyle_SpectrumDark();
}  // namespace ImStyle

namespace ImGuiB {
void DrawTabHorizontally(std::string childName, ImVec2 childSize, std::vector<std::string> tabNames, int &selectedSubTab);

void SetupImGuiStyle_NeverBlue();
void SetupImGuiStyle_NeverDark();
void SetupImGuiStyle_NeverLight();
bool TabButton(const char *label, bool active);
bool Checkbox(const char *label, bool *v);
bool SliderInt(const char *label, int *v, int v_min, int v_max, const char *format = "%d", ImGuiSliderFlags flags = 0);
bool Combo(const char *label, int *current_item, const char *items_separated_by_zeros, int popup_max_height_in_items = -1);
bool Toggle(const char *label, bool *v);
bool ButtonScrollableEx(const char *label, const ImVec2 &size_arg = ImVec2(0, 0), ImGuiButtonFlags flags = 0);
bool ButtonScrollable(const char *label, const ImVec2 &size_arg = ImVec2(0, 0));

enum ImGuiToggleFlags_ {
  ImGuiToggleFlags_None = 0,
  ImGuiToggleFlags_Animated = 1 << 0,       // The toggle should be animated. Mutually exclusive with ImGuiToggleFlags_Static.
  ImGuiToggleFlags_Static = 1 << 1,         // The toggle should not animate. Mutually exclusive with ImGuiToggleFlags_Animated.
  ImGuiToggleFlags_BorderedFrame = 1 << 2,  // The toggle should have a border drawn on the frame.
  ImGuiToggleFlags_BorderedKnob = 1 << 3,   // The toggle should have a border drawn on the knob.
  ImGuiToggleFlags_Bordered = ImGuiToggleFlags_BorderedFrame | ImGuiToggleFlags_BorderedKnob,  // Shorthand for bordered frame and knob.
  ImGuiToggleFlags_Default = ImGuiToggleFlags_Static,  // The default flags used when no ImGuiToggleFlags_ are specified.
};

typedef int ImGuiToggleFlags;  // -> enum ImGuiToggleFlags_        // Flags: for Toggle() modes

// Widgets: Toggle Switches
// - Toggles behave similarly to ImGui::Checkbox()
// - Sometimes called a toggle switch, see also: https://en.wikipedia.org/wiki/Toggle_switch_(widget)
// - They represent two mutually exclusive states, with an optional animation on the UI when toggled.
// Optional parameters:
// - flags: Values from the ImGuiToggleFlags_ enumeration to set toggle modes.
// - speed: Animation speed scalar. (0,...] default: 1.0f (Overloads with this parameter imply ImGuiToggleFlags_Animated)
// - frame_rounding: A scalar that controls how rounded the toggle frame is. 0 is square, 1 is round. (0, 1) default 1.0f
// - knob_rounding: A scalar that controls how rounded the toggle knob is. 0 is square, 1 is round. (0, 1) default 1.0f
bool ToggleA(const char *label, bool *v);
bool Toggle(const char *label, bool *v);
bool Toggle(const char *label, bool *v, ImGuiToggleFlags flags);
bool Toggle(const char *label, bool *v, ImGuiToggleFlags flags, float speed);
bool Toggle(const char *label, bool *v, ImGuiToggleFlags flags, float frame_rounding, float knob_rounding);
bool Toggle(const char *label, bool *v, ImGuiToggleFlags flags, float speed, float frame_rounding, float knob_rounding);
}  // namespace ImGui
