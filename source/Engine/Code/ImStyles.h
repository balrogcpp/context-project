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
