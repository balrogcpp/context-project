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
inline ImVec4 rgbaToVec4(float r, float g, float b, float a) { return ImVec4(r / 255.f, g / 255.f, b / 255.f, a / 255.f); }
inline ImVec4 rgbaToVec4(ImColor &col) { return col.Value; }
inline ImVec2 GetTextLength(std::string text) { return ImGui::CalcTextSize(text.c_str()); }

void DrawTabHorizontally(std::string childName, ImVec2 childSize, std::vector<std::string> tabNames, int &selectedSubTab);

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
void SetupImGuiStyle_NeverBlue();
void SetupImGuiStyle_NeverDark();
void SetupImGuiStyle_NeverLight();
}  // namespace ImStyle
