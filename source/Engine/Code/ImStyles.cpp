/// created by Andrey Vasiliev

#include "ImStyles.h"
#include <algorithm>
#include <imgui_internal.h>

namespace Spectrum {
// a list of changes introduced to change the look of the widgets.
// Collected here as const rather than being magic numbers spread
// around imgui.cpp and imgui_widgets.cpp.
const float CHECKBOX_BORDER_SIZE = 2.0f;
const float CHECKBOX_ROUNDING = 2.0f;

// Load SourceSansProRegular and sets it as a default font.
// You may want to call ImGui::GetIO().Fonts->Clear() before this
void LoadFont(float size = 16.0f);

// Sets the ImGui style to Spectrum
void StyleColorsSpectrum();

namespace {  // Unnamed namespace, since we only use this here.
inline unsigned int Color(unsigned int c) {
  // add alpha.
  // also swap red and blue channel for some reason.
  // todo: figure out why, and fix it.
  const short a = 0xFF;
  const short r = (c >> 16) & 0xFF;
  const short g = (c >> 8) & 0xFF;
  const short b = (c >> 0) & 0xFF;
  return (a << 24) | (r << 0) | (g << 8) | (b << 16);
}
}  // namespace
// all colors are from http://spectrum.corp.adobe.com/color.html

inline unsigned int color_alpha(unsigned int alpha, unsigned int c) { return ((alpha & 0xFF) << 24) | (c & 0x00FFFFFF); }

namespace Static {                     // static colors
const unsigned int NONE = 0x00000000;  // transparent
const unsigned int WHITE = Color(0xFFFFFF);
const unsigned int BLACK = Color(0x000000);
const unsigned int GRAY200 = Color(0xF4F4F4);
const unsigned int GRAY300 = Color(0xEAEAEA);
const unsigned int GRAY400 = Color(0xD3D3D3);
const unsigned int GRAY500 = Color(0xBCBCBC);
const unsigned int GRAY600 = Color(0x959595);
const unsigned int GRAY700 = Color(0x767676);
const unsigned int GRAY800 = Color(0x505050);
const unsigned int GRAY900 = Color(0x323232);
const unsigned int BLUE400 = Color(0x378EF0);
const unsigned int BLUE500 = Color(0x2680EB);
const unsigned int BLUE600 = Color(0x1473E6);
const unsigned int BLUE700 = Color(0x0D66D0);
const unsigned int RED400 = Color(0xEC5B62);
const unsigned int RED500 = Color(0xE34850);
const unsigned int RED600 = Color(0xD7373F);
const unsigned int RED700 = Color(0xC9252D);
const unsigned int ORANGE400 = Color(0xF29423);
const unsigned int ORANGE500 = Color(0xE68619);
const unsigned int ORANGE600 = Color(0xDA7B11);
const unsigned int ORANGE700 = Color(0xCB6F10);
const unsigned int GREEN400 = Color(0x33AB84);
const unsigned int GREEN500 = Color(0x2D9D78);
const unsigned int GREEN600 = Color(0x268E6C);
const unsigned int GREEN700 = Color(0x12805C);
}  // namespace Static

namespace Light {
const unsigned int GRAY50 = Color(0xFFFFFF);
const unsigned int GRAY75 = Color(0xFAFAFA);
const unsigned int GRAY100 = Color(0xF5F5F5);
const unsigned int GRAY200 = Color(0xEAEAEA);
const unsigned int GRAY300 = Color(0xE1E1E1);
const unsigned int GRAY400 = Color(0xCACACA);
const unsigned int GRAY500 = Color(0xB3B3B3);
const unsigned int GRAY600 = Color(0x8E8E8E);
const unsigned int GRAY700 = Color(0x707070);
const unsigned int GRAY800 = Color(0x4B4B4B);
const unsigned int GRAY900 = Color(0x2C2C2C);
const unsigned int BLUE400 = Color(0x2680EB);
const unsigned int BLUE500 = Color(0x1473E6);
const unsigned int BLUE600 = Color(0x0D66D0);
const unsigned int BLUE700 = Color(0x095ABA);
const unsigned int RED400 = Color(0xE34850);
const unsigned int RED500 = Color(0xD7373F);
const unsigned int RED600 = Color(0xC9252D);
const unsigned int RED700 = Color(0xBB121A);
const unsigned int ORANGE400 = Color(0xE68619);
const unsigned int ORANGE500 = Color(0xDA7B11);
const unsigned int ORANGE600 = Color(0xCB6F10);
const unsigned int ORANGE700 = Color(0xBD640D);
const unsigned int GREEN400 = Color(0x2D9D78);
const unsigned int GREEN500 = Color(0x268E6C);
const unsigned int GREEN600 = Color(0x12805C);
const unsigned int GREEN700 = Color(0x107154);
const unsigned int INDIGO400 = Color(0x6767EC);
const unsigned int INDIGO500 = Color(0x5C5CE0);
const unsigned int INDIGO600 = Color(0x5151D3);
const unsigned int INDIGO700 = Color(0x4646C6);
const unsigned int CELERY400 = Color(0x44B556);
const unsigned int CELERY500 = Color(0x3DA74E);
const unsigned int CELERY600 = Color(0x379947);
const unsigned int CELERY700 = Color(0x318B40);
const unsigned int MAGENTA400 = Color(0xD83790);
const unsigned int MAGENTA500 = Color(0xCE2783);
const unsigned int MAGENTA600 = Color(0xBC1C74);
const unsigned int MAGENTA700 = Color(0xAE0E66);
const unsigned int YELLOW400 = Color(0xDFBF00);
const unsigned int YELLOW500 = Color(0xD2B200);
const unsigned int YELLOW600 = Color(0xC4A600);
const unsigned int YELLOW700 = Color(0xB79900);
const unsigned int FUCHSIA400 = Color(0xC038CC);
const unsigned int FUCHSIA500 = Color(0xB130BD);
const unsigned int FUCHSIA600 = Color(0xA228AD);
const unsigned int FUCHSIA700 = Color(0x93219E);
const unsigned int SEAFOAM400 = Color(0x1B959A);
const unsigned int SEAFOAM500 = Color(0x16878C);
const unsigned int SEAFOAM600 = Color(0x0F797D);
const unsigned int SEAFOAM700 = Color(0x096C6F);
const unsigned int CHARTREUSE400 = Color(0x85D044);
const unsigned int CHARTREUSE500 = Color(0x7CC33F);
const unsigned int CHARTREUSE600 = Color(0x73B53A);
const unsigned int CHARTREUSE700 = Color(0x6AA834);
const unsigned int PURPLE400 = Color(0x9256D9);
const unsigned int PURPLE500 = Color(0x864CCC);
const unsigned int PURPLE600 = Color(0x7A42BF);
const unsigned int PURPLE700 = Color(0x6F38B1);
}  // namespace Light

namespace Dark {
const unsigned int GRAY50 = Color(0x252525);
const unsigned int GRAY75 = Color(0x2F2F2F);
const unsigned int GRAY100 = Color(0x323232);
const unsigned int GRAY200 = Color(0x393939);
const unsigned int GRAY300 = Color(0x3E3E3E);
const unsigned int GRAY400 = Color(0x4D4D4D);
const unsigned int GRAY500 = Color(0x5C5C5C);
const unsigned int GRAY600 = Color(0x7B7B7B);
const unsigned int GRAY700 = Color(0x999999);
const unsigned int GRAY800 = Color(0xCDCDCD);
const unsigned int GRAY900 = Color(0xFFFFFF);
const unsigned int BLUE400 = Color(0x2680EB);
const unsigned int BLUE500 = Color(0x378EF0);
const unsigned int BLUE600 = Color(0x4B9CF5);
const unsigned int BLUE700 = Color(0x5AA9FA);
const unsigned int RED400 = Color(0xE34850);
const unsigned int RED500 = Color(0xEC5B62);
const unsigned int RED600 = Color(0xF76D74);
const unsigned int RED700 = Color(0xFF7B82);
const unsigned int ORANGE400 = Color(0xE68619);
const unsigned int ORANGE500 = Color(0xF29423);
const unsigned int ORANGE600 = Color(0xF9A43F);
const unsigned int ORANGE700 = Color(0xFFB55B);
const unsigned int GREEN400 = Color(0x2D9D78);
const unsigned int GREEN500 = Color(0x33AB84);
const unsigned int GREEN600 = Color(0x39B990);
const unsigned int GREEN700 = Color(0x3FC89C);
const unsigned int INDIGO400 = Color(0x6767EC);
const unsigned int INDIGO500 = Color(0x7575F1);
const unsigned int INDIGO600 = Color(0x8282F6);
const unsigned int INDIGO700 = Color(0x9090FA);
const unsigned int CELERY400 = Color(0x44B556);
const unsigned int CELERY500 = Color(0x4BC35F);
const unsigned int CELERY600 = Color(0x51D267);
const unsigned int CELERY700 = Color(0x58E06F);
const unsigned int MAGENTA400 = Color(0xD83790);
const unsigned int MAGENTA500 = Color(0xE2499D);
const unsigned int MAGENTA600 = Color(0xEC5AAA);
const unsigned int MAGENTA700 = Color(0xF56BB7);
const unsigned int YELLOW400 = Color(0xDFBF00);
const unsigned int YELLOW500 = Color(0xEDCC00);
const unsigned int YELLOW600 = Color(0xFAD900);
const unsigned int YELLOW700 = Color(0xFFE22E);
const unsigned int FUCHSIA400 = Color(0xC038CC);
const unsigned int FUCHSIA500 = Color(0xCF3EDC);
const unsigned int FUCHSIA600 = Color(0xD951E5);
const unsigned int FUCHSIA700 = Color(0xE366EF);
const unsigned int SEAFOAM400 = Color(0x1B959A);
const unsigned int SEAFOAM500 = Color(0x20A3A8);
const unsigned int SEAFOAM600 = Color(0x23B2B8);
const unsigned int SEAFOAM700 = Color(0x26C0C7);
const unsigned int CHARTREUSE400 = Color(0x85D044);
const unsigned int CHARTREUSE500 = Color(0x8EDE49);
const unsigned int CHARTREUSE600 = Color(0x9BEC54);
const unsigned int CHARTREUSE700 = Color(0xA3F858);
const unsigned int PURPLE400 = Color(0x9256D9);
const unsigned int PURPLE500 = Color(0x9D64E1);
const unsigned int PURPLE600 = Color(0xA873E9);
const unsigned int PURPLE700 = Color(0xB483F0);
}  // namespace Dark
}  // namespace Spectrum

namespace ImGuiB {
void SetupImGuiStyle_Unreal() {
  // Unreal style by dev0-1 from ImThemes
  ImGuiStyle &style = ImGui::GetStyle();

  style.Alpha = 1.0f;
  style.DisabledAlpha = 0.6000000238418579f;
  style.WindowPadding = ImVec2(8.0f, 8.0f);
  style.WindowRounding = 0.0f;
  style.WindowBorderSize = 1.0f;
  style.WindowMinSize = ImVec2(32.0f, 32.0f);
  style.WindowTitleAlign = ImVec2(0.0f, 0.5f);
  style.WindowMenuButtonPosition = ImGuiDir_Left;
  style.ChildRounding = 0.0f;
  style.ChildBorderSize = 1.0f;
  style.PopupRounding = 0.0f;
  style.PopupBorderSize = 1.0f;
  style.FramePadding = ImVec2(4.0f, 3.0f);
  style.FrameRounding = 0.0f;
  style.FrameBorderSize = 0.0f;
  style.ItemSpacing = ImVec2(8.0f, 4.0f);
  style.ItemInnerSpacing = ImVec2(4.0f, 4.0f);
  style.CellPadding = ImVec2(4.0f, 2.0f);
  style.IndentSpacing = 21.0f;
  style.ColumnsMinSpacing = 6.0f;
  style.ScrollbarSize = 14.0f;
  style.ScrollbarRounding = 9.0f;
  style.GrabMinSize = 10.0f;
  style.GrabRounding = 0.0f;
  style.TabRounding = 4.0f;
  style.TabBorderSize = 0.0f;
  style.TabMinWidthForCloseButton = 0.0f;
  style.ColorButtonPosition = ImGuiDir_Right;
  style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
  style.SelectableTextAlign = ImVec2(0.0f, 0.0f);

  style.Colors[ImGuiCol_Text] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
  style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.4980392158031464f, 0.4980392158031464f, 0.4980392158031464f, 1.0f);
  style.Colors[ImGuiCol_WindowBg] = ImVec4(0.05882352963089943f, 0.05882352963089943f, 0.05882352963089943f, 0.9399999976158142f);
  style.Colors[ImGuiCol_ChildBg] = ImVec4(1.0f, 1.0f, 1.0f, 0.0f);
  style.Colors[ImGuiCol_PopupBg] = ImVec4(0.0784313753247261f, 0.0784313753247261f, 0.0784313753247261f, 0.9399999976158142f);
  style.Colors[ImGuiCol_Border] = ImVec4(0.4274509847164154f, 0.4274509847164154f, 0.4980392158031464f, 0.5f);
  style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
  style.Colors[ImGuiCol_FrameBg] = ImVec4(0.2000000029802322f, 0.2078431397676468f, 0.2196078449487686f, 0.5400000214576721f);
  style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.4000000059604645f, 0.4000000059604645f, 0.4000000059604645f, 0.4000000059604645f);
  style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.1764705926179886f, 0.1764705926179886f, 0.1764705926179886f, 0.6700000166893005f);
  style.Colors[ImGuiCol_TitleBg] = ImVec4(0.03921568766236305f, 0.03921568766236305f, 0.03921568766236305f, 1.0f);
  style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.2862745225429535f, 0.2862745225429535f, 0.2862745225429535f, 1.0f);
  style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.0f, 0.0f, 0.0f, 0.5099999904632568f);
  style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.1372549086809158f, 0.1372549086809158f, 0.1372549086809158f, 1.0f);
  style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.01960784383118153f, 0.01960784383118153f, 0.01960784383118153f, 0.5299999713897705f);
  style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.3098039329051971f, 0.3098039329051971f, 0.3098039329051971f, 1.0f);
  style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.407843142747879f, 0.407843142747879f, 0.407843142747879f, 1.0f);
  style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.5098039507865906f, 0.5098039507865906f, 0.5098039507865906f, 1.0f);
  style.Colors[ImGuiCol_CheckMark] = ImVec4(0.9372549057006836f, 0.9372549057006836f, 0.9372549057006836f, 1.0f);
  style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.5098039507865906f, 0.5098039507865906f, 0.5098039507865906f, 1.0f);
  style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.8588235378265381f, 0.8588235378265381f, 0.8588235378265381f, 1.0f);
  style.Colors[ImGuiCol_Button] = ImVec4(0.4392156898975372f, 0.4392156898975372f, 0.4392156898975372f, 0.4000000059604645f);
  style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.4588235318660736f, 0.4666666686534882f, 0.47843137383461f, 1.0f);
  style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.4196078479290009f, 0.4196078479290009f, 0.4196078479290009f, 1.0f);
  style.Colors[ImGuiCol_Header] = ImVec4(0.6980392336845398f, 0.6980392336845398f, 0.6980392336845398f, 0.3100000023841858f);
  style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.6980392336845398f, 0.6980392336845398f, 0.6980392336845398f, 0.800000011920929f);
  style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.47843137383461f, 0.4980392158031464f, 0.5176470875740051f, 1.0f);
  style.Colors[ImGuiCol_Separator] = ImVec4(0.4274509847164154f, 0.4274509847164154f, 0.4980392158031464f, 0.5f);
  style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.7176470756530762f, 0.7176470756530762f, 0.7176470756530762f, 0.7799999713897705f);
  style.Colors[ImGuiCol_SeparatorActive] = ImVec4(0.5098039507865906f, 0.5098039507865906f, 0.5098039507865906f, 1.0f);
  style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.9098039269447327f, 0.9098039269447327f, 0.9098039269447327f, 0.25f);
  style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.8078431487083435f, 0.8078431487083435f, 0.8078431487083435f, 0.6700000166893005f);
  style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.4588235318660736f, 0.4588235318660736f, 0.4588235318660736f, 0.949999988079071f);
  style.Colors[ImGuiCol_Tab] = ImVec4(0.1764705926179886f, 0.3490196168422699f, 0.5764706134796143f, 0.8619999885559082f);
  style.Colors[ImGuiCol_TabHovered] = ImVec4(0.2588235437870026f, 0.5882353186607361f, 0.9764705896377563f, 0.800000011920929f);
  style.Colors[ImGuiCol_TabActive] = ImVec4(0.196078434586525f, 0.407843142747879f, 0.6784313917160034f, 1.0f);
  style.Colors[ImGuiCol_TabUnfocused] = ImVec4(0.06666667014360428f, 0.1019607856869698f, 0.1450980454683304f, 0.9724000096321106f);
  style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.1333333402872086f, 0.2588235437870026f, 0.4235294163227081f, 1.0f);
  style.Colors[ImGuiCol_PlotLines] = ImVec4(0.6078431606292725f, 0.6078431606292725f, 0.6078431606292725f, 1.0f);
  style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.0f, 0.4274509847164154f, 0.3490196168422699f, 1.0f);
  style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.729411780834198f, 0.6000000238418579f, 0.1490196138620377f, 1.0f);
  style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.0f, 0.6000000238418579f, 0.0f, 1.0f);
  style.Colors[ImGuiCol_TableHeaderBg] = ImVec4(0.1882352977991104f, 0.1882352977991104f, 0.2000000029802322f, 1.0f);
  style.Colors[ImGuiCol_TableBorderStrong] = ImVec4(0.3098039329051971f, 0.3098039329051971f, 0.3490196168422699f, 1.0f);
  style.Colors[ImGuiCol_TableBorderLight] = ImVec4(0.2274509817361832f, 0.2274509817361832f, 0.2470588237047195f, 1.0f);
  style.Colors[ImGuiCol_TableRowBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
  style.Colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.0f, 1.0f, 1.0f, 0.05999999865889549f);
  style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.8666666746139526f, 0.8666666746139526f, 0.8666666746139526f, 0.3499999940395355f);
  style.Colors[ImGuiCol_DragDropTarget] = ImVec4(1.0f, 1.0f, 0.0f, 0.8999999761581421f);
  style.Colors[ImGuiCol_NavHighlight] = ImVec4(0.6000000238418579f, 0.6000000238418579f, 0.6000000238418579f, 1.0f);
  style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.0f, 1.0f, 1.0f, 0.699999988079071f);
  style.Colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.800000011920929f, 0.800000011920929f, 0.800000011920929f, 0.2000000029802322f);
  style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.800000011920929f, 0.800000011920929f, 0.800000011920929f, 0.3499999940395355f);
}

void SetupImGuiStyle_DeepDark() {
  // Deep Dark style by janekb04 from ImThemes
  ImGuiStyle &style = ImGui::GetStyle();

  style.Alpha = 1.0f;
  style.DisabledAlpha = 0.6000000238418579f;
  style.WindowPadding = ImVec2(8.0f, 8.0f);
  style.WindowRounding = 7.0f;
  style.WindowBorderSize = 1.0f;
  style.WindowMinSize = ImVec2(32.0f, 32.0f);
  style.WindowTitleAlign = ImVec2(0.0f, 0.5f);
  style.WindowMenuButtonPosition = ImGuiDir_Left;
  style.ChildRounding = 4.0f;
  style.ChildBorderSize = 1.0f;
  style.PopupRounding = 4.0f;
  style.PopupBorderSize = 1.0f;
  style.FramePadding = ImVec2(5.0f, 2.0f);
  style.FrameRounding = 3.0f;
  style.FrameBorderSize = 1.0f;
  style.ItemSpacing = ImVec2(6.0f, 6.0f);
  style.ItemInnerSpacing = ImVec2(6.0f, 6.0f);
  style.CellPadding = ImVec2(6.0f, 6.0f);
  style.IndentSpacing = 25.0f;
  style.ColumnsMinSpacing = 6.0f;
  style.ScrollbarSize = 15.0f;
  style.ScrollbarRounding = 9.0f;
  style.GrabMinSize = 10.0f;
  style.GrabRounding = 3.0f;
  style.TabRounding = 4.0f;
  style.TabBorderSize = 1.0f;
  style.TabMinWidthForCloseButton = 0.0f;
  style.ColorButtonPosition = ImGuiDir_Right;
  style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
  style.SelectableTextAlign = ImVec2(0.0f, 0.0f);

  style.Colors[ImGuiCol_Text] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
  style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.4980392158031464f, 0.4980392158031464f, 0.4980392158031464f, 1.0f);
  style.Colors[ImGuiCol_WindowBg] = ImVec4(0.09803921729326248f, 0.09803921729326248f, 0.09803921729326248f, 1.0f);
  style.Colors[ImGuiCol_ChildBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
  style.Colors[ImGuiCol_PopupBg] = ImVec4(0.1882352977991104f, 0.1882352977991104f, 0.1882352977991104f, 0.9200000166893005f);
  style.Colors[ImGuiCol_Border] = ImVec4(0.1882352977991104f, 0.1882352977991104f, 0.1882352977991104f, 0.2899999916553497f);
  style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.0f, 0.0f, 0.0f, 0.239999994635582f);
  style.Colors[ImGuiCol_FrameBg] = ImVec4(0.0470588244497776f, 0.0470588244497776f, 0.0470588244497776f, 0.5400000214576721f);
  style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.1882352977991104f, 0.1882352977991104f, 0.1882352977991104f, 0.5400000214576721f);
  style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.2000000029802322f, 0.2196078449487686f, 0.2274509817361832f, 1.0f);
  style.Colors[ImGuiCol_TitleBg] = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
  style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.05882352963089943f, 0.05882352963089943f, 0.05882352963089943f, 1.0f);
  style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
  style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.1372549086809158f, 0.1372549086809158f, 0.1372549086809158f, 1.0f);
  style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.0470588244497776f, 0.0470588244497776f, 0.0470588244497776f, 0.5400000214576721f);
  style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.3372549116611481f, 0.3372549116611481f, 0.3372549116611481f, 0.5400000214576721f);
  style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.4000000059604645f, 0.4000000059604645f, 0.4000000059604645f, 0.5400000214576721f);
  style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.5568627715110779f, 0.5568627715110779f, 0.5568627715110779f, 0.5400000214576721f);
  style.Colors[ImGuiCol_CheckMark] = ImVec4(0.3294117748737335f, 0.6666666865348816f, 0.8588235378265381f, 1.0f);
  style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.3372549116611481f, 0.3372549116611481f, 0.3372549116611481f, 0.5400000214576721f);
  style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.5568627715110779f, 0.5568627715110779f, 0.5568627715110779f, 0.5400000214576721f);
  style.Colors[ImGuiCol_Button] = ImVec4(0.0470588244497776f, 0.0470588244497776f, 0.0470588244497776f, 0.5400000214576721f);
  style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.1882352977991104f, 0.1882352977991104f, 0.1882352977991104f, 0.5400000214576721f);
  style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.2000000029802322f, 0.2196078449487686f, 0.2274509817361832f, 1.0f);
  style.Colors[ImGuiCol_Header] = ImVec4(0.0f, 0.0f, 0.0f, 0.5199999809265137f);
  style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.0f, 0.0f, 0.0f, 0.3600000143051147f);
  style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.2000000029802322f, 0.2196078449487686f, 0.2274509817361832f, 0.3300000131130219f);
  style.Colors[ImGuiCol_Separator] = ImVec4(0.2784313857555389f, 0.2784313857555389f, 0.2784313857555389f, 0.2899999916553497f);
  style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.4392156898975372f, 0.4392156898975372f, 0.4392156898975372f, 0.2899999916553497f);
  style.Colors[ImGuiCol_SeparatorActive] = ImVec4(0.4000000059604645f, 0.4392156898975372f, 0.4666666686534882f, 1.0f);
  style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.2784313857555389f, 0.2784313857555389f, 0.2784313857555389f, 0.2899999916553497f);
  style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.4392156898975372f, 0.4392156898975372f, 0.4392156898975372f, 0.2899999916553497f);
  style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.4000000059604645f, 0.4392156898975372f, 0.4666666686534882f, 1.0f);
  style.Colors[ImGuiCol_Tab] = ImVec4(0.0f, 0.0f, 0.0f, 0.5199999809265137f);
  style.Colors[ImGuiCol_TabHovered] = ImVec4(0.1372549086809158f, 0.1372549086809158f, 0.1372549086809158f, 1.0f);
  style.Colors[ImGuiCol_TabActive] = ImVec4(0.2000000029802322f, 0.2000000029802322f, 0.2000000029802322f, 0.3600000143051147f);
  style.Colors[ImGuiCol_TabUnfocused] = ImVec4(0.0f, 0.0f, 0.0f, 0.5199999809265137f);
  style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.1372549086809158f, 0.1372549086809158f, 0.1372549086809158f, 1.0f);
  style.Colors[ImGuiCol_PlotLines] = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
  style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
  style.Colors[ImGuiCol_PlotHistogram] = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
  style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
  style.Colors[ImGuiCol_TableHeaderBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.5199999809265137f);
  style.Colors[ImGuiCol_TableBorderStrong] = ImVec4(0.0f, 0.0f, 0.0f, 0.5199999809265137f);
  style.Colors[ImGuiCol_TableBorderLight] = ImVec4(0.2784313857555389f, 0.2784313857555389f, 0.2784313857555389f, 0.2899999916553497f);
  style.Colors[ImGuiCol_TableRowBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
  style.Colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.0f, 1.0f, 1.0f, 0.05999999865889549f);
  style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.2000000029802322f, 0.2196078449487686f, 0.2274509817361832f, 1.0f);
  style.Colors[ImGuiCol_DragDropTarget] = ImVec4(0.3294117748737335f, 0.6666666865348816f, 0.8588235378265381f, 1.0f);
  style.Colors[ImGuiCol_NavHighlight] = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
  style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.0f, 0.0f, 0.0f, 0.699999988079071f);
  style.Colors[ImGuiCol_NavWindowingDimBg] = ImVec4(1.0f, 0.0f, 0.0f, 0.2000000029802322f);
  style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(1.0f, 0.0f, 0.0f, 0.3499999940395355f);
}

void SetupImGuiStyle_DiscordDark() {
  // Discord (Dark) style by BttrDrgn from ImThemes
  ImGuiStyle &style = ImGui::GetStyle();

  style.Alpha = 1.0f;
  style.DisabledAlpha = 0.6000000238418579f;
  style.WindowPadding = ImVec2(8.0f, 8.0f);
  style.WindowRounding = 0.0f;
  style.WindowBorderSize = 1.0f;
  style.WindowMinSize = ImVec2(32.0f, 32.0f);
  style.WindowTitleAlign = ImVec2(0.0f, 0.5f);
  style.WindowMenuButtonPosition = ImGuiDir_Left;
  style.ChildRounding = 0.0f;
  style.ChildBorderSize = 1.0f;
  style.PopupRounding = 0.0f;
  style.PopupBorderSize = 1.0f;
  style.FramePadding = ImVec2(4.0f, 3.0f);
  style.FrameRounding = 0.0f;
  style.FrameBorderSize = 0.0f;
  style.ItemSpacing = ImVec2(8.0f, 4.0f);
  style.ItemInnerSpacing = ImVec2(4.0f, 4.0f);
  style.CellPadding = ImVec2(4.0f, 2.0f);
  style.IndentSpacing = 21.0f;
  style.ColumnsMinSpacing = 6.0f;
  style.ScrollbarSize = 14.0f;
  style.ScrollbarRounding = 0.0f;
  style.GrabMinSize = 10.0f;
  style.GrabRounding = 0.0f;
  style.TabRounding = 0.0f;
  style.TabBorderSize = 0.0f;
  style.TabMinWidthForCloseButton = 0.0f;
  style.ColorButtonPosition = ImGuiDir_Right;
  style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
  style.SelectableTextAlign = ImVec2(0.0f, 0.0f);

  style.Colors[ImGuiCol_Text] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
  style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.4980392158031464f, 0.4980392158031464f, 0.4980392158031464f, 1.0f);
  style.Colors[ImGuiCol_WindowBg] = ImVec4(0.2117647081613541f, 0.2235294133424759f, 0.2470588237047195f, 1.0f);
  style.Colors[ImGuiCol_ChildBg] = ImVec4(0.1843137294054031f, 0.1921568661928177f, 0.2117647081613541f, 1.0f);
  style.Colors[ImGuiCol_PopupBg] = ImVec4(0.0784313753247261f, 0.0784313753247261f, 0.0784313753247261f, 0.9399999976158142f);
  style.Colors[ImGuiCol_Border] = ImVec4(0.4274509847164154f, 0.4274509847164154f, 0.4980392158031464f, 0.5f);
  style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
  style.Colors[ImGuiCol_FrameBg] = ImVec4(0.3098039329051971f, 0.3294117748737335f, 0.3607843220233917f, 1.0f);
  style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.3098039329051971f, 0.3294117748737335f, 0.3607843220233917f, 1.0f);
  style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.3450980484485626f, 0.3960784375667572f, 0.9490196108818054f, 1.0f);
  style.Colors[ImGuiCol_TitleBg] = ImVec4(0.1843137294054031f, 0.1921568661928177f, 0.2117647081613541f, 1.0f);
  style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.125490203499794f, 0.1333333402872086f, 0.1450980454683304f, 1.0f);
  style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.125490203499794f, 0.1333333402872086f, 0.1450980454683304f, 1.0f);
  style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.125490203499794f, 0.1333333402872086f, 0.1450980454683304f, 1.0f);
  style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.01960784383118153f, 0.01960784383118153f, 0.01960784383118153f, 0.5299999713897705f);
  style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.3098039329051971f, 0.3098039329051971f, 0.3098039329051971f, 1.0f);
  style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.407843142747879f, 0.407843142747879f, 0.407843142747879f, 1.0f);
  style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.5098039507865906f, 0.5098039507865906f, 0.5098039507865906f, 1.0f);
  style.Colors[ImGuiCol_CheckMark] = ImVec4(0.2313725501298904f, 0.6470588445663452f, 0.364705890417099f, 1.0f);
  style.Colors[ImGuiCol_SliderGrab] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
  style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
  style.Colors[ImGuiCol_Button] = ImVec4(0.3098039329051971f, 0.3294117748737335f, 0.3607843220233917f, 1.0f);
  style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.407843142747879f, 0.4274509847164154f, 0.4509803950786591f, 1.0f);
  style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.125490203499794f, 0.1333333402872086f, 0.1450980454683304f, 1.0f);
  style.Colors[ImGuiCol_Header] = ImVec4(0.3098039329051971f, 0.3294117748737335f, 0.3607843220233917f, 1.0f);
  style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.407843142747879f, 0.4274509847164154f, 0.4509803950786591f, 1.0f);
  style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.407843142747879f, 0.4274509847164154f, 0.4509803950786591f, 1.0f);
  style.Colors[ImGuiCol_Separator] = ImVec4(0.4274509847164154f, 0.4274509847164154f, 0.4980392158031464f, 0.5f);
  style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.09803921729326248f, 0.4000000059604645f, 0.7490196228027344f, 0.7799999713897705f);
  style.Colors[ImGuiCol_SeparatorActive] = ImVec4(0.09803921729326248f, 0.4000000059604645f, 0.7490196228027344f, 1.0f);
  style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.2588235437870026f, 0.5882353186607361f, 0.9764705896377563f, 0.2000000029802322f);
  style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.2588235437870026f, 0.5882353186607361f, 0.9764705896377563f, 0.6700000166893005f);
  style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.2588235437870026f, 0.5882353186607361f, 0.9764705896377563f, 0.949999988079071f);
  style.Colors[ImGuiCol_Tab] = ImVec4(0.1843137294054031f, 0.1921568661928177f, 0.2117647081613541f, 1.0f);
  style.Colors[ImGuiCol_TabHovered] = ImVec4(0.2352941185235977f, 0.2470588237047195f, 0.2705882489681244f, 1.0f);
  style.Colors[ImGuiCol_TabActive] = ImVec4(0.2588235437870026f, 0.2745098173618317f, 0.3019607961177826f, 1.0f);
  style.Colors[ImGuiCol_TabUnfocused] = ImVec4(0.06666667014360428f, 0.1019607856869698f, 0.1450980454683304f, 0.9724000096321106f);
  style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.1333333402872086f, 0.2588235437870026f, 0.4235294163227081f, 1.0f);
  style.Colors[ImGuiCol_PlotLines] = ImVec4(0.6078431606292725f, 0.6078431606292725f, 0.6078431606292725f, 1.0f);
  style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.3450980484485626f, 0.3960784375667572f, 0.9490196108818054f, 1.0f);
  style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.3450980484485626f, 0.3960784375667572f, 0.9490196108818054f, 1.0f);
  style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.3607843220233917f, 0.4000000059604645f, 0.4274509847164154f, 1.0f);
  style.Colors[ImGuiCol_TableHeaderBg] = ImVec4(0.1882352977991104f, 0.1882352977991104f, 0.2000000029802322f, 1.0f);
  style.Colors[ImGuiCol_TableBorderStrong] = ImVec4(0.3098039329051971f, 0.3098039329051971f, 0.3490196168422699f, 1.0f);
  style.Colors[ImGuiCol_TableBorderLight] = ImVec4(0.2274509817361832f, 0.2274509817361832f, 0.2470588237047195f, 1.0f);
  style.Colors[ImGuiCol_TableRowBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
  style.Colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.0f, 1.0f, 1.0f, 0.05999999865889549f);
  style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.05098039284348488f, 0.4196078479290009f, 0.8588235378265381f, 1.0f);
  style.Colors[ImGuiCol_DragDropTarget] = ImVec4(0.3450980484485626f, 0.3960784375667572f, 0.9490196108818054f, 1.0f);
  style.Colors[ImGuiCol_NavHighlight] = ImVec4(0.2588235437870026f, 0.5882353186607361f, 0.9764705896377563f, 1.0f);
  style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.0f, 1.0f, 1.0f, 0.699999988079071f);
  style.Colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.800000011920929f, 0.800000011920929f, 0.800000011920929f, 0.2000000029802322f);
  style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.800000011920929f, 0.800000011920929f, 0.800000011920929f, 0.3499999940395355f);
}

void SetupImGuiStyle_DarkRuda() {
  // Dark Ruda style by Raikiri from ImThemes
  ImGuiStyle &style = ImGui::GetStyle();

  style.Alpha = 1.0f;
  style.DisabledAlpha = 0.6000000238418579f;
  style.WindowPadding = ImVec2(8.0f, 8.0f);
  style.WindowRounding = 0.0f;
  style.WindowBorderSize = 1.0f;
  style.WindowMinSize = ImVec2(32.0f, 32.0f);
  style.WindowTitleAlign = ImVec2(0.0f, 0.5f);
  style.WindowMenuButtonPosition = ImGuiDir_Left;
  style.ChildRounding = 0.0f;
  style.ChildBorderSize = 1.0f;
  style.PopupRounding = 0.0f;
  style.PopupBorderSize = 1.0f;
  style.FramePadding = ImVec2(4.0f, 3.0f);
  style.FrameRounding = 4.0f;
  style.FrameBorderSize = 0.0f;
  style.ItemSpacing = ImVec2(8.0f, 4.0f);
  style.ItemInnerSpacing = ImVec2(4.0f, 4.0f);
  style.CellPadding = ImVec2(4.0f, 2.0f);
  style.IndentSpacing = 21.0f;
  style.ColumnsMinSpacing = 6.0f;
  style.ScrollbarSize = 14.0f;
  style.ScrollbarRounding = 9.0f;
  style.GrabMinSize = 10.0f;
  style.GrabRounding = 4.0f;
  style.TabRounding = 4.0f;
  style.TabBorderSize = 0.0f;
  style.TabMinWidthForCloseButton = 0.0f;
  style.ColorButtonPosition = ImGuiDir_Right;
  style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
  style.SelectableTextAlign = ImVec2(0.0f, 0.0f);

  style.Colors[ImGuiCol_Text] = ImVec4(0.9490196108818054f, 0.95686274766922f, 0.9764705896377563f, 1.0f);
  style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.3568627536296844f, 0.4196078479290009f, 0.4666666686534882f, 1.0f);
  style.Colors[ImGuiCol_WindowBg] = ImVec4(0.1098039224743843f, 0.1490196138620377f, 0.168627455830574f, 1.0f);
  style.Colors[ImGuiCol_ChildBg] = ImVec4(0.1490196138620377f, 0.1764705926179886f, 0.2196078449487686f, 1.0f);
  style.Colors[ImGuiCol_PopupBg] = ImVec4(0.0784313753247261f, 0.0784313753247261f, 0.0784313753247261f, 0.9399999976158142f);
  style.Colors[ImGuiCol_Border] = ImVec4(0.0784313753247261f, 0.09803921729326248f, 0.1176470592617989f, 1.0f);
  style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
  style.Colors[ImGuiCol_FrameBg] = ImVec4(0.2000000029802322f, 0.2470588237047195f, 0.2862745225429535f, 1.0f);
  style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.1176470592617989f, 0.2000000029802322f, 0.2784313857555389f, 1.0f);
  style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.08627451211214066f, 0.1176470592617989f, 0.1372549086809158f, 1.0f);
  style.Colors[ImGuiCol_TitleBg] = ImVec4(0.08627451211214066f, 0.1176470592617989f, 0.1372549086809158f, 0.6499999761581421f);
  style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.0784313753247261f, 0.09803921729326248f, 0.1176470592617989f, 1.0f);
  style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.0f, 0.0f, 0.0f, 0.5099999904632568f);
  style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.1490196138620377f, 0.1764705926179886f, 0.2196078449487686f, 1.0f);
  style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.01960784383118153f, 0.01960784383118153f, 0.01960784383118153f, 0.3899999856948853f);
  style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.2000000029802322f, 0.2470588237047195f, 0.2862745225429535f, 1.0f);
  style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.1764705926179886f, 0.2196078449487686f, 0.2470588237047195f, 1.0f);
  style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.08627451211214066f, 0.2078431397676468f, 0.3098039329051971f, 1.0f);
  style.Colors[ImGuiCol_CheckMark] = ImVec4(0.2784313857555389f, 0.5568627715110779f, 1.0f, 1.0f);
  style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.2784313857555389f, 0.5568627715110779f, 1.0f, 1.0f);
  style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.3686274588108063f, 0.6078431606292725f, 1.0f, 1.0f);
  style.Colors[ImGuiCol_Button] = ImVec4(0.2000000029802322f, 0.2470588237047195f, 0.2862745225429535f, 1.0f);
  style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.2784313857555389f, 0.5568627715110779f, 1.0f, 1.0f);
  style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.05882352963089943f, 0.529411792755127f, 0.9764705896377563f, 1.0f);
  style.Colors[ImGuiCol_Header] = ImVec4(0.2000000029802322f, 0.2470588237047195f, 0.2862745225429535f, 0.550000011920929f);
  style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.2588235437870026f, 0.5882353186607361f, 0.9764705896377563f, 0.800000011920929f);
  style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.2588235437870026f, 0.5882353186607361f, 0.9764705896377563f, 1.0f);
  style.Colors[ImGuiCol_Separator] = ImVec4(0.2000000029802322f, 0.2470588237047195f, 0.2862745225429535f, 1.0f);
  style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.09803921729326248f, 0.4000000059604645f, 0.7490196228027344f, 0.7799999713897705f);
  style.Colors[ImGuiCol_SeparatorActive] = ImVec4(0.09803921729326248f, 0.4000000059604645f, 0.7490196228027344f, 1.0f);
  style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.2588235437870026f, 0.5882353186607361f, 0.9764705896377563f, 0.25f);
  style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.2588235437870026f, 0.5882353186607361f, 0.9764705896377563f, 0.6700000166893005f);
  style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.2588235437870026f, 0.5882353186607361f, 0.9764705896377563f, 0.949999988079071f);
  style.Colors[ImGuiCol_Tab] = ImVec4(0.1098039224743843f, 0.1490196138620377f, 0.168627455830574f, 1.0f);
  style.Colors[ImGuiCol_TabHovered] = ImVec4(0.2588235437870026f, 0.5882353186607361f, 0.9764705896377563f, 0.800000011920929f);
  style.Colors[ImGuiCol_TabActive] = ImVec4(0.2000000029802322f, 0.2470588237047195f, 0.2862745225429535f, 1.0f);
  style.Colors[ImGuiCol_TabUnfocused] = ImVec4(0.1098039224743843f, 0.1490196138620377f, 0.168627455830574f, 1.0f);
  style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.1098039224743843f, 0.1490196138620377f, 0.168627455830574f, 1.0f);
  style.Colors[ImGuiCol_PlotLines] = ImVec4(0.6078431606292725f, 0.6078431606292725f, 0.6078431606292725f, 1.0f);
  style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.0f, 0.4274509847164154f, 0.3490196168422699f, 1.0f);
  style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.8980392217636108f, 0.6980392336845398f, 0.0f, 1.0f);
  style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.0f, 0.6000000238418579f, 0.0f, 1.0f);
  style.Colors[ImGuiCol_TableHeaderBg] = ImVec4(0.1882352977991104f, 0.1882352977991104f, 0.2000000029802322f, 1.0f);
  style.Colors[ImGuiCol_TableBorderStrong] = ImVec4(0.3098039329051971f, 0.3098039329051971f, 0.3490196168422699f, 1.0f);
  style.Colors[ImGuiCol_TableBorderLight] = ImVec4(0.2274509817361832f, 0.2274509817361832f, 0.2470588237047195f, 1.0f);
  style.Colors[ImGuiCol_TableRowBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
  style.Colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.0f, 1.0f, 1.0f, 0.05999999865889549f);
  style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.2588235437870026f, 0.5882353186607361f, 0.9764705896377563f, 0.3499999940395355f);
  style.Colors[ImGuiCol_DragDropTarget] = ImVec4(1.0f, 1.0f, 0.0f, 0.8999999761581421f);
  style.Colors[ImGuiCol_NavHighlight] = ImVec4(0.2588235437870026f, 0.5882353186607361f, 0.9764705896377563f, 1.0f);
  style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.0f, 1.0f, 1.0f, 0.699999988079071f);
  style.Colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.800000011920929f, 0.800000011920929f, 0.800000011920929f, 0.2000000029802322f);
  style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.800000011920929f, 0.800000011920929f, 0.800000011920929f, 0.3499999940395355f);
}

void SetupImGuiStyle_Darcula() {
  // Darcula style by ice1000 from ImThemes
  ImGuiStyle &style = ImGui::GetStyle();

  style.Alpha = 1.0f;
  style.DisabledAlpha = 0.6000000238418579f;
  style.WindowPadding = ImVec2(8.0f, 8.0f);
  style.WindowRounding = 5.300000190734863f;
  style.WindowBorderSize = 1.0f;
  style.WindowMinSize = ImVec2(32.0f, 32.0f);
  style.WindowTitleAlign = ImVec2(0.0f, 0.5f);
  style.WindowMenuButtonPosition = ImGuiDir_Left;
  style.ChildRounding = 0.0f;
  style.ChildBorderSize = 1.0f;
  style.PopupRounding = 0.0f;
  style.PopupBorderSize = 1.0f;
  style.FramePadding = ImVec2(4.0f, 3.0f);
  style.FrameRounding = 2.299999952316284f;
  style.FrameBorderSize = 1.0f;
  style.ItemSpacing = ImVec2(8.0f, 6.5f);
  style.ItemInnerSpacing = ImVec2(4.0f, 4.0f);
  style.CellPadding = ImVec2(4.0f, 2.0f);
  style.IndentSpacing = 21.0f;
  style.ColumnsMinSpacing = 6.0f;
  style.ScrollbarSize = 14.0f;
  style.ScrollbarRounding = 5.0f;
  style.GrabMinSize = 10.0f;
  style.GrabRounding = 2.299999952316284f;
  style.TabRounding = 4.0f;
  style.TabBorderSize = 0.0f;
  style.TabMinWidthForCloseButton = 0.0f;
  style.ColorButtonPosition = ImGuiDir_Right;
  style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
  style.SelectableTextAlign = ImVec2(0.0f, 0.0f);

  style.Colors[ImGuiCol_Text] = ImVec4(0.7333333492279053f, 0.7333333492279053f, 0.7333333492279053f, 1.0f);
  style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.3450980484485626f, 0.3450980484485626f, 0.3450980484485626f, 1.0f);
  style.Colors[ImGuiCol_WindowBg] = ImVec4(0.2352941185235977f, 0.2470588237047195f, 0.2549019753932953f, 0.9399999976158142f);
  style.Colors[ImGuiCol_ChildBg] = ImVec4(0.2352941185235977f, 0.2470588237047195f, 0.2549019753932953f, 0.0f);
  style.Colors[ImGuiCol_PopupBg] = ImVec4(0.2352941185235977f, 0.2470588237047195f, 0.2549019753932953f, 0.9399999976158142f);
  style.Colors[ImGuiCol_Border] = ImVec4(0.3333333432674408f, 0.3333333432674408f, 0.3333333432674408f, 0.5f);
  style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.1568627506494522f, 0.1568627506494522f, 0.1568627506494522f, 0.0f);
  style.Colors[ImGuiCol_FrameBg] = ImVec4(0.168627455830574f, 0.168627455830574f, 0.168627455830574f, 0.5400000214576721f);
  style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.4509803950786591f, 0.6745098233222961f, 0.9960784316062927f, 0.6700000166893005f);
  style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.4705882370471954f, 0.4705882370471954f, 0.4705882370471954f, 0.6700000166893005f);
  style.Colors[ImGuiCol_TitleBg] = ImVec4(0.03921568766236305f, 0.03921568766236305f, 0.03921568766236305f, 1.0f);
  style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.0f, 0.0f, 0.0f, 0.5099999904632568f);
  style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.1568627506494522f, 0.2862745225429535f, 0.47843137383461f, 1.0f);
  style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.2705882489681244f, 0.2862745225429535f, 0.2901960909366608f, 0.800000011920929f);
  style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.2705882489681244f, 0.2862745225429535f, 0.2901960909366608f, 0.6000000238418579f);
  style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.2196078449487686f, 0.3098039329051971f, 0.4196078479290009f, 0.5099999904632568f);
  style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.2196078449487686f, 0.3098039329051971f, 0.4196078479290009f, 1.0f);
  style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.1372549086809158f, 0.1921568661928177f, 0.2627451121807098f, 0.9100000262260437f);
  style.Colors[ImGuiCol_CheckMark] = ImVec4(0.8980392217636108f, 0.8980392217636108f, 0.8980392217636108f, 0.8299999833106995f);
  style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.6980392336845398f, 0.6980392336845398f, 0.6980392336845398f, 0.6200000047683716f);
  style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.2980392277240753f, 0.2980392277240753f, 0.2980392277240753f, 0.8399999737739563f);
  style.Colors[ImGuiCol_Button] = ImVec4(0.3333333432674408f, 0.3529411852359772f, 0.3607843220233917f, 0.4900000095367432f);
  style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.2196078449487686f, 0.3098039329051971f, 0.4196078479290009f, 1.0f);
  style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.1372549086809158f, 0.1921568661928177f, 0.2627451121807098f, 1.0f);
  style.Colors[ImGuiCol_Header] = ImVec4(0.3333333432674408f, 0.3529411852359772f, 0.3607843220233917f, 0.5299999713897705f);
  style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.4509803950786591f, 0.6745098233222961f, 0.9960784316062927f, 0.6700000166893005f);
  style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.4705882370471954f, 0.4705882370471954f, 0.4705882370471954f, 0.6700000166893005f);
  style.Colors[ImGuiCol_Separator] = ImVec4(0.3137255012989044f, 0.3137255012989044f, 0.3137255012989044f, 1.0f);
  style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.3137255012989044f, 0.3137255012989044f, 0.3137255012989044f, 1.0f);
  style.Colors[ImGuiCol_SeparatorActive] = ImVec4(0.3137255012989044f, 0.3137255012989044f, 0.3137255012989044f, 1.0f);
  style.Colors[ImGuiCol_ResizeGrip] = ImVec4(1.0f, 1.0f, 1.0f, 0.8500000238418579f);
  style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(1.0f, 1.0f, 1.0f, 0.6000000238418579f);
  style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(1.0f, 1.0f, 1.0f, 0.8999999761581421f);
  style.Colors[ImGuiCol_Tab] = ImVec4(0.1764705926179886f, 0.3490196168422699f, 0.5764706134796143f, 0.8619999885559082f);
  style.Colors[ImGuiCol_TabHovered] = ImVec4(0.2588235437870026f, 0.5882353186607361f, 0.9764705896377563f, 0.800000011920929f);
  style.Colors[ImGuiCol_TabActive] = ImVec4(0.196078434586525f, 0.407843142747879f, 0.6784313917160034f, 1.0f);
  style.Colors[ImGuiCol_TabUnfocused] = ImVec4(0.06666667014360428f, 0.1019607856869698f, 0.1450980454683304f, 0.9724000096321106f);
  style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.1333333402872086f, 0.2588235437870026f, 0.4235294163227081f, 1.0f);
  style.Colors[ImGuiCol_PlotLines] = ImVec4(0.6078431606292725f, 0.6078431606292725f, 0.6078431606292725f, 1.0f);
  style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.0f, 0.4274509847164154f, 0.3490196168422699f, 1.0f);
  style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.8980392217636108f, 0.6980392336845398f, 0.0f, 1.0f);
  style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.0f, 0.6000000238418579f, 0.0f, 1.0f);
  style.Colors[ImGuiCol_TableHeaderBg] = ImVec4(0.1882352977991104f, 0.1882352977991104f, 0.2000000029802322f, 1.0f);
  style.Colors[ImGuiCol_TableBorderStrong] = ImVec4(0.3098039329051971f, 0.3098039329051971f, 0.3490196168422699f, 1.0f);
  style.Colors[ImGuiCol_TableBorderLight] = ImVec4(0.2274509817361832f, 0.2274509817361832f, 0.2470588237047195f, 1.0f);
  style.Colors[ImGuiCol_TableRowBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
  style.Colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.0f, 1.0f, 1.0f, 0.05999999865889549f);
  style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.1843137294054031f, 0.3960784375667572f, 0.7921568751335144f, 0.8999999761581421f);
  style.Colors[ImGuiCol_DragDropTarget] = ImVec4(1.0f, 1.0f, 0.0f, 0.8999999761581421f);
  style.Colors[ImGuiCol_NavHighlight] = ImVec4(0.2588235437870026f, 0.5882353186607361f, 0.9764705896377563f, 1.0f);
  style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.0f, 1.0f, 1.0f, 0.699999988079071f);
  style.Colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.800000011920929f, 0.800000011920929f, 0.800000011920929f, 0.2000000029802322f);
  style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.800000011920929f, 0.800000011920929f, 0.800000011920929f, 0.3499999940395355f);
}

void SetupImGuiStyle_RedFont() {
  // Red Font style by aiekick from ImThemes
  ImGuiStyle &style = ImGui::GetStyle();

  style.Alpha = 1.0f;
  style.DisabledAlpha = 0.6000000238418579f;
  style.WindowPadding = ImVec2(8.0f, 8.0f);
  style.WindowRounding = 0.0f;
  style.WindowBorderSize = 1.0f;
  style.WindowMinSize = ImVec2(32.0f, 32.0f);
  style.WindowTitleAlign = ImVec2(0.0f, 0.5f);
  style.WindowMenuButtonPosition = ImGuiDir_Left;
  style.ChildRounding = 0.0f;
  style.ChildBorderSize = 1.0f;
  style.PopupRounding = 0.0f;
  style.PopupBorderSize = 1.0f;
  style.FramePadding = ImVec2(4.0f, 3.0f);
  style.FrameRounding = 0.0f;
  style.FrameBorderSize = 0.0f;
  style.ItemSpacing = ImVec2(8.0f, 4.0f);
  style.ItemInnerSpacing = ImVec2(4.0f, 4.0f);
  style.CellPadding = ImVec2(4.0f, 2.0f);
  style.IndentSpacing = 21.0f;
  style.ColumnsMinSpacing = 6.0f;
  style.ScrollbarSize = 14.0f;
  style.ScrollbarRounding = 9.0f;
  style.GrabMinSize = 10.0f;
  style.GrabRounding = 0.0f;
  style.TabRounding = 4.0f;
  style.TabBorderSize = 0.0f;
  style.TabMinWidthForCloseButton = 0.0f;
  style.ColorButtonPosition = ImGuiDir_Right;
  style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
  style.SelectableTextAlign = ImVec2(0.0f, 0.0f);

  style.Colors[ImGuiCol_Text] = ImVec4(0.7490196228027344f, 0.7490196228027344f, 0.7490196228027344f, 1.0f);
  style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.3490196168422699f, 0.3490196168422699f, 0.3490196168422699f, 1.0f);
  style.Colors[ImGuiCol_WindowBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.9399999976158142f);
  style.Colors[ImGuiCol_ChildBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
  style.Colors[ImGuiCol_PopupBg] = ImVec4(0.0784313753247261f, 0.0784313753247261f, 0.0784313753247261f, 0.9399999976158142f);
  style.Colors[ImGuiCol_Border] = ImVec4(0.0f, 0.0f, 0.0f, 0.5f);
  style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
  style.Colors[ImGuiCol_FrameBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.5400000214576721f);
  style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.3686274588108063f, 0.1372549086809158f, 0.1372549086809158f, 0.6700000166893005f);
  style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.3882353007793427f, 0.2000000029802322f, 0.2000000029802322f, 0.6700000166893005f);
  style.Colors[ImGuiCol_TitleBg] = ImVec4(0.03921568766236305f, 0.03921568766236305f, 0.03921568766236305f, 1.0f);
  style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.47843137383461f, 0.1568627506494522f, 0.1568627506494522f, 1.0f);
  style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.47843137383461f, 0.1568627506494522f, 0.1568627506494522f, 1.0f);
  style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.1372549086809158f, 0.1372549086809158f, 0.1372549086809158f, 1.0f);
  style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.01960784383118153f, 0.01960784383118153f, 0.01960784383118153f, 0.5299999713897705f);
  style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.3098039329051971f, 0.3098039329051971f, 0.3098039329051971f, 1.0f);
  style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.407843142747879f, 0.407843142747879f, 0.407843142747879f, 1.0f);
  style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.5098039507865906f, 0.5098039507865906f, 0.5098039507865906f, 1.0f);
  style.Colors[ImGuiCol_CheckMark] = ImVec4(0.5568627715110779f, 0.09803921729326248f, 0.09803921729326248f, 1.0f);
  style.Colors[ImGuiCol_SliderGrab] = ImVec4(1.0f, 0.1882352977991104f, 0.1882352977991104f, 0.4000000059604645f);
  style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.886274516582489f, 0.0f, 0.1882352977991104f, 1.0f);
  style.Colors[ImGuiCol_Button] = ImVec4(1.0f, 0.1882352977991104f, 0.1882352977991104f, 0.4000000059604645f);
  style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.800000011920929f, 0.168627455830574f, 0.0f, 1.0f);
  style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.886274516582489f, 0.0f, 0.1882352977991104f, 1.0f);
  style.Colors[ImGuiCol_Header] = ImVec4(0.3294117748737335f, 0.3490196168422699f, 0.3568627536296844f, 0.5299999713897705f);
  style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.7568627595901489f, 0.2784313857555389f, 0.4392156898975372f, 0.6700000166893005f);
  style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.4666666686534882f, 0.4666666686534882f, 0.4666666686534882f, 0.6700000166893005f);
  style.Colors[ImGuiCol_Separator] = ImVec4(0.3176470696926117f, 0.3176470696926117f, 0.3176470696926117f, 1.0f);
  style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.3176470696926117f, 0.3176470696926117f, 0.3176470696926117f, 1.0f);
  style.Colors[ImGuiCol_SeparatorActive] = ImVec4(0.3176470696926117f, 0.3176470696926117f, 0.3176470696926117f, 1.0f);
  style.Colors[ImGuiCol_ResizeGrip] = ImVec4(1.0f, 1.0f, 1.0f, 0.8500000238418579f);
  style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(1.0f, 1.0f, 1.0f, 0.6000000238418579f);
  style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(1.0f, 1.0f, 1.0f, 0.8999999761581421f);
  style.Colors[ImGuiCol_Tab] = ImVec4(0.06666667014360428f, 0.06666667014360428f, 0.06666667014360428f, 0.5099999904632568f);
  style.Colors[ImGuiCol_TabHovered] = ImVec4(0.8588235378265381f, 0.2274509817361832f, 0.4274509847164154f, 0.6700000166893005f);
  style.Colors[ImGuiCol_TabActive] = ImVec4(0.1882352977991104f, 0.1882352977991104f, 0.1882352977991104f, 0.5699999928474426f);
  style.Colors[ImGuiCol_TabUnfocused] = ImVec4(0.0470588244497776f, 0.0470588244497776f, 0.0470588244497776f, 0.8999999761581421f);
  style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.1294117718935013f, 0.1294117718935013f, 0.1294117718935013f, 0.7400000095367432f);
  style.Colors[ImGuiCol_PlotLines] = ImVec4(0.6078431606292725f, 0.6078431606292725f, 0.6078431606292725f, 1.0f);
  style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.0f, 0.4274509847164154f, 0.3490196168422699f, 1.0f);
  style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.8980392217636108f, 0.6980392336845398f, 0.0f, 1.0f);
  style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.0f, 0.6000000238418579f, 0.0f, 1.0f);
  style.Colors[ImGuiCol_TableHeaderBg] = ImVec4(0.1882352977991104f, 0.1882352977991104f, 0.2000000029802322f, 1.0f);
  style.Colors[ImGuiCol_TableBorderStrong] = ImVec4(0.3098039329051971f, 0.3098039329051971f, 0.3490196168422699f, 1.0f);
  style.Colors[ImGuiCol_TableBorderLight] = ImVec4(0.2274509817361832f, 0.2274509817361832f, 0.2470588237047195f, 1.0f);
  style.Colors[ImGuiCol_TableRowBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
  style.Colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.0f, 1.0f, 1.0f, 0.07000000029802322f);
  style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.2588235437870026f, 0.5882353186607361f, 0.9764705896377563f, 0.3499999940395355f);
  style.Colors[ImGuiCol_DragDropTarget] = ImVec4(1.0f, 1.0f, 0.0f, 0.8999999761581421f);
  style.Colors[ImGuiCol_NavHighlight] = ImVec4(0.2588235437870026f, 0.5882353186607361f, 0.9764705896377563f, 1.0f);
  style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.0f, 1.0f, 1.0f, 0.699999988079071f);
  style.Colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.800000011920929f, 0.800000011920929f, 0.800000011920929f, 0.2000000029802322f);
  style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.800000011920929f, 0.800000011920929f, 0.800000011920929f, 0.3499999940395355f);
}

void SetupImGuiStyle_MaterialFlat() {
  // Material Flat style by ImJC1C from ImThemes
  ImGuiStyle &style = ImGui::GetStyle();

  style.Alpha = 1.0f;
  style.DisabledAlpha = 0.5f;
  style.WindowPadding = ImVec2(8.0f, 8.0f);
  style.WindowRounding = 0.0f;
  style.WindowBorderSize = 1.0f;
  style.WindowMinSize = ImVec2(32.0f, 32.0f);
  style.WindowTitleAlign = ImVec2(0.0f, 0.5f);
  style.WindowMenuButtonPosition = ImGuiDir_Left;
  style.ChildRounding = 0.0f;
  style.ChildBorderSize = 1.0f;
  style.PopupRounding = 0.0f;
  style.PopupBorderSize = 1.0f;
  style.FramePadding = ImVec2(4.0f, 3.0f);
  style.FrameRounding = 0.0f;
  style.FrameBorderSize = 0.0f;
  style.ItemSpacing = ImVec2(8.0f, 4.0f);
  style.ItemInnerSpacing = ImVec2(4.0f, 4.0f);
  style.CellPadding = ImVec2(4.0f, 2.0f);
  style.IndentSpacing = 21.0f;
  style.ColumnsMinSpacing = 6.0f;
  style.ScrollbarSize = 14.0f;
  style.ScrollbarRounding = 0.0f;
  style.GrabMinSize = 10.0f;
  style.GrabRounding = 0.0f;
  style.TabRounding = 0.0f;
  style.TabBorderSize = 0.0f;
  style.TabMinWidthForCloseButton = 0.0f;
  style.ColorButtonPosition = ImGuiDir_Left;
  style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
  style.SelectableTextAlign = ImVec2(0.0f, 0.0f);

  style.Colors[ImGuiCol_Text] = ImVec4(0.8313725590705872f, 0.8470588326454163f, 0.8784313797950745f, 1.0f);
  style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.8313725590705872f, 0.8470588326454163f, 0.8784313797950745f, 0.501960813999176f);
  style.Colors[ImGuiCol_WindowBg] = ImVec4(0.1725490242242813f, 0.1921568661928177f, 0.2352941185235977f, 1.0f);
  style.Colors[ImGuiCol_ChildBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.1587982773780823f);
  style.Colors[ImGuiCol_PopupBg] = ImVec4(0.1725490242242813f, 0.1921568661928177f, 0.2352941185235977f, 1.0f);
  style.Colors[ImGuiCol_Border] = ImVec4(0.2039215713739395f, 0.2313725501298904f, 0.2823529541492462f, 1.0f);
  style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
  style.Colors[ImGuiCol_FrameBg] = ImVec4(0.105882354080677f, 0.1137254908680916f, 0.1372549086809158f, 0.501960813999176f);
  style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.3098039329051971f, 0.6235294342041016f, 0.9333333373069763f, 0.250980406999588f);
  style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.3098039329051971f, 0.6235294342041016f, 0.9333333373069763f, 1.0f);
  style.Colors[ImGuiCol_TitleBg] = ImVec4(0.105882354080677f, 0.1137254908680916f, 0.1372549086809158f, 1.0f);
  style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.105882354080677f, 0.1137254908680916f, 0.1372549086809158f, 1.0f);
  style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.105882354080677f, 0.1137254908680916f, 0.1372549086809158f, 1.0f);
  style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.105882354080677f, 0.1137254908680916f, 0.1372549086809158f, 1.0f);
  style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.01960784383118153f, 0.01960784383118153f, 0.01960784383118153f, 0.0f);
  style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.5333333611488342f, 0.5333333611488342f, 0.5333333611488342f, 1.0f);
  style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.3333333432674408f, 0.3333333432674408f, 0.3333333432674408f, 1.0f);
  style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.6000000238418579f, 0.6000000238418579f, 0.6000000238418579f, 1.0f);
  style.Colors[ImGuiCol_CheckMark] = ImVec4(0.3098039329051971f, 0.6235294342041016f, 0.9333333373069763f, 1.0f);
  style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.239215686917305f, 0.5215686559677124f, 0.8784313797950745f, 1.0f);
  style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.2588235437870026f, 0.5882353186607361f, 0.9803921580314636f, 1.0f);
  style.Colors[ImGuiCol_Button] = ImVec4(0.1529411822557449f, 0.1725490242242813f, 0.2117647081613541f, 0.501960813999176f);
  style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.1529411822557449f, 0.1725490242242813f, 0.2117647081613541f, 1.0f);
  style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.3098039329051971f, 0.6235294342041016f, 0.9333333373069763f, 1.0f);
  style.Colors[ImGuiCol_Header] = ImVec4(0.1529411822557449f, 0.1725490242242813f, 0.2117647081613541f, 1.0f);
  style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.3098039329051971f, 0.6235294342041016f, 0.9333333373069763f, 0.250980406999588f);
  style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.3098039329051971f, 0.6235294342041016f, 0.9333333373069763f, 1.0f);
  style.Colors[ImGuiCol_Separator] = ImVec4(0.4274509847164154f, 0.4274509847164154f, 0.4980392158031464f, 0.5f);
  style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.09803921729326248f, 0.4000000059604645f, 0.7490196228027344f, 0.7799999713897705f);
  style.Colors[ImGuiCol_SeparatorActive] = ImVec4(0.09803921729326248f, 0.4000000059604645f, 0.7490196228027344f, 1.0f);
  style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.105882354080677f, 0.1137254908680916f, 0.1372549086809158f, 1.0f);
  style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.3098039329051971f, 0.6235294342041016f, 0.9333333373069763f, 0.250980406999588f);
  style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.3098039329051971f, 0.6235294342041016f, 0.9333333373069763f, 1.0f);
  style.Colors[ImGuiCol_Tab] = ImVec4(0.1529411822557449f, 0.1725490242242813f, 0.2117647081613541f, 1.0f);
  style.Colors[ImGuiCol_TabHovered] = ImVec4(0.3098039329051971f, 0.6235294342041016f, 0.9333333373069763f, 0.250980406999588f);
  style.Colors[ImGuiCol_TabActive] = ImVec4(0.3098039329051971f, 0.6235294342041016f, 0.9333333373069763f, 1.0f);
  style.Colors[ImGuiCol_TabUnfocused] = ImVec4(0.1529411822557449f, 0.1725490242242813f, 0.2117647081613541f, 1.0f);
  style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.3098039329051971f, 0.6235294342041016f, 0.9333333373069763f, 1.0f);
  style.Colors[ImGuiCol_PlotLines] = ImVec4(0.6078431606292725f, 0.6078431606292725f, 0.6078431606292725f, 1.0f);
  style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.0f, 0.4274509847164154f, 0.3490196168422699f, 1.0f);
  style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.8980392217636108f, 0.6980392336845398f, 0.0f, 1.0f);
  style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.0f, 0.6000000238418579f, 0.0f, 1.0f);
  style.Colors[ImGuiCol_TableHeaderBg] = ImVec4(0.105882354080677f, 0.1137254908680916f, 0.1372549086809158f, 1.0f);
  style.Colors[ImGuiCol_TableBorderStrong] = ImVec4(0.2039215713739395f, 0.2313725501298904f, 0.2823529541492462f, 1.0f);
  style.Colors[ImGuiCol_TableBorderLight] = ImVec4(0.2039215713739395f, 0.2313725501298904f, 0.2823529541492462f, 0.5021458864212036f);
  style.Colors[ImGuiCol_TableRowBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
  style.Colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.0f, 1.0f, 1.0f, 0.03862661123275757f);
  style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.2039215713739395f, 0.2313725501298904f, 0.2823529541492462f, 1.0f);
  style.Colors[ImGuiCol_DragDropTarget] = ImVec4(1.0f, 1.0f, 0.0f, 0.8999999761581421f);
  style.Colors[ImGuiCol_NavHighlight] = ImVec4(0.2588235437870026f, 0.5882353186607361f, 0.9764705896377563f, 1.0f);
  style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(0.2039215713739395f, 0.2313725501298904f, 0.2823529541492462f, 0.7529411911964417f);
  style.Colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.105882354080677f, 0.1137254908680916f, 0.1372549086809158f, 0.7529411911964417f);
  style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.105882354080677f, 0.1137254908680916f, 0.1372549086809158f, 0.7529411911964417f);
}

void SetupImGuiStyle_GreenFont() {
  // Green Font style by aiekick from ImThemes
  ImGuiStyle &style = ImGui::GetStyle();

  style.Alpha = 1.0f;
  style.DisabledAlpha = 0.6000000238418579f;
  style.WindowPadding = ImVec2(8.0f, 8.0f);
  style.WindowRounding = 0.0f;
  style.WindowBorderSize = 1.0f;
  style.WindowMinSize = ImVec2(32.0f, 32.0f);
  style.WindowTitleAlign = ImVec2(0.0f, 0.5f);
  style.WindowMenuButtonPosition = ImGuiDir_Left;
  style.ChildRounding = 0.0f;
  style.ChildBorderSize = 1.0f;
  style.PopupRounding = 0.0f;
  style.PopupBorderSize = 1.0f;
  style.FramePadding = ImVec2(4.0f, 3.0f);
  style.FrameRounding = 0.0f;
  style.FrameBorderSize = 0.0f;
  style.ItemSpacing = ImVec2(8.0f, 4.0f);
  style.ItemInnerSpacing = ImVec2(4.0f, 4.0f);
  style.CellPadding = ImVec2(4.0f, 2.0f);
  style.IndentSpacing = 21.0f;
  style.ColumnsMinSpacing = 6.0f;
  style.ScrollbarSize = 14.0f;
  style.ScrollbarRounding = 9.0f;
  style.GrabMinSize = 10.0f;
  style.GrabRounding = 0.0f;
  style.TabRounding = 4.0f;
  style.TabBorderSize = 0.0f;
  style.TabMinWidthForCloseButton = 0.0f;
  style.ColorButtonPosition = ImGuiDir_Right;
  style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
  style.SelectableTextAlign = ImVec2(0.0f, 0.0f);

  style.Colors[ImGuiCol_Text] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
  style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.4980392158031464f, 0.4980392158031464f, 0.4980392158031464f, 1.0f);
  style.Colors[ImGuiCol_WindowBg] = ImVec4(0.05882352963089943f, 0.05882352963089943f, 0.05882352963089943f, 0.9399999976158142f);
  style.Colors[ImGuiCol_ChildBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
  style.Colors[ImGuiCol_PopupBg] = ImVec4(0.0784313753247261f, 0.0784313753247261f, 0.0784313753247261f, 0.9399999976158142f);
  style.Colors[ImGuiCol_Border] = ImVec4(0.4274509847164154f, 0.4274509847164154f, 0.4980392158031464f, 0.5f);
  style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
  style.Colors[ImGuiCol_FrameBg] = ImVec4(0.4392156898975372f, 0.4392156898975372f, 0.4392156898975372f, 0.6000000238418579f);
  style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.5686274766921997f, 0.5686274766921997f, 0.5686274766921997f, 0.699999988079071f);
  style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.7568627595901489f, 0.7568627595901489f, 0.7568627595901489f, 0.800000011920929f);
  style.Colors[ImGuiCol_TitleBg] = ImVec4(0.03921568766236305f, 0.03921568766236305f, 0.03921568766236305f, 1.0f);
  style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.1568627506494522f, 0.1568627506494522f, 0.1568627506494522f, 1.0f);
  style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.0f, 0.0f, 0.0f, 0.6000000238418579f);
  style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.1372549086809158f, 0.1372549086809158f, 0.1372549086809158f, 1.0f);
  style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.01960784383118153f, 0.01960784383118153f, 0.01960784383118153f, 0.5299999713897705f);
  style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.3098039329051971f, 0.3098039329051971f, 0.3098039329051971f, 1.0f);
  style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.407843142747879f, 0.407843142747879f, 0.407843142747879f, 1.0f);
  style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.5098039507865906f, 0.5098039507865906f, 0.5098039507865906f, 1.0f);
  style.Colors[ImGuiCol_CheckMark] = ImVec4(0.1294117718935013f, 0.7490196228027344f, 0.5490196347236633f, 0.800000011920929f);
  style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.1294117718935013f, 0.7490196228027344f, 0.7490196228027344f, 0.800000011920929f);
  style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.1294117718935013f, 0.7490196228027344f, 1.0f, 0.800000011920929f);
  style.Colors[ImGuiCol_Button] = ImVec4(0.1294117718935013f, 0.7490196228027344f, 0.5490196347236633f, 0.4000000059604645f);
  style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.1294117718935013f, 0.7490196228027344f, 0.7490196228027344f, 0.6000000238418579f);
  style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.1294117718935013f, 0.7490196228027344f, 1.0f, 0.800000011920929f);
  style.Colors[ImGuiCol_Header] = ImVec4(0.1294117718935013f, 0.7490196228027344f, 0.5490196347236633f, 0.4000000059604645f);
  style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.1294117718935013f, 0.7490196228027344f, 0.7490196228027344f, 0.6000000238418579f);
  style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.1294117718935013f, 0.7490196228027344f, 1.0f, 0.800000011920929f);
  style.Colors[ImGuiCol_Separator] = ImVec4(0.1294117718935013f, 0.7490196228027344f, 0.5490196347236633f, 0.4000000059604645f);
  style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.1294117718935013f, 0.7490196228027344f, 0.7490196228027344f, 0.6000000238418579f);
  style.Colors[ImGuiCol_SeparatorActive] = ImVec4(0.1294117718935013f, 0.7490196228027344f, 1.0f, 0.800000011920929f);
  style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.1294117718935013f, 0.7490196228027344f, 0.5490196347236633f, 0.4000000059604645f);
  style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.1294117718935013f, 0.7490196228027344f, 0.7490196228027344f, 0.6000000238418579f);
  style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.1294117718935013f, 0.7490196228027344f, 1.0f, 0.800000011920929f);
  style.Colors[ImGuiCol_Tab] = ImVec4(0.1294117718935013f, 0.7490196228027344f, 0.5490196347236633f, 0.800000011920929f);
  style.Colors[ImGuiCol_TabHovered] = ImVec4(0.1294117718935013f, 0.7490196228027344f, 0.7490196228027344f, 0.800000011920929f);
  style.Colors[ImGuiCol_TabActive] = ImVec4(0.1294117718935013f, 0.7490196228027344f, 1.0f, 0.800000011920929f);
  style.Colors[ImGuiCol_TabUnfocused] = ImVec4(0.1764705926179886f, 0.1764705926179886f, 0.1764705926179886f, 1.0f);
  style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.3568627536296844f, 0.3568627536296844f, 0.3568627536296844f, 0.5400000214576721f);
  style.Colors[ImGuiCol_PlotLines] = ImVec4(0.6078431606292725f, 0.6078431606292725f, 0.6078431606292725f, 1.0f);
  style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.0f, 0.4274509847164154f, 0.3490196168422699f, 1.0f);
  style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.8980392217636108f, 0.6980392336845398f, 0.0f, 1.0f);
  style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.0f, 0.6000000238418579f, 0.0f, 1.0f);
  style.Colors[ImGuiCol_TableHeaderBg] = ImVec4(0.1882352977991104f, 0.1882352977991104f, 0.2000000029802322f, 1.0f);
  style.Colors[ImGuiCol_TableBorderStrong] = ImVec4(0.3098039329051971f, 0.3098039329051971f, 0.3490196168422699f, 1.0f);
  style.Colors[ImGuiCol_TableBorderLight] = ImVec4(0.2274509817361832f, 0.2274509817361832f, 0.2470588237047195f, 1.0f);
  style.Colors[ImGuiCol_TableRowBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
  style.Colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.0f, 1.0f, 1.0f, 0.07000000029802322f);
  style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.2588235437870026f, 0.5882353186607361f, 0.9764705896377563f, 0.3499999940395355f);
  style.Colors[ImGuiCol_DragDropTarget] = ImVec4(1.0f, 1.0f, 0.0f, 0.8999999761581421f);
  style.Colors[ImGuiCol_NavHighlight] = ImVec4(0.2588235437870026f, 0.5882353186607361f, 0.9764705896377563f, 1.0f);
  style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.0f, 1.0f, 1.0f, 0.699999988079071f);
  style.Colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.800000011920929f, 0.800000011920929f, 0.800000011920929f, 0.2000000029802322f);
  style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.800000011920929f, 0.800000011920929f, 0.800000011920929f, 0.3499999940395355f);
}

void SetupImGuiStyle_DarkCustom() {
  // https://www.unknowncheats.me/forum/direct3d/189635-imgui-style-settings.html
  ImGuiStyle &style = ImGui::GetStyle();

  style.WindowPadding = ImVec2(15, 15);
  style.WindowRounding = 5.0f;
  style.FramePadding = ImVec2(5, 5);
  style.FrameRounding = 4.0f;
  style.ItemSpacing = ImVec2(12, 8);
  style.ItemInnerSpacing = ImVec2(8, 6);
  style.IndentSpacing = 25.0f;
  style.ScrollbarSize = 15.0f;
  style.ScrollbarRounding = 9.0f;
  style.GrabMinSize = 5.0f;
  style.GrabRounding = 3.0f;

  style.Colors[ImGuiCol_Text] = ImVec4(0.80f, 0.80f, 0.83f, 1.00f);
  style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
  style.Colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
  style.Colors[ImGuiCol_ChildBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
  style.Colors[ImGuiCol_PopupBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
  style.Colors[ImGuiCol_Border] = ImVec4(0.80f, 0.80f, 0.83f, 0.88f);
  style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.92f, 0.91f, 0.88f, 0.00f);
  style.Colors[ImGuiCol_FrameBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
  style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
  style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
  style.Colors[ImGuiCol_TitleBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
  style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(1.00f, 0.98f, 0.95f, 0.75f);
  style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
  style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
  style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
  style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
  style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
  style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
  style.Colors[ImGuiCol_CheckMark] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
  style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
  style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
  style.Colors[ImGuiCol_Button] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
  style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
  style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
  style.Colors[ImGuiCol_Header] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
  style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
  style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
  style.Colors[ImGuiCol_Separator] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
  style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
  style.Colors[ImGuiCol_SeparatorActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
  style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
  style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
  style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
  style.Colors[ImGuiCol_PlotLines] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
  style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
  style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
  style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
  style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.25f, 1.00f, 0.00f, 0.43f);
  style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(1.00f, 0.98f, 0.95f, 0.73f);
}

void SetupImGuiStyle_RedCustom() {
  ImGuiStyle &style = ImGui::GetStyle();

  style.WindowRounding = 6;
  style.ChildRounding = 6;
  style.FrameRounding = 2;
  style.GrabRounding = 2;
  style.PopupRounding = 2;
  style.ScrollbarSize = 9;
  style.FramePadding = ImVec2(6.0f, 3.0f);
  style.ItemSpacing = ImVec2(4.0f, 4.0f);

  style.Colors[ImGuiCol_WindowBg] = ImColor(0, 0, 0, 230);
  style.Colors[ImGuiCol_Border] = ImColor(0, 0, 0, 0);
  style.Colors[ImGuiCol_Button] = ImColor(31, 30, 31, 255);
  style.Colors[ImGuiCol_ButtonActive] = ImColor(239, 73, 88, 255);
  style.Colors[ImGuiCol_ButtonHovered] = ImColor(173, 55, 65, 255);
  style.Colors[ImGuiCol_FrameBg] = ImColor(31, 30, 31, 255);
  style.Colors[ImGuiCol_FrameBgActive] = ImColor(44, 43, 44, 255);
  style.Colors[ImGuiCol_FrameBgHovered] = ImColor(37, 36, 37, 255);
  style.Colors[ImGuiCol_Text] = ImColor(255, 255, 255, 255);
  style.Colors[ImGuiCol_ChildBg] = ImColor(33, 34, 45, 255);
  style.Colors[ImGuiCol_CheckMark] = ImColor(240, 50, 66, 255);
  style.Colors[ImGuiCol_SliderGrab] = ImColor(240, 74, 88, 255);
  style.Colors[ImGuiCol_SliderGrabActive] = ImColor(240, 50, 66, 255);
  style.Colors[ImGuiCol_Header] = ImColor(240, 50, 66, 255);
  style.Colors[ImGuiCol_HeaderHovered] = ImColor(240, 74, 88, 255);
  style.Colors[ImGuiCol_HeaderActive] = ImColor(240, 50, 66, 255);
  style.Colors[ImGuiCol_ResizeGrip] = ImColor(220, 50, 66, 120);
  style.Colors[ImGuiCol_ResizeGripHovered] = ImColor(250, 50, 66, 140);
  style.Colors[ImGuiCol_ResizeGripActive] = ImColor(240, 50, 66, 255);
  style.Colors[ImGuiCol_SeparatorHovered] = ImColor(250, 50, 66, 140);
  style.Colors[ImGuiCol_SeparatorActive] = ImColor(240, 50, 66, 255);
  style.Colors[ImGuiCol_TitleBgActive] = ImColor(240, 50, 66, 255);
}

void SetupImGuiStyle_Enemymouse() {
  // Enemymouse style by enemymouse from ImThemes
  ImGuiStyle &style = ImGui::GetStyle();

  style.Alpha = 1.0f;
  style.DisabledAlpha = 0.6000000238418579f;
  style.WindowPadding = ImVec2(8.0f, 8.0f);
  style.WindowRounding = 3.0f;
  style.WindowBorderSize = 1.0f;
  style.WindowMinSize = ImVec2(32.0f, 32.0f);
  style.WindowTitleAlign = ImVec2(0.0f, 0.5f);
  style.WindowMenuButtonPosition = ImGuiDir_Left;
  style.ChildRounding = 3.0f;
  style.ChildBorderSize = 1.0f;
  style.PopupRounding = 0.0f;
  style.PopupBorderSize = 1.0f;
  style.FramePadding = ImVec2(4.0f, 3.0f);
  style.FrameRounding = 3.0f;
  style.FrameBorderSize = 0.0f;
  style.ItemSpacing = ImVec2(8.0f, 4.0f);
  style.ItemInnerSpacing = ImVec2(4.0f, 4.0f);
  style.CellPadding = ImVec2(4.0f, 2.0f);
  style.IndentSpacing = 21.0f;
  style.ColumnsMinSpacing = 6.0f;
  style.ScrollbarSize = 14.0f;
  style.ScrollbarRounding = 9.0f;
  style.GrabMinSize = 20.0f;
  style.GrabRounding = 1.0f;
  style.TabRounding = 4.0f;
  style.TabBorderSize = 0.0f;
  style.TabMinWidthForCloseButton = 0.0f;
  style.ColorButtonPosition = ImGuiDir_Right;
  style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
  style.SelectableTextAlign = ImVec2(0.0f, 0.0f);

  style.Colors[ImGuiCol_Text] = ImVec4(0.0f, 1.0f, 1.0f, 1.0f);
  style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.0f, 0.4000000059604645f, 0.407843142747879f, 1.0f);
  style.Colors[ImGuiCol_WindowBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.8299999833106995f);
  style.Colors[ImGuiCol_ChildBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
  style.Colors[ImGuiCol_PopupBg] = ImVec4(0.1568627506494522f, 0.239215686917305f, 0.2196078449487686f, 0.6000000238418579f);
  style.Colors[ImGuiCol_Border] = ImVec4(0.0f, 1.0f, 1.0f, 0.6499999761581421f);
  style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
  style.Colors[ImGuiCol_FrameBg] = ImVec4(0.4392156898975372f, 0.800000011920929f, 0.800000011920929f, 0.1800000071525574f);
  style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.4392156898975372f, 0.800000011920929f, 0.800000011920929f, 0.2700000107288361f);
  style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.4392156898975372f, 0.8078431487083435f, 0.8588235378265381f, 0.6600000262260437f);
  style.Colors[ImGuiCol_TitleBg] = ImVec4(0.1372549086809158f, 0.1764705926179886f, 0.2078431397676468f, 0.7300000190734863f);
  style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.0f, 1.0f, 1.0f, 0.2700000107288361f);
  style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.0f, 0.0f, 0.0f, 0.5400000214576721f);
  style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.2000000029802322f);
  style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.2196078449487686f, 0.2862745225429535f, 0.2980392277240753f, 0.7099999785423279f);
  style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.0f, 1.0f, 1.0f, 0.4399999976158142f);
  style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.0f, 1.0f, 1.0f, 0.7400000095367432f);
  style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.0f, 1.0f, 1.0f, 1.0f);
  style.Colors[ImGuiCol_CheckMark] = ImVec4(0.0f, 1.0f, 1.0f, 0.6800000071525574f);
  style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.0f, 1.0f, 1.0f, 0.3600000143051147f);
  style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.0f, 1.0f, 1.0f, 0.7599999904632568f);
  style.Colors[ImGuiCol_Button] = ImVec4(0.0f, 0.6470588445663452f, 0.6470588445663452f, 0.4600000083446503f);
  style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.007843137718737125f, 1.0f, 1.0f, 0.4300000071525574f);
  style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.0f, 1.0f, 1.0f, 0.6200000047683716f);
  style.Colors[ImGuiCol_Header] = ImVec4(0.0f, 1.0f, 1.0f, 0.3300000131130219f);
  style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.0f, 1.0f, 1.0f, 0.4199999868869781f);
  style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.0f, 1.0f, 1.0f, 0.5400000214576721f);
  style.Colors[ImGuiCol_Separator] = ImVec4(0.0f, 0.4980392158031464f, 0.4980392158031464f, 0.3300000131130219f);
  style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.0f, 0.4980392158031464f, 0.4980392158031464f, 0.4699999988079071f);
  style.Colors[ImGuiCol_SeparatorActive] = ImVec4(0.0f, 0.6980392336845398f, 0.6980392336845398f, 1.0f);
  style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.0f, 1.0f, 1.0f, 0.5400000214576721f);
  style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.0f, 1.0f, 1.0f, 0.7400000095367432f);
  style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.0f, 1.0f, 1.0f, 1.0f);
  style.Colors[ImGuiCol_Tab] = ImVec4(0.1764705926179886f, 0.3490196168422699f, 0.5764706134796143f, 0.8619999885559082f);
  style.Colors[ImGuiCol_TabHovered] = ImVec4(0.2588235437870026f, 0.5882353186607361f, 0.9764705896377563f, 0.800000011920929f);
  style.Colors[ImGuiCol_TabActive] = ImVec4(0.196078434586525f, 0.407843142747879f, 0.6784313917160034f, 1.0f);
  style.Colors[ImGuiCol_TabUnfocused] = ImVec4(0.06666667014360428f, 0.1019607856869698f, 0.1450980454683304f, 0.9724000096321106f);
  style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.1333333402872086f, 0.2588235437870026f, 0.4235294163227081f, 1.0f);
  style.Colors[ImGuiCol_PlotLines] = ImVec4(0.0f, 1.0f, 1.0f, 1.0f);
  style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.0f, 1.0f, 1.0f, 1.0f);
  style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.0f, 1.0f, 1.0f, 1.0f);
  style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.0f, 1.0f, 1.0f, 1.0f);
  style.Colors[ImGuiCol_TableHeaderBg] = ImVec4(0.1882352977991104f, 0.1882352977991104f, 0.2000000029802322f, 1.0f);
  style.Colors[ImGuiCol_TableBorderStrong] = ImVec4(0.3098039329051971f, 0.3098039329051971f, 0.3490196168422699f, 1.0f);
  style.Colors[ImGuiCol_TableBorderLight] = ImVec4(0.2274509817361832f, 0.2274509817361832f, 0.2470588237047195f, 1.0f);
  style.Colors[ImGuiCol_TableRowBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
  style.Colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.0f, 1.0f, 1.0f, 0.05999999865889549f);
  style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.0f, 1.0f, 1.0f, 0.2199999988079071f);
  style.Colors[ImGuiCol_DragDropTarget] = ImVec4(1.0f, 1.0f, 0.0f, 0.8999999761581421f);
  style.Colors[ImGuiCol_NavHighlight] = ImVec4(0.2588235437870026f, 0.5882353186607361f, 0.9764705896377563f, 1.0f);
  style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.0f, 1.0f, 1.0f, 0.699999988079071f);
  style.Colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.800000011920929f, 0.800000011920929f, 0.800000011920929f, 0.2000000029802322f);
  style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.03921568766236305f, 0.09803921729326248f, 0.08627451211214066f, 0.5099999904632568f);
}

void SetupImGuiStyle_SpectrumLight() {
  ImGuiStyle *style = &ImGui::GetStyle();
  style->GrabRounding = 4.0f;

  ImVec4 *colors = style->Colors;
  colors[ImGuiCol_Text] = ImGui::ColorConvertU32ToFloat4(Spectrum::Light::GRAY800);
  colors[ImGuiCol_TextDisabled] = ImGui::ColorConvertU32ToFloat4(Spectrum::Light::GRAY500);
  colors[ImGuiCol_WindowBg] = ImGui::ColorConvertU32ToFloat4(Spectrum::Light::GRAY100);
  colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
  colors[ImGuiCol_PopupBg] = ImGui::ColorConvertU32ToFloat4(Spectrum::Light::GRAY50);
  colors[ImGuiCol_Border] = ImGui::ColorConvertU32ToFloat4(Spectrum::Light::GRAY300);
  colors[ImGuiCol_BorderShadow] = ImGui::ColorConvertU32ToFloat4(Spectrum::Static::NONE);
  colors[ImGuiCol_FrameBg] = ImGui::ColorConvertU32ToFloat4(Spectrum::Light::GRAY75);
  colors[ImGuiCol_FrameBgHovered] = ImGui::ColorConvertU32ToFloat4(Spectrum::Light::GRAY50);
  colors[ImGuiCol_FrameBgActive] = ImGui::ColorConvertU32ToFloat4(Spectrum::Light::GRAY200);
  colors[ImGuiCol_TitleBg] = ImGui::ColorConvertU32ToFloat4(Spectrum::Light::GRAY300);
  colors[ImGuiCol_TitleBgActive] = ImGui::ColorConvertU32ToFloat4(Spectrum::Light::GRAY200);
  colors[ImGuiCol_TitleBgCollapsed] = ImGui::ColorConvertU32ToFloat4(Spectrum::Light::GRAY400);
  colors[ImGuiCol_MenuBarBg] = ImGui::ColorConvertU32ToFloat4(Spectrum::Light::GRAY100);
  colors[ImGuiCol_ScrollbarBg] = ImGui::ColorConvertU32ToFloat4(Spectrum::Light::GRAY100);
  colors[ImGuiCol_ScrollbarGrab] = ImGui::ColorConvertU32ToFloat4(Spectrum::Light::GRAY400);
  colors[ImGuiCol_ScrollbarGrabHovered] = ImGui::ColorConvertU32ToFloat4(Spectrum::Light::GRAY600);
  colors[ImGuiCol_ScrollbarGrabActive] = ImGui::ColorConvertU32ToFloat4(Spectrum::Light::GRAY700);
  colors[ImGuiCol_CheckMark] = ImGui::ColorConvertU32ToFloat4(Spectrum::Light::BLUE500);
  colors[ImGuiCol_SliderGrab] = ImGui::ColorConvertU32ToFloat4(Spectrum::Light::GRAY700);
  colors[ImGuiCol_SliderGrabActive] = ImGui::ColorConvertU32ToFloat4(Spectrum::Light::GRAY800);
  colors[ImGuiCol_Button] = ImGui::ColorConvertU32ToFloat4(Spectrum::Light::GRAY75);
  colors[ImGuiCol_ButtonHovered] = ImGui::ColorConvertU32ToFloat4(Spectrum::Light::GRAY50);
  colors[ImGuiCol_ButtonActive] = ImGui::ColorConvertU32ToFloat4(Spectrum::Light::GRAY200);
  colors[ImGuiCol_Header] = ImGui::ColorConvertU32ToFloat4(Spectrum::Light::BLUE400);
  colors[ImGuiCol_HeaderHovered] = ImGui::ColorConvertU32ToFloat4(Spectrum::Light::BLUE500);
  colors[ImGuiCol_HeaderActive] = ImGui::ColorConvertU32ToFloat4(Spectrum::Light::BLUE600);
  colors[ImGuiCol_Separator] = ImGui::ColorConvertU32ToFloat4(Spectrum::Light::GRAY400);
  colors[ImGuiCol_SeparatorHovered] = ImGui::ColorConvertU32ToFloat4(Spectrum::Light::GRAY600);
  colors[ImGuiCol_SeparatorActive] = ImGui::ColorConvertU32ToFloat4(Spectrum::Light::GRAY700);
  colors[ImGuiCol_ResizeGrip] = ImGui::ColorConvertU32ToFloat4(Spectrum::Light::GRAY400);
  colors[ImGuiCol_ResizeGripHovered] = ImGui::ColorConvertU32ToFloat4(Spectrum::Light::GRAY600);
  colors[ImGuiCol_ResizeGripActive] = ImGui::ColorConvertU32ToFloat4(Spectrum::Light::GRAY700);
  colors[ImGuiCol_PlotLines] = ImGui::ColorConvertU32ToFloat4(Spectrum::Light::BLUE400);
  colors[ImGuiCol_PlotLinesHovered] = ImGui::ColorConvertU32ToFloat4(Spectrum::Light::BLUE600);
  colors[ImGuiCol_PlotHistogram] = ImGui::ColorConvertU32ToFloat4(Spectrum::Light::BLUE400);
  colors[ImGuiCol_PlotHistogramHovered] = ImGui::ColorConvertU32ToFloat4(Spectrum::Light::BLUE600);
  colors[ImGuiCol_TextSelectedBg] = ImGui::ColorConvertU32ToFloat4((Spectrum::Light::BLUE400 & 0x00FFFFFF) | 0x33000000);
  colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
  colors[ImGuiCol_NavHighlight] = ImGui::ColorConvertU32ToFloat4((Spectrum::Light::GRAY900 & 0x00FFFFFF) | 0x0A000000);
  colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
  colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
  colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);
}

void SetupImGuiStyle_SpectrumDark() {
  ImGuiStyle *style = &ImGui::GetStyle();
  style->GrabRounding = 4.0f;

  ImVec4 *colors = style->Colors;
  colors[ImGuiCol_Text] = ImGui::ColorConvertU32ToFloat4(Spectrum::Dark::GRAY800);
  colors[ImGuiCol_TextDisabled] = ImGui::ColorConvertU32ToFloat4(Spectrum::Dark::GRAY500);
  colors[ImGuiCol_WindowBg] = ImGui::ColorConvertU32ToFloat4(Spectrum::Dark::GRAY100);
  colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
  colors[ImGuiCol_PopupBg] = ImGui::ColorConvertU32ToFloat4(Spectrum::Dark::GRAY50);
  colors[ImGuiCol_Border] = ImGui::ColorConvertU32ToFloat4(Spectrum::Dark::GRAY300);
  colors[ImGuiCol_BorderShadow] = ImGui::ColorConvertU32ToFloat4(Spectrum::Static::NONE);
  colors[ImGuiCol_FrameBg] = ImGui::ColorConvertU32ToFloat4(Spectrum::Dark::GRAY75);
  colors[ImGuiCol_FrameBgHovered] = ImGui::ColorConvertU32ToFloat4(Spectrum::Dark::GRAY50);
  colors[ImGuiCol_FrameBgActive] = ImGui::ColorConvertU32ToFloat4(Spectrum::Dark::GRAY200);
  colors[ImGuiCol_TitleBg] = ImGui::ColorConvertU32ToFloat4(Spectrum::Dark::GRAY300);
  colors[ImGuiCol_TitleBgActive] = ImGui::ColorConvertU32ToFloat4(Spectrum::Dark::GRAY200);
  colors[ImGuiCol_TitleBgCollapsed] = ImGui::ColorConvertU32ToFloat4(Spectrum::Dark::GRAY400);
  colors[ImGuiCol_MenuBarBg] = ImGui::ColorConvertU32ToFloat4(Spectrum::Dark::GRAY100);
  colors[ImGuiCol_ScrollbarBg] = ImGui::ColorConvertU32ToFloat4(Spectrum::Dark::GRAY100);
  colors[ImGuiCol_ScrollbarGrab] = ImGui::ColorConvertU32ToFloat4(Spectrum::Dark::GRAY400);
  colors[ImGuiCol_ScrollbarGrabHovered] = ImGui::ColorConvertU32ToFloat4(Spectrum::Dark::GRAY600);
  colors[ImGuiCol_ScrollbarGrabActive] = ImGui::ColorConvertU32ToFloat4(Spectrum::Dark::GRAY700);
  colors[ImGuiCol_CheckMark] = ImGui::ColorConvertU32ToFloat4(Spectrum::Dark::BLUE500);
  colors[ImGuiCol_SliderGrab] = ImGui::ColorConvertU32ToFloat4(Spectrum::Dark::GRAY700);
  colors[ImGuiCol_SliderGrabActive] = ImGui::ColorConvertU32ToFloat4(Spectrum::Dark::GRAY800);
  colors[ImGuiCol_Button] = ImGui::ColorConvertU32ToFloat4(Spectrum::Dark::GRAY75);
  colors[ImGuiCol_ButtonHovered] = ImGui::ColorConvertU32ToFloat4(Spectrum::Dark::GRAY50);
  colors[ImGuiCol_ButtonActive] = ImGui::ColorConvertU32ToFloat4(Spectrum::Dark::GRAY200);
  colors[ImGuiCol_Header] = ImGui::ColorConvertU32ToFloat4(Spectrum::Dark::BLUE400);
  colors[ImGuiCol_HeaderHovered] = ImGui::ColorConvertU32ToFloat4(Spectrum::Dark::BLUE500);
  colors[ImGuiCol_HeaderActive] = ImGui::ColorConvertU32ToFloat4(Spectrum::Dark::BLUE600);
  colors[ImGuiCol_Separator] = ImGui::ColorConvertU32ToFloat4(Spectrum::Dark::GRAY400);
  colors[ImGuiCol_SeparatorHovered] = ImGui::ColorConvertU32ToFloat4(Spectrum::Dark::GRAY600);
  colors[ImGuiCol_SeparatorActive] = ImGui::ColorConvertU32ToFloat4(Spectrum::Dark::GRAY700);
  colors[ImGuiCol_ResizeGrip] = ImGui::ColorConvertU32ToFloat4(Spectrum::Dark::GRAY400);
  colors[ImGuiCol_ResizeGripHovered] = ImGui::ColorConvertU32ToFloat4(Spectrum::Dark::GRAY600);
  colors[ImGuiCol_ResizeGripActive] = ImGui::ColorConvertU32ToFloat4(Spectrum::Dark::GRAY700);
  colors[ImGuiCol_PlotLines] = ImGui::ColorConvertU32ToFloat4(Spectrum::Dark::BLUE400);
  colors[ImGuiCol_PlotLinesHovered] = ImGui::ColorConvertU32ToFloat4(Spectrum::Dark::BLUE600);
  colors[ImGuiCol_PlotHistogram] = ImGui::ColorConvertU32ToFloat4(Spectrum::Dark::BLUE400);
  colors[ImGuiCol_PlotHistogramHovered] = ImGui::ColorConvertU32ToFloat4(Spectrum::Dark::BLUE600);
  colors[ImGuiCol_TextSelectedBg] = ImGui::ColorConvertU32ToFloat4((Spectrum::Dark::BLUE400 & 0x00FFFFFF) | 0x33000000);
  colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
  colors[ImGuiCol_NavHighlight] = ImGui::ColorConvertU32ToFloat4((Spectrum::Dark::GRAY900 & 0x00FFFFFF) | 0x0A000000);
  colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
  colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
  colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);
}

void DrawTabHorizontally(std::string childName, ImVec2 childSize, std::vector<std::string> tabNames, int &selectedSubTab) {
  int length = tabNames.front().length();  // get shortest string length
  int strIndex = 1;
  for (int i = 1; i < tabNames.size(); i++) {
    if (length > tabNames.at(i).length()) {
      length = tabNames.at(i).length();
      strIndex = i;
    }
  }

  ImGui::BeginChild(childName.c_str(), childSize, true, ImGuiWindowFlags_HorizontalScrollbar);

  int minWidth = GetTextLength(tabNames.at(strIndex).c_str()).x;
  int maxWidth = 200;

  int btnWidth = (ImGetWidth() - ImGui::GetStyle().ItemSpacing.x * (tabNames.size())) / tabNames.size();
  int btnHeight = std::clamp(ImGetHeight(), 20.0f, 60.0f);
  btnWidth = (std::max)(minWidth, (std::min)(btnWidth, maxWidth));

  {  // center buttons
    // tell Dear ImGui to render the button at the new pos
    ImGui::SetCursorPosX((childSize.x - btnWidth * tabNames.size() - ImGui::GetStyle().ItemSpacing.x) / 2);
  }

  ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5);  // round buttons
  for (int i = 0; i < tabNames.size(); i++) {
    std::string it = tabNames.at(i);
    ImGui::PushStyleColor(ImGuiCol_Button,
                          selectedSubTab == i ? ImGui::GetStyle().Colors[ImGuiCol_ButtonActive] : ImGui::GetStyle().Colors[ImGuiCol_Button]);
    ImGui::PushStyleColor(ImGuiCol_Text, selectedSubTab == i ? ImGui::GetStyle().Colors[ImGuiCol_Text] : RBGA2Vec4(140, 140, 140, 255));

    //if (ImGui::Button(it.c_str(), ImVec2(btnWidth, btnHeight))) selectedSubTab = i;
    if (ImGuiB::TabButton(it.c_str(), ImVec2(btnWidth, btnHeight) , selectedSubTab == i)) selectedSubTab = i;
    ImGui::SameLine();
    ImGui::PopStyleColor(2);
  }
  ImGui::PopStyleVar();

  ImGui::EndChild();
}

namespace Colors {
inline ImVec4 LeftBar;
inline ImVec4 Background;
inline ImVec4 Borders;
inline ImVec4 BordersActive;
inline ImVec4 Child;
inline ImVec4 ChildText;
inline ImVec4 TabFrame;
inline ImVec4 TabText;
inline ImVec4 Frame;
inline ImVec4 FrameDisabled;
inline ImVec4 FrameHovered;
inline ImVec4 FrameActive;
inline ImVec4 FrameOpened;
inline ImVec4 Logo;
inline ImVec4 AccentDisabled;
inline ImVec4 Accent;
inline ImVec4 Text;
inline ImVec4 TextDisabled;
inline ImVec4 TextActive;
}  // namespace Colors

using namespace ImGui;
static float AnimationSpeed = 1.0;

// Those MIN/MAX values are not define because we need to point to them
static const signed char IM_S8_MIN = -128;
static const signed char IM_S8_MAX = 127;
static const unsigned char IM_U8_MIN = 0;
static const unsigned char IM_U8_MAX = 0xFF;
static const signed short IM_S16_MIN = -32768;
static const signed short IM_S16_MAX = 32767;
static const unsigned short IM_U16_MIN = 0;
static const unsigned short IM_U16_MAX = 0xFFFF;
static const ImS32 IM_S32_MIN = INT_MIN;  // (-2147483647 - 1), (0x80000000);
static const ImS32 IM_S32_MAX = INT_MAX;  // (2147483647), (0x7FFFFFFF)
static const ImU32 IM_U32_MIN = 0;
static const ImU32 IM_U32_MAX = UINT_MAX;  // (0xFFFFFFFF)
#ifdef LLONG_MIN
static const ImS64 IM_S64_MIN = LLONG_MIN;  // (-9223372036854775807ll - 1ll);
static const ImS64 IM_S64_MAX = LLONG_MAX;  // (9223372036854775807ll);
#else
static const ImS64 IM_S64_MIN = -9223372036854775807LL - 1;
static const ImS64 IM_S64_MAX = 9223372036854775807LL;
#endif
static const ImU64 IM_U64_MIN = 0;
#ifdef ULLONG_MAX
static const ImU64 IM_U64_MAX = ULLONG_MAX;  // (0xFFFFFFFFFFFFFFFFull);
#else
static const ImU64 IM_U64_MAX = (2ULL * 9223372036854775807LL + 1);
#endif

void SetupImGuiStyle_NeverBlue() {
  ImGuiIO &io = ImGui::GetIO();
  ImVec4 *colors = ImGui::GetStyle().Colors;
  Ogre::Real time = io.DeltaTime * 12.f * AnimationSpeed;
  int alpha = 255 * ImGui::GetStyle().Alpha;

  Colors::LeftBar = ImLerp(Colors::LeftBar, ImColor(3, 30, 50, alpha).Value, time);
  Colors::Background = ImLerp(Colors::Background, ImColor(9, 8, 13, alpha).Value, time);
  Colors::Child = ImLerp(Colors::Child, ImColor(1, 11, 21, alpha).Value, time);
  Colors::ChildText = ImLerp(Colors::ChildText, ImColor(200, 203, 208, alpha).Value, time);
  Colors::Borders = ImLerp(Colors::Borders, ImColor(12, 33, 52, alpha).Value, time);
  Colors::BordersActive = ImLerp(Colors::BordersActive, ImColor(4, 25, 40, alpha).Value, time);
  Colors::TabFrame = ImLerp(Colors::TabFrame, ImColor(1, 72, 104, alpha).Value, time);
  Colors::TabText = ImLerp(Colors::TabText, ImColor(255, 255, 255, alpha).Value, time);
  Colors::Logo = ImLerp(Colors::Logo, ImColor(248, 255, 255, alpha).Value, time);
  Colors::Text = ImLerp(Colors::Text, ImColor(162, 176, 185, alpha).Value, time);
  Colors::TextActive = ImLerp(Colors::TextActive, ImColor(255, 255, 255, alpha).Value, time);
  Colors::Accent = ImLerp(Colors::Accent, ImColor(0, 165, 243, alpha).Value, io.DeltaTime * 6.f);
  Colors::AccentDisabled = ImLerp(Colors::AccentDisabled, ImColor(76, 90, 101, alpha).Value, time);
  Colors::Frame = ImLerp(Colors::Frame, ImColor(2, 5, 12, alpha).Value, time);
  Colors::FrameDisabled = ImLerp(Colors::FrameDisabled, ImColor(8, 9, 13, alpha).Value, time);
  Colors::FrameHovered = ImLerp(Colors::FrameHovered, ImColor(2, 5, 12, alpha).Value, time);
  Colors::FrameActive = ImLerp(Colors::FrameActive, ImColor(2, 18, 33, alpha).Value, time);
  Colors::FrameOpened = ImLerp(Colors::FrameOpened, ImColor(2, 18, 33, alpha).Value, time);
}

void SetupImGuiStyle_NeverDark() {
  ImGuiIO &io = ImGui::GetIO();
  ImVec4 *colors = ImGui::GetStyle().Colors;
  Ogre::Real time = io.DeltaTime * 12.f * AnimationSpeed;
  int alpha = 255 * ImGui::GetStyle().Alpha;

  Colors::LeftBar = ImLerp(Colors::LeftBar, ImColor(8, 8, 8, alpha).Value, time);
  Colors::Background = ImLerp(Colors::Background, ImColor(8, 8, 8, alpha).Value, time);
  Colors::Child = ImLerp(Colors::Child, ImColor(11, 11, 13, alpha).Value, time);
  Colors::ChildText = ImLerp(Colors::ChildText, ImColor(221, 220, 225, alpha).Value, time);
  Colors::Borders = ImLerp(Colors::Borders, ImColor(26, 26, 26, alpha).Value, time);
  Colors::BordersActive = ImLerp(Colors::BordersActive, ImColor(36, 34, 38, alpha), time);
  Colors::TabFrame = ImLerp(Colors::TabFrame, ImColor(62, 63, 58, alpha).Value, time);
  Colors::TabText = ImLerp(Colors::TabText, ImColor(255, 255, 255, alpha).Value, time);
  Colors::Logo = ImLerp(Colors::Logo, ImColor(253, 254, 255, alpha).Value, time);
  Colors::Text = ImLerp(Colors::Text, ImColor(162, 176, 185, alpha).Value, time);
  Colors::TextActive = ImLerp(Colors::TextActive, ImColor(255, 255, 255, alpha).Value, time);
  Colors::Accent = ImLerp(Colors::Accent, ImColor(0, 165, 243, alpha).Value, io.DeltaTime * 6.f);
  Colors::AccentDisabled = ImLerp(Colors::AccentDisabled, ImColor(76, 90, 101, alpha).Value, time);
  Colors::Frame = ImLerp(Colors::Frame, ImColor(3, 6, 15, alpha).Value, time);
  Colors::FrameDisabled = ImLerp(Colors::FrameDisabled, ImColor(8, 8, 8, alpha).Value, time);
  Colors::FrameHovered = ImLerp(Colors::FrameHovered, ImColor(13, 13, 13, alpha).Value, time);
  Colors::FrameActive = ImLerp(Colors::FrameActive, ImColor(4, 25, 46, alpha).Value, time);
  Colors::FrameOpened = ImLerp(Colors::FrameOpened, ImColor(32, 31, 33, alpha).Value, time);
}

void SetupImGuiStyle_NeverLight() {
  ImGuiIO &io = ImGui::GetIO();
  ImVec4 *colors = ImGui::GetStyle().Colors;
  Ogre::Real time = io.DeltaTime * 12.f * AnimationSpeed;
  int alpha = 255 * ImGui::GetStyle().Alpha;

  Colors::LeftBar = ImLerp(Colors::LeftBar, ImColor(255, 255, 255, int(255 * ImGui::GetStyle().Alpha * 0.75f)).Value, time);
  Colors::Background = ImLerp(Colors::Background, ImColor(255, 255, 255, alpha).Value, time);
  Colors::Child = ImLerp(Colors::Child, ImColor(250, 250, 250, alpha).Value, time);
  Colors::ChildText = ImLerp(Colors::ChildText, ImColor(0, 0, 0, alpha).Value, time);
  Colors::Borders = ImLerp(Colors::Borders, ImColor(217, 217, 217, alpha).Value, time);
  Colors::BordersActive = ImLerp(Colors::BordersActive, ImColor(217, 217, 217, alpha).Value, time);
  Colors::TabFrame = ImLerp(Colors::TabFrame, ImColor(214, 213, 213, alpha).Value, time);
  Colors::TabText = ImLerp(Colors::TabText, ImColor(38, 38, 38, alpha).Value, time);
  Colors::Logo = ImLerp(Colors::Logo, ImColor(51, 51, 51, alpha).Value, time);
  Colors::Text = ImLerp(Colors::Text, ImColor(77, 77, 77, alpha).Value, time);
  Colors::TextActive = ImLerp(Colors::TextActive, ImColor(6, 6, 6, alpha).Value, time);
  Colors::Accent = ImLerp(Colors::Accent, ImColor(0, 165, 243, alpha).Value, io.DeltaTime * 6.f);
  Colors::AccentDisabled = ImLerp(Colors::AccentDisabled, ImColor(255, 255, 255, alpha).Value, time);
  Colors::Frame = ImLerp(Colors::Frame, ImColor(231, 231, 231, alpha).Value, time);
  Colors::FrameDisabled = ImLerp(Colors::FrameDisabled, ImColor(255, 255, 255, alpha).Value, time);
  Colors::FrameHovered = ImLerp(Colors::FrameHovered, ImColor(255, 255, 255, alpha).Value, time);
  Colors::FrameActive = ImLerp(Colors::FrameActive, ImColor(0, 119, 188, alpha).Value, time);
  Colors::FrameOpened = ImLerp(Colors::FrameOpened, ImColor(238, 238, 238, alpha).Value, time);
}

bool TabButton(const char *label, ImVec2 size, bool active) {
  ImGuiWindow *window = GetCurrentWindow();
  if (window->SkipItems) return false;

  ImGuiContext &g = *GImGui;
  const ImGuiStyle &style = g.Style;
  const ImGuiID id = window->GetID(label);
  const ImVec2 label_size = CalcTextSize(label, NULL, true);

  ImVec2 pos = window->DC.CursorPos;

  const ImRect bb(pos, pos + size);
  ItemSize(size, style.FramePadding.y);
  if (!ItemAdd(bb, id)) return false;

  bool hovered, held;
  bool pressed = ButtonBehavior(bb, id, &hovered, &held, 0);

  struct TabAnimation {
    ImColor TabFrame;
  };

  static std::map<ImGuiID, TabAnimation> TabMap;
  auto TabItemMap = TabMap.find(id);
  if (TabItemMap == TabMap.end()) {
    TabMap.insert({id, TabAnimation()});
    TabItemMap = TabMap.find(id);
  }
  auto TabItem = TabItemMap->second.TabFrame = ImLerp(TabItemMap->second.TabFrame.Value,
                                                      active ? ImColor(Colors::TabFrame.x, Colors::TabFrame.y, Colors::TabFrame.z, Colors::TabFrame.w)
                                                             : ImColor(Colors::TabFrame.x, Colors::TabFrame.y, Colors::TabFrame.z, 0.f),
                                                      g.IO.DeltaTime * 8.f * AnimationSpeed);

  // Render
  RenderNavHighlight(bb, id);
  RenderFrame(bb.Min + ImVec2(15, 0), bb.Max - ImVec2(15, 0), TabItemMap->second.TabFrame, true, 4.f);

  if (g.LogEnabled) LogSetNextTextDecoration("[", "]");
  window->DrawList->AddText(bb.Min + ImVec2(50, bb.GetSize().y / 2.f - CalcTextSize(label).y / 2.f), ImColor(Colors::TabText), label);

  IMGUI_TEST_ENGINE_ITEM_INFO(id, label, g.LastItemData.StatusFlags);
  return pressed;
}

bool Checkbox(const char *label, bool *v) {
  ImGuiWindow *window = GetCurrentWindow();
  if (window->SkipItems) return false;

  ImGuiContext &g = *GImGui;
  const ImGuiStyle &style = g.Style;
  const ImGuiID id = window->GetID(label);
  const ImGuiID id_mark = window->GetID(std::string(std::string(label) + "_mark").c_str());
  const ImVec2 label_size = CalcTextSize(label, NULL, true);

  const float square_sz2 = ImGui::GetColumnWidth();
  const float square_sz = ImGui::GetFrameHeight();
  const ImVec2 pos = window->DC.CursorPos;
  const ImRect total_bb(pos, pos + ImVec2(square_sz + (label_size.x > 0.0f ? style.ItemInnerSpacing.x + label_size.x : 0.0f),
                                          label_size.y + style.FramePadding.y * 2.0f + 5.f));
  const ImRect checkmark_bb(pos + ImVec2(square_sz2 - 35.f, 0), pos + ImVec2(square_sz2 - 10.f, label_size.y + style.FramePadding.y * 2.0f + 5.f));
  ItemSize(total_bb, style.FramePadding.y);
  const bool can_add = ItemAdd(total_bb, id);

  bool hovered, held;
  bool pressed = ButtonBehavior(total_bb, id, &hovered, &held);

  bool hovered_mark, held_mark;
  pressed = pressed || ButtonBehavior(checkmark_bb, id_mark, &hovered_mark, &held_mark);
  if (pressed) {
    *v = !(*v);
    MarkItemEdited(id);
  }

  struct TabAnimation {
    ImColor CheckboxFrame;
    ImColor CircleFrame;
    ImColor Text;
    float CirclePos;
  };

  static std::map<ImGuiID, TabAnimation> TabMap;
  auto TabItemMap = TabMap.find(id);
  if (TabItemMap == TabMap.end()) {
    TabMap.insert({id, TabAnimation()});
    TabItemMap = TabMap.find(id);
  }
  TabItemMap->second.CheckboxFrame =
      ImLerp(TabItemMap->second.CheckboxFrame.Value, *v ? Colors::FrameActive : Colors::Frame, g.IO.DeltaTime * 8.f * AnimationSpeed);
  TabItemMap->second.CircleFrame =
      ImLerp(TabItemMap->second.CircleFrame.Value, *v ? Colors::Accent : Colors::AccentDisabled, g.IO.DeltaTime * 8.f * AnimationSpeed);
  TabItemMap->second.Text = ImLerp(TabItemMap->second.Text.Value, *v ? Colors::TextActive : Colors::Text, g.IO.DeltaTime * 8.f * AnimationSpeed);
  TabItemMap->second.CirclePos = ImLerp(TabItemMap->second.CirclePos, *v ? 21.5f : 4.5f, g.IO.DeltaTime * 8.f * AnimationSpeed);

  RenderNavHighlight(total_bb, id);
  RenderFrame(checkmark_bb.Min + ImVec2(0, 8.f), checkmark_bb.Max - ImVec2(0, 8.f), TabItemMap->second.CheckboxFrame, false, 12.f);
  window->DrawList->AddCircleFilled(checkmark_bb.Min + ImVec2(TabItemMap->second.CirclePos, checkmark_bb.GetSize().y / 2.f), 7.f,
                                    TabItemMap->second.CircleFrame);

  ImVec2 label_pos = ImVec2(total_bb.Min.x + 10.f, total_bb.GetCenter().y - CalcTextSize(label).y / 2.f);
  if (label_size.x > 0.0f) window->DrawList->AddText(label_pos, TabItemMap->second.Text, label);

  IMGUI_TEST_ENGINE_ITEM_INFO(id, label, g.LastItemData.StatusFlags | ImGuiItemStatusFlags_Checkable | (*v ? ImGuiItemStatusFlags_Checked : 0));
  return pressed;
}

// FIXME-LEGACY: Prior to 1.61 our DragInt() function internally used floats and because of this the compile-time default value for format was "%.0f".
// Even though we changed the compile-time default, we expect users to have carried %f around, which would break the display of DragInt() calls.
// To honor backward compatibility we are rewriting the format string, unless IMGUI_DISABLE_OBSOLETE_FUNCTIONS is enabled. What could possibly go
// wrong?!
static const char *PatchFormatStringFloatToInt(const char *fmt) {
  if (fmt[0] == '%' && fmt[1] == '.' && fmt[2] == '0' && fmt[3] == 'f' &&
      fmt[4] == 0)  // Fast legacy path for "%.0f" which is expected to be the most common case.
    return "%d";
  const char *fmt_start = ImParseFormatFindStart(fmt);    // Find % (if any, and ignore %%)
  const char *fmt_end = ImParseFormatFindEnd(fmt_start);  // Find end of format specifier, which itself is an exercise of confidence/recklessness
                                                          // (because snprintf is dependent on libc or user).
  if (fmt_end > fmt_start && fmt_end[-1] == 'f') {
#ifndef IMGUI_DISABLE_OBSOLETE_FUNCTIONS
    if (fmt_start == fmt && fmt_end[0] == 0) return "%d";
    ImGuiContext &g = *GImGui;
    ImFormatString(g.TempBuffer, IM_ARRAYSIZE(g.TempBuffer), "%.*s%%d%s", (int)(fmt_start - fmt), fmt,
                   fmt_end);  // Honor leading and trailing decorations, but lose alignment/precision.
    return g.TempBuffer;
#else
    IM_ASSERT(0 && "DragInt(): Invalid format string!");  // Old versions used a default parameter of "%.0f", please replace with e.g. "%d"
#endif
  }
  return fmt;
}

// Convert a value v in the output space of a slider into a parametric position on the slider itself (the logical opposite of ScaleValueFromRatioT)
template<typename TYPE, typename SIGNEDTYPE, typename FLOATTYPE>
float ScaleRatioFromValueT(ImGuiDataType data_type, TYPE v, TYPE v_min, TYPE v_max, bool is_logarithmic, float logarithmic_zero_epsilon, float zero_deadzone_halfsize)
{
  if (v_min == v_max)
    return 0.0f;
  IM_UNUSED(data_type);

  const TYPE v_clamped = (v_min < v_max) ? ImClamp(v, v_min, v_max) : ImClamp(v, v_max, v_min);
  if (is_logarithmic)
  {
    bool flipped = v_max < v_min;

    if (flipped) // Handle the case where the range is backwards
      ImSwap(v_min, v_max);

    // Fudge min/max to avoid getting close to log(0)
    FLOATTYPE v_min_fudged = (ImAbs((FLOATTYPE)v_min) < logarithmic_zero_epsilon) ? ((v_min < 0.0f) ? -logarithmic_zero_epsilon : logarithmic_zero_epsilon) : (FLOATTYPE)v_min;
    FLOATTYPE v_max_fudged = (ImAbs((FLOATTYPE)v_max) < logarithmic_zero_epsilon) ? ((v_max < 0.0f) ? -logarithmic_zero_epsilon : logarithmic_zero_epsilon) : (FLOATTYPE)v_max;

    // Awkward special cases - we need ranges of the form (-100 .. 0) to convert to (-100 .. -epsilon), not (-100 .. epsilon)
    if ((v_min == 0.0f) && (v_max < 0.0f))
      v_min_fudged = -logarithmic_zero_epsilon;
    else if ((v_max == 0.0f) && (v_min < 0.0f))
      v_max_fudged = -logarithmic_zero_epsilon;

    float result;

    if (v_clamped <= v_min_fudged)
      result = 0.0f; // Workaround for values that are in-range but below our fudge
    else if (v_clamped >= v_max_fudged)
      result = 1.0f; // Workaround for values that are in-range but above our fudge
    else if ((v_min * v_max) < 0.0f) // Range crosses zero, so split into two portions
    {
      float zero_point_center = (-(float)v_min) / ((float)v_max - (float)v_min); // The zero point in parametric space.  There's an argument we should take the logarithmic nature into account when calculating this, but for now this should do (and the most common case of a symmetrical range works fine)
      float zero_point_snap_L = zero_point_center - zero_deadzone_halfsize;
      float zero_point_snap_R = zero_point_center + zero_deadzone_halfsize;
      if (v == 0.0f)
        result = zero_point_center; // Special case for exactly zero
      else if (v < 0.0f)
        result = (1.0f - (float)(ImLog(-(FLOATTYPE)v_clamped / logarithmic_zero_epsilon) / ImLog(-v_min_fudged / logarithmic_zero_epsilon))) * zero_point_snap_L;
      else
        result = zero_point_snap_R + ((float)(ImLog((FLOATTYPE)v_clamped / logarithmic_zero_epsilon) / ImLog(v_max_fudged / logarithmic_zero_epsilon)) * (1.0f - zero_point_snap_R));
    }
    else if ((v_min < 0.0f) || (v_max < 0.0f)) // Entirely negative slider
      result = 1.0f - (float)(ImLog(-(FLOATTYPE)v_clamped / -v_max_fudged) / ImLog(-v_min_fudged / -v_max_fudged));
    else
      result = (float)(ImLog((FLOATTYPE)v_clamped / v_min_fudged) / ImLog(v_max_fudged / v_min_fudged));

    return flipped ? (1.0f - result) : result;
  }

  // Linear slider
  return (float)((FLOATTYPE)(SIGNEDTYPE)(v_clamped - v_min) / (FLOATTYPE)(SIGNEDTYPE)(v_max - v_min));
}

// Convert a parametric position on a slider into a value v in the output space (the logical opposite of ScaleRatioFromValueT)
template<typename TYPE, typename SIGNEDTYPE, typename FLOATTYPE>
TYPE ScaleValueFromRatioT(ImGuiDataType data_type, float t, TYPE v_min, TYPE v_max, bool is_logarithmic, float logarithmic_zero_epsilon, float zero_deadzone_halfsize)
{
  if (v_min == v_max)
    return v_min;
  const bool is_floating_point = (data_type == ImGuiDataType_Float) || (data_type == ImGuiDataType_Double);

  TYPE result;
  if (is_logarithmic)
  {
    // We special-case the extents because otherwise our fudging can lead to "mathematically correct" but non-intuitive behaviors like a fully-left slider not actually reaching the minimum value
    if (t <= 0.0f)
      result = v_min;
    else if (t >= 1.0f)
      result = v_max;
    else
    {
      bool flipped = v_max < v_min; // Check if range is "backwards"

      // Fudge min/max to avoid getting silly results close to zero
      FLOATTYPE v_min_fudged = (ImAbs((FLOATTYPE)v_min) < logarithmic_zero_epsilon) ? ((v_min < 0.0f) ? -logarithmic_zero_epsilon : logarithmic_zero_epsilon) : (FLOATTYPE)v_min;
      FLOATTYPE v_max_fudged = (ImAbs((FLOATTYPE)v_max) < logarithmic_zero_epsilon) ? ((v_max < 0.0f) ? -logarithmic_zero_epsilon : logarithmic_zero_epsilon) : (FLOATTYPE)v_max;

      if (flipped)
        ImSwap(v_min_fudged, v_max_fudged);

      // Awkward special case - we need ranges of the form (-100 .. 0) to convert to (-100 .. -epsilon), not (-100 .. epsilon)
      if ((v_max == 0.0f) && (v_min < 0.0f))
        v_max_fudged = -logarithmic_zero_epsilon;

      float t_with_flip = flipped ? (1.0f - t) : t; // t, but flipped if necessary to account for us flipping the range

      if ((v_min * v_max) < 0.0f) // Range crosses zero, so we have to do this in two parts
      {
        float zero_point_center = (-(float)ImMin(v_min, v_max)) / ImAbs((float)v_max - (float)v_min); // The zero point in parametric space
        float zero_point_snap_L = zero_point_center - zero_deadzone_halfsize;
        float zero_point_snap_R = zero_point_center + zero_deadzone_halfsize;
        if (t_with_flip >= zero_point_snap_L && t_with_flip <= zero_point_snap_R)
          result = (TYPE)0.0f; // Special case to make getting exactly zero possible (the epsilon prevents it otherwise)
        else if (t_with_flip < zero_point_center)
          result = (TYPE)-(logarithmic_zero_epsilon * ImPow(-v_min_fudged / logarithmic_zero_epsilon, (FLOATTYPE)(1.0f - (t_with_flip / zero_point_snap_L))));
        else
          result = (TYPE)(logarithmic_zero_epsilon * ImPow(v_max_fudged / logarithmic_zero_epsilon, (FLOATTYPE)((t_with_flip - zero_point_snap_R) / (1.0f - zero_point_snap_R))));
      }
      else if ((v_min < 0.0f) || (v_max < 0.0f)) // Entirely negative slider
        result = (TYPE)-(-v_max_fudged * ImPow(-v_min_fudged / -v_max_fudged, (FLOATTYPE)(1.0f - t_with_flip)));
      else
        result = (TYPE)(v_min_fudged * ImPow(v_max_fudged / v_min_fudged, (FLOATTYPE)t_with_flip));
    }
  }
  else
  {
    // Linear slider
    if (is_floating_point)
    {
      result = ImLerp(v_min, v_max, t);
    }
    else
    {
      // - For integer values we want the clicking position to match the grab box so we round above
      //   This code is carefully tuned to work with large values (e.g. high ranges of U64) while preserving this property..
      // - Not doing a *1.0 multiply at the end of a range as it tends to be lossy. While absolute aiming at a large s64/u64
      //   range is going to be imprecise anyway, with this check we at least make the edge values matches expected limits.
      if (t < 1.0)
      {
        FLOATTYPE v_new_off_f = (SIGNEDTYPE)(v_max - v_min) * t;
        result = (TYPE)((SIGNEDTYPE)v_min + (SIGNEDTYPE)(v_new_off_f + (FLOATTYPE)(v_min > v_max ? -0.5 : 0.5)));
      }
      else
      {
        result = v_max;
      }
    }
  }

  return result;
}

// FIXME: Move more of the code into SliderBehavior()
//template <typename TYPE, typename SIGNEDTYPE, typename FLOATTYPE>
//bool SliderBehaviorT(const ImRect &bb, ImGuiID id, ImGuiDataType data_type, TYPE *v, const TYPE v_min, const TYPE v_max, const char *format,
//                     ImGuiSliderFlags flags, ImRect *out_grab_bb, float *percent) {
//  ImGuiContext &g = *GImGui;
//  const ImGuiStyle &style = g.Style;
//
//  const ImGuiAxis axis = (flags & ImGuiSliderFlags_Vertical) ? ImGuiAxis_Y : ImGuiAxis_X;
//  const bool is_logarithmic = (flags & ImGuiSliderFlags_Logarithmic) != 0;
//  const bool is_floating_point = (data_type == ImGuiDataType_Float) || (data_type == ImGuiDataType_Double);
//
//  const float grab_padding = 2.0f;
//  const float slider_sz = (bb.Max[axis] - bb.Min[axis]) - grab_padding * 2.0f;
//  float grab_sz = style.GrabMinSize;
//  SIGNEDTYPE v_range = (v_min < v_max ? v_max - v_min : v_min - v_max);
//  if (!is_floating_point && v_range >= 0)                                    // v_range < 0 may happen on integer overflows
//    grab_sz = ImMax((float)(slider_sz / (v_range + 1)), style.GrabMinSize);  // For integer sliders: if possible have the grab size represent 1 unit
//  grab_sz = ImMin(grab_sz, slider_sz);
//  const float slider_usable_sz = slider_sz - grab_sz;
//  const float slider_usable_pos_min = bb.Min[axis] + grab_padding + grab_sz * 0.5f;
//  const float slider_usable_pos_max = bb.Max[axis] - grab_padding - grab_sz * 0.5f;
//
//  float logarithmic_zero_epsilon = 0.0f;  // Only valid when is_logarithmic is true
//  float zero_deadzone_halfsize = 0.0f;    // Only valid when is_logarithmic is true
//  if (is_logarithmic) {
//    // When using logarithmic sliders, we need to clamp to avoid hitting zero, but our choice of clamp value greatly affects slider precision. We
//    // attempt to use the specified precision to estimate a good lower bound.
//    const int decimal_precision = is_floating_point ? ImParseFormatPrecision(format, 3) : 1;
//    logarithmic_zero_epsilon = ImPow(0.1f, (float)decimal_precision);
//    zero_deadzone_halfsize = (style.LogSliderDeadzone * 0.5f) / ImMax(slider_usable_sz, 1.0f);
//  }
//
//  // Process interacting with the slider
//  bool value_changed = false;
//  if (g.ActiveId == id) {
//    bool set_new_value = false;
//    float clicked_t = 0.0f;
//    if (g.ActiveIdSource == ImGuiInputSource_Mouse) {
//      if (!g.IO.MouseDown[0]) {
//        ClearActiveID();
//      } else {
//        const float mouse_abs_pos = g.IO.MousePos[axis];
//        clicked_t = (slider_usable_sz > 0.0f) ? ImClamp((mouse_abs_pos - slider_usable_pos_min) / slider_usable_sz, 0.0f, 1.0f) : 0.0f;
//        if (axis == ImGuiAxis_Y) clicked_t = 1.0f - clicked_t;
//        set_new_value = true;
//      }
//    } else if (g.ActiveIdSource == ImGuiInputSource_Nav) {
//      if (g.ActiveIdIsJustActivated) {
//        g.SliderCurrentAccum = 0.0f;  // Reset any stored nav delta upon activation
//        g.SliderCurrentAccumDirty = false;
//      }
//
//      const ImVec2 input_delta2 =
//          GetNavInputAmount2d(ImGuiNavDirSourceFlags_Keyboard | ImGuiNavDirSourceFlags_PadDPad, ImGuiInputReadMode_RepeatFast, 0.0f, 0.0f);
//      float input_delta = (axis == ImGuiAxis_X) ? input_delta2.x : -input_delta2.y;
//      if (input_delta != 0.0f) {
//        const int decimal_precision = is_floating_point ? ImParseFormatPrecision(format, 3) : 0;
//        if (decimal_precision > 0) {
//          input_delta /= 100.0f;  // Gamepad/keyboard tweak speeds in % of slider bounds
//          if (IsNavInputDown(ImGuiNavInput_TweakSlow)) input_delta /= 10.0f;
//        } else {
//          if ((v_range >= -100.0f && v_range <= 100.0f) || IsNavInputDown(ImGuiNavInput_TweakSlow))
//            input_delta = ((input_delta < 0.0f) ? -1.0f : +1.0f) / (float)v_range;  // Gamepad/keyboard tweak speeds in integer steps
//          else
//            input_delta /= 100.0f;
//        }
//        if (IsNavInputDown(ImGuiNavInput_TweakFast)) input_delta *= 10.0f;
//
//        g.SliderCurrentAccum += input_delta;
//        g.SliderCurrentAccumDirty = true;
//      }
//
//      float delta = g.SliderCurrentAccum;
//      if (g.NavActivatePressedId == id && !g.ActiveIdIsJustActivated) {
//        ClearActiveID();
//      } else if (g.SliderCurrentAccumDirty) {
//        clicked_t = ScaleRatioFromValueT<TYPE, SIGNEDTYPE, FLOATTYPE>(data_type, *v, v_min, v_max, is_logarithmic, logarithmic_zero_epsilon,
//                                                                      zero_deadzone_halfsize);
//
//        if ((clicked_t >= 1.0f && delta > 0.0f) ||
//            (clicked_t <= 0.0f && delta < 0.0f))  // This is to avoid applying the saturation when already past the limits
//        {
//          set_new_value = false;
//          g.SliderCurrentAccum = 0.0f;  // If pushing up against the limits, don't continue to accumulate
//        } else {
//          set_new_value = true;
//          float old_clicked_t = clicked_t;
//          clicked_t = ImSaturate(clicked_t + delta);
//
//          // Calculate what our "new" clicked_t will be, and thus how far we actually moved the slider, and subtract this from the accumulator
//          TYPE v_new = ScaleValueFromRatioT<TYPE, SIGNEDTYPE, FLOATTYPE>(data_type, clicked_t, v_min, v_max, is_logarithmic, logarithmic_zero_epsilon,
//                                                                         zero_deadzone_halfsize);
//          if (!(flags & ImGuiSliderFlags_NoRoundToFormat)) v_new = RoundScalarWithFormatT<TYPE, SIGNEDTYPE>(format, data_type, v_new);
//          float new_clicked_t = ScaleRatioFromValueT<TYPE, SIGNEDTYPE, FLOATTYPE>(data_type, v_new, v_min, v_max, is_logarithmic,
//                                                                                  logarithmic_zero_epsilon, zero_deadzone_halfsize);
//
//          if (delta > 0)
//            g.SliderCurrentAccum -= ImMin(new_clicked_t - old_clicked_t, delta);
//          else
//            g.SliderCurrentAccum -= ImMax(new_clicked_t - old_clicked_t, delta);
//        }
//
//        g.SliderCurrentAccumDirty = false;
//      }
//    }
//
//    if (set_new_value) {
//      TYPE v_new = ScaleValueFromRatioT<TYPE, SIGNEDTYPE, FLOATTYPE>(data_type, clicked_t, v_min, v_max, is_logarithmic, logarithmic_zero_epsilon,
//                                                                     zero_deadzone_halfsize);
//
//      // Round to user desired precision based on format string
//      if (!(flags & ImGuiSliderFlags_NoRoundToFormat)) v_new = RoundScalarWithFormatT<TYPE, SIGNEDTYPE>(format, data_type, v_new);
//
//      // Apply result
//      if (*v != v_new) {
//        *v = v_new;
//        value_changed = true;
//      }
//    }
//  }
//
//  if (slider_sz < 1.0f) {
//    *out_grab_bb = ImRect(bb.Min, bb.Min);
//  } else {
//    // Output grab position so it can be displayed by the caller
//    float grab_t = ScaleRatioFromValueT<TYPE, SIGNEDTYPE, FLOATTYPE>(data_type, *v, v_min, v_max, is_logarithmic, logarithmic_zero_epsilon,
//                                                                     zero_deadzone_halfsize);
//    if (axis == ImGuiAxis_Y) grab_t = 1.0f - grab_t;
//    const float grab_pos = ImLerp(slider_usable_pos_min, slider_usable_pos_max, grab_t);
//    if (axis == ImGuiAxis_X)
//      *out_grab_bb = ImRect(grab_pos - grab_sz * 0.5f, bb.Min.y + grab_padding, grab_pos + grab_sz * 0.5f, bb.Max.y - grab_padding);
//    else
//      *out_grab_bb = ImRect(bb.Min.x + grab_padding, grab_pos - grab_sz * 0.5f, bb.Max.x - grab_padding, grab_pos + grab_sz * 0.5f);
//  }
//  *percent = floorf((float)(*v - v_min) / (float)(v_max - v_min) * 100.f);
//  return value_changed;
//}

// For 32-bit and larger types, slider bounds are limited to half the natural type range.
// So e.g. an integer Slider between INT_MAX-10 and INT_MAX will fail, but an integer Slider between INT_MAX/2-10 and INT_MAX/2 will be ok.
// It would be possible to lift that limitation with some work but it doesn't seem to be worth it for sliders.
//bool SliderBehaviorB(const ImRect &bb, ImGuiID id, ImGuiDataType data_type, void *p_v, const void *p_min, const void *p_max, const char *format,
//                     ImGuiSliderFlags flags, ImRect *out_grab_bb, float *percent) {
//  // Read imgui.cpp "API BREAKING CHANGES" section for 1.78 if you hit this assert.
//  IM_ASSERT((flags == 1 || (flags & ImGuiSliderFlags_InvalidMask_) == 0) &&
//            "Invalid ImGuiSliderFlags flag!  Has the 'float power' argument been mistakenly cast to flags? Call function with "
//            "ImGuiSliderFlags_Logarithmic flags instead.");
//
//  ImGuiContext &g = *GImGui;
//  if ((g.LastItemData.InFlags & ImGuiItemFlags_ReadOnly) || (flags & ImGuiSliderFlags_ReadOnly)) return false;
//
//  switch (data_type) {
//    case ImGuiDataType_S8: {
//      ImS32 v32 = (ImS32) * (ImS8 *)p_v;
//      bool r = SliderBehaviorT<ImS32, ImS32, float>(bb, id, ImGuiDataType_S32, &v32, *(const ImS8 *)p_min, *(const ImS8 *)p_max, format, flags,
//                                                    out_grab_bb, percent);
//      if (r) *(ImS8 *)p_v = (ImS8)v32;
//      return r;
//    }
//    case ImGuiDataType_U8: {
//      ImU32 v32 = (ImU32) * (ImU8 *)p_v;
//      bool r = SliderBehaviorT<ImU32, ImS32, float>(bb, id, ImGuiDataType_U32, &v32, *(const ImU8 *)p_min, *(const ImU8 *)p_max, format, flags,
//                                                    out_grab_bb, percent);
//      if (r) *(ImU8 *)p_v = (ImU8)v32;
//      return r;
//    }
//    case ImGuiDataType_S16: {
//      ImS32 v32 = (ImS32) * (ImS16 *)p_v;
//      bool r = SliderBehaviorT<ImS32, ImS32, float>(bb, id, ImGuiDataType_S32, &v32, *(const ImS16 *)p_min, *(const ImS16 *)p_max, format, flags,
//                                                    out_grab_bb, percent);
//      if (r) *(ImS16 *)p_v = (ImS16)v32;
//      return r;
//    }
//    case ImGuiDataType_U16: {
//      ImU32 v32 = (ImU32) * (ImU16 *)p_v;
//      bool r = SliderBehaviorT<ImU32, ImS32, float>(bb, id, ImGuiDataType_U32, &v32, *(const ImU16 *)p_min, *(const ImU16 *)p_max, format, flags,
//                                                    out_grab_bb, percent);
//      if (r) *(ImU16 *)p_v = (ImU16)v32;
//      return r;
//    }
//    case ImGuiDataType_S32:
//      IM_ASSERT(*(const ImS32 *)p_min >= IM_S32_MIN / 2 && *(const ImS32 *)p_max <= IM_S32_MAX / 2);
//      return SliderBehaviorT<ImS32, ImS32, float>(bb, id, data_type, (ImS32 *)p_v, *(const ImS32 *)p_min, *(const ImS32 *)p_max, format, flags,
//                                                  out_grab_bb, percent);
//    case ImGuiDataType_U32:
//      IM_ASSERT(*(const ImU32 *)p_max <= IM_U32_MAX / 2);
//      return SliderBehaviorT<ImU32, ImS32, float>(bb, id, data_type, (ImU32 *)p_v, *(const ImU32 *)p_min, *(const ImU32 *)p_max, format, flags,
//                                                  out_grab_bb, percent);
//    case ImGuiDataType_S64:
//      IM_ASSERT(*(const ImS64 *)p_min >= IM_S64_MIN / 2 && *(const ImS64 *)p_max <= IM_S64_MAX / 2);
//      return SliderBehaviorT<ImS64, ImS64, double>(bb, id, data_type, (ImS64 *)p_v, *(const ImS64 *)p_min, *(const ImS64 *)p_max, format, flags,
//                                                   out_grab_bb, percent);
//    case ImGuiDataType_U64:
//      IM_ASSERT(*(const ImU64 *)p_max <= IM_U64_MAX / 2);
//      return SliderBehaviorT<ImU64, ImS64, double>(bb, id, data_type, (ImU64 *)p_v, *(const ImU64 *)p_min, *(const ImU64 *)p_max, format, flags,
//                                                   out_grab_bb, percent);
//    case ImGuiDataType_Float:
//      IM_ASSERT(*(const float *)p_min >= -FLT_MAX / 2.0f && *(const float *)p_max <= FLT_MAX / 2.0f);
//      return SliderBehaviorT<float, float, float>(bb, id, data_type, (float *)p_v, *(const float *)p_min, *(const float *)p_max, format, flags,
//                                                  out_grab_bb, percent);
//    case ImGuiDataType_Double:
//      IM_ASSERT(*(const double *)p_min >= -DBL_MAX / 2.0f && *(const double *)p_max <= DBL_MAX / 2.0f);
//      return SliderBehaviorT<double, double, double>(bb, id, data_type, (double *)p_v, *(const double *)p_min, *(const double *)p_max, format, flags,
//                                                     out_grab_bb, percent);
//    case ImGuiDataType_COUNT:
//      break;
//  }
//  IM_ASSERT(0);
//  return false;
//}

// Note: p_data, p_min and p_max are _pointers_ to a memory address holding the data. For a slider, they are all required.
// Read code of e.g. SliderFloat(), SliderInt() etc. or examples in 'Demo->Widgets->Data Types' to understand how to use this function directly.
// Note: p_data, p_min and p_max are _pointers_ to a memory address holding the data. For a slider, they are all required.
// Read code of e.g. SliderFloat(), SliderInt() etc. or examples in 'Demo->Widgets->Data Types' to understand how to use this function directly.
bool SliderScalar(const char *label, ImGuiDataType data_type, void *p_data, const void *p_min, const void *p_max, const char *format,
                   ImGuiSliderFlags flags) {
  ImGuiWindow *window = GetCurrentWindow();
  if (window->SkipItems) return false;

  ImGuiContext &g = *GImGui;
  const ImGuiStyle &style = g.Style;
  const ImGuiID id = window->GetID(label);
  const ImGuiID id_i = window->GetID(std::string(std::string(label) + "_inpit").c_str());
  const float w = ImGui::GetColumnWidth();

  const ImVec2 label_size = CalcTextSize(label, NULL, true);
  const ImRect frame_bb(window->DC.CursorPos, window->DC.CursorPos + ImVec2(w, 27));
  const ImRect slider_bb(frame_bb.Min + ImVec2(w - 135, 0.f), ImVec2(frame_bb.Min.x + window->Size.x - 46.f, frame_bb.Max.y) + ImVec2(0.0f, 0.0f));
  const ImRect text(ImVec2(window->DC.CursorPos.x + w - 38, frame_bb.Min.y + 5.f), ImVec2(window->DC.CursorPos.x + w - 10, frame_bb.Max.y - 5.f));
  const ImRect input(ImVec2(window->DC.CursorPos.x + w - 38, frame_bb.Min.y), ImVec2(window->DC.CursorPos.x + w - 10, frame_bb.Max.y));
  const ImRect total_bb(frame_bb.Min, text.Max);
  ItemSize(slider_bb, style.FramePadding.y);
  bool can_add = ItemAdd(slider_bb, id, &total_bb);

  // Default format string when passing NULL
  if (format == NULL)
    format = DataTypeGetInfo(data_type)->PrintFmt;
  else if (data_type == ImGuiDataType_S32 &&
           strcmp(format, "%d") != 0)  // (FIXME-LEGACY: Patch old "%.0f" format string to use "%d", read function more details.)
    format = PatchFormatStringFloatToInt(format);
  const bool temp_input_allowed = (flags & ImGuiSliderFlags_NoInput) == 0;
  // Tabbing or CTRL-clicking on Slider turns it into an input box
  const bool hovered = /*ItemHoverable( text, id ) || */ ItemHoverable(slider_bb, id, ImGuiItemFlags_None);
  bool temp_input_is_active = temp_input_allowed && TempInputIsActive(id);
  if (!temp_input_is_active) {
    const bool input_requested_by_tabbing = temp_input_allowed && (g.LastItemData.StatusFlags & ImGuiItemStatusFlags_FocusedByTabbing) != 0;
    const bool clicked = (hovered && g.IO.MouseClicked[0]);
    if (input_requested_by_tabbing || clicked || g.NavActivateId == id) {
      SetActiveID(id, window);
      SetFocusID(id, window);
      FocusWindow(window);
      g.ActiveIdUsingNavDirMask |= (1 << ImGuiDir_Left) | (1 << ImGuiDir_Right);
      if (temp_input_allowed && (input_requested_by_tabbing || (clicked && g.IO.KeyCtrl))) temp_input_is_active = true;
    }
  }

  struct SliderAnimation {
    ImColor text_color;
    float circle_size;
    ImColor slider_bg;
    ImColor Text;
    ImColor slider_grab;
    float position;
    float hovered;
  };
  static std::map<ImGuiID, SliderAnimation> AnimationMap;
  auto AnimationHandle = AnimationMap.find(ImColor());

  if (AnimationHandle == AnimationMap.end()) {
    AnimationMap.insert({ImGui::GetItemID(), SliderAnimation()});
    AnimationHandle = AnimationMap.find(ImGui::GetItemID());
  }

  AnimationHandle->second.slider_bg =
      ImLerp(AnimationHandle->second.slider_bg,
             hovered || g.ActiveId == id ? ImColor(42, 44, 48, int(255 * g.Style.Alpha)).Value : ImColor(24, 24, 27, int(255 * g.Style.Alpha)).Value,
             g.IO.DeltaTime * 8.f * AnimationSpeed);
  AnimationHandle->second.Text =
      ImLerp(AnimationHandle->second.Text, hovered || g.ActiveId == id ? Colors::TextActive : Colors::Text, g.IO.DeltaTime * 8.f * AnimationSpeed);
  AnimationHandle->second.hovered =
      ImLerp(AnimationHandle->second.hovered, hovered || g.ActiveId == id ? 1.f : 0.f, g.IO.DeltaTime * 8.f * AnimationSpeed);
  AnimationHandle->second.slider_grab = ImLerp(Colors::AccentDisabled, Colors::Accent, AnimationHandle->second.hovered);
  // AnimationHandle->second.mark_bg = ImLerp( AnimationHandle->second.mark_bg, g.Style.Alpha >= 0.5f && can_add && !window->SkipItems && *v ?
  // ImColor( ( 130 / 3.44f ) / 255.f, ( 132 / 3.44f ) / 255.f, ( 170 / 3.44f ) / 255.f,  g.Style.Alpha ).Value : g.Style.Alpha >= 0.5f && can_add &&
  // !window->SkipItems && ( hovered || hovered_mark ) ? ImColor( 42, 44, 48, int( 255 * g.Style.Alpha ) ).Value : ImColor( 24, 24, 27, int( 255 *
  // g.Style.Alpha ) ).Value, g.IO.DeltaTime * 8.f );

  // Draw frame
  RenderNavHighlight(slider_bb, id);
  RenderFrame(ImVec2(slider_bb.Min.x, slider_bb.GetCenter().y - 1.5f), ImVec2(slider_bb.Max.x, slider_bb.GetCenter().y + 1.5f),
              ImColor(Colors::Borders), false, 8.f);

  // Slider behavior
  ImRect grab_bb;
  float percent = 0;
  bool value_changed = SliderBehavior(slider_bb, id, data_type, p_data, p_min, p_max, format, ImGuiSliderFlags_None, &grab_bb);
  if (value_changed) MarkItemEdited(id);
  // if ( !( temp_input_is_active || temp_input_is_active ) )
  AnimationHandle->second.position =
      ImLerp(AnimationHandle->second.position, g.Style.Alpha < 0.5f || !can_add ? 0.f : (float)percent, g.IO.DeltaTime * 12.f * AnimationSpeed);

  // Render grab
  {
    window->DrawList->AddRectFilled(
        ImVec2(slider_bb.Min.x, slider_bb.GetCenter().y - 1.5f),
        ImVec2(slider_bb.Min.x + (slider_bb.GetSize().x - 4.f) * AnimationHandle->second.position / 100.f, slider_bb.GetCenter().y + 1.5f),
        ImColor(Colors::Accent.x, Colors::Accent.y, Colors::Accent.z, Colors::Accent.w * 0.5f), 8.f);
    window->DrawList->AddCircleFilled(
        ImVec2(slider_bb.Min.x + (slider_bb.GetSize().x - 4.f) * AnimationHandle->second.position / 100.f, slider_bb.GetCenter().y), 7.f,
        ImColor(Colors::Accent), 24);
  }

  // Display value using user-provided display format so user can add prefix/suffix/decorations to the value.
  char value_buf[64];
  const char *value_buf_end = value_buf + DataTypeFormatString(value_buf, IM_ARRAYSIZE(value_buf), data_type, p_data, format);

  if (label_size.x > 0.0f)
    window->DrawList->AddText(ImVec2(frame_bb.Min.x + 10.f, frame_bb.GetCenter().y - CalcTextSize(label).y / 2.f), AnimationHandle->second.Text,
                              label);

  bool changed_test = false;

  window->DrawList->AddRectFilled(text.Min, text.Max, ImColor(Colors::FrameHovered), 3.f);
  window->DrawList->AddRect(text.Min, text.Max, ImColor(Colors::Borders), 3.f);

  // if ( temp_input_is_active || temp_input_is_active )
  //     return TempInputScalar( input, id, label, data_type, p_data, format );

  // PushFont(g.IO.Fonts->Fonts[3]);
  window->DrawList->AddText(text.GetCenter() - CalcTextSize(value_buf) / 2.f, ImColor(Colors::TextActive), value_buf, value_buf_end);
  // PopFont();

  IMGUI_TEST_ENGINE_ITEM_INFO(id, label, window->DC.ItemFlags);
  return temp_input_is_active || temp_input_is_active ? changed_test : value_changed;
}

bool SliderInt(const char *label, int *v, int v_min, int v_max, const char *format, ImGuiSliderFlags flags) {
  return SliderScalar(label, ImGuiDataType_S32, v, &v_min, &v_max, format, flags);
}

static float CalcMaxPopupHeightFromItemCount(int items_count) {
  ImGuiContext &g = *GImGui;
  if (items_count <= 0) return FLT_MAX;
  return (g.FontSize + g.Style.ItemSpacing.y) * items_count - g.Style.ItemSpacing.y + (g.Style.WindowPadding.y * 2);
}

// Getter for the old Combo() API: "item1\0item2\0item3\0"
static bool Items_SingleStringGetter(void *data, int idx, const char **out_text) {
  // FIXME-OPT: we could pre-compute the indices to fasten this. But only 1 active combo means the waste is limited.
  const char *items_separated_by_zeros = (const char *)data;
  int items_count = 0;
  const char *p = items_separated_by_zeros;
  while (*p) {
    if (idx == items_count) break;
    p += strlen(p) + 1;
    items_count++;
  }
  if (!*p) return false;
  if (out_text) *out_text = p;
  return true;
}

bool BeginComboPopupB(ImGuiID popup_id, const ImRect &bb, ImGuiComboFlags flags, float alpha) {
  ImGuiContext &g = *GImGui;
  if (!IsPopupOpen(popup_id, ImGuiPopupFlags_None)) {
    g.NextWindowData.ClearFlags();
    return false;
  }

  // Set popup size
  float w = bb.GetWidth();
  if (g.NextWindowData.Flags & ImGuiNextWindowDataFlags_HasSizeConstraint) {
    g.NextWindowData.SizeConstraintRect.Min.x = ImMax(g.NextWindowData.SizeConstraintRect.Min.x, w);
  } else {
    if ((flags & ImGuiComboFlags_HeightMask_) == 0) flags |= ImGuiComboFlags_HeightRegular;
    IM_ASSERT(ImIsPowerOfTwo(flags & ImGuiComboFlags_HeightMask_));  // Only one
    int popup_max_height_in_items = -1;
    if (flags & ImGuiComboFlags_HeightRegular)
      popup_max_height_in_items = 8;
    else if (flags & ImGuiComboFlags_HeightSmall)
      popup_max_height_in_items = 4;
    else if (flags & ImGuiComboFlags_HeightLarge)
      popup_max_height_in_items = 20;
    SetNextWindowSizeConstraints(ImVec2(w, 0.0f), ImVec2(w, CalcMaxPopupHeightFromItemCount(popup_max_height_in_items)));
  }

  // This is essentially a specialized version of BeginPopupEx()
  char name[16];
  ImFormatString(name, IM_ARRAYSIZE(name), "##Combo_%02d", g.BeginPopupStack.Size);  // Recycle windows based on depth

  // Set position given a custom constraint (peak into expected window size so we can position it)
  // FIXME: This might be easier to express with an hypothetical SetNextWindowPosConstraints() function?
  // FIXME: This might be moved to Begin() or at least around the same spot where Tooltips and other Popups are calling FindBestWindowPosForPopupEx()?
  if (ImGuiWindow *popup_window = FindWindowByName(name))
    if (popup_window->WasActive) {
      // Always override 'AutoPosLastDirection' to not leave a chance for a past value to affect us.
      ImVec2 size_expected = CalcWindowNextAutoFitSize(popup_window);
      popup_window->AutoPosLastDirection = (flags & ImGuiComboFlags_PopupAlignLeft)
                                               ? ImGuiDir_Left
                                               : ImGuiDir_Down;  // Left = "Below, Toward Left", Down = "Below, Toward Right (default)"
      ImRect r_outer = GetPopupAllowedExtentRect(popup_window);
      ImVec2 pos =
          FindBestWindowPosForPopupEx(bb.GetBL(), size_expected, &popup_window->AutoPosLastDirection, r_outer, bb, ImGuiPopupPositionPolicy_ComboBox);
      SetNextWindowPos(pos);
    }

  // We don't use BeginPopupEx() solely because we have a custom name string, which we could make an argument to BeginPopupEx()
  ImGuiWindowFlags window_flags = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_Popup | ImGuiWindowFlags_NoTitleBar |
                                  ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoMove;

  PushStyleVar(ImGuiStyleVar_Alpha, alpha);
  PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(g.Style.FramePadding.x, 2));  // Horizontally align ourselves with the framed text
  bool ret = Begin(name, NULL, window_flags);
  PopStyleVar();
  if (!ret) {
    EndPopup();
    PopStyleVar();
    IM_ASSERT(0);  // This should never happen as we tested for IsPopupOpen() above
    return false;
  }
  return true;
}

bool BeginComboB(const char *label, const char *preview_value, ImGuiComboFlags flags) {
  ImGuiContext &g = *GImGui;
  ImGuiWindow *window = GetCurrentWindow();

  ImGuiNextWindowDataFlags backup_next_window_data_flags = g.NextWindowData.Flags;
  g.NextWindowData.ClearFlags();  // We behave like Begin() and need to consume those values
  if (window->SkipItems) return false;

  const ImGuiStyle &style = g.Style;
  const ImGuiID id = window->GetID(label);
  IM_ASSERT((flags & (ImGuiComboFlags_NoArrowButton | ImGuiComboFlags_NoPreview)) !=
            (ImGuiComboFlags_NoArrowButton | ImGuiComboFlags_NoPreview));  // Can't use both flags together

  const float arrow_size = (flags & ImGuiComboFlags_NoArrowButton) ? 0.0f : GetFrameHeight();
  const ImVec2 label_size = CalcTextSize(label, NULL, true);
  const float w = ImGui::GetColumnWidth();
  const ImRect bb(window->DC.CursorPos, window->DC.CursorPos + ImVec2(w, label_size.y + style.FramePadding.y * 2.0f + 5.f));
  const ImRect total_bb(bb.Min, bb.Max + ImVec2(label_size.x > 0.0f ? style.ItemInnerSpacing.x + label_size.x : 0.0f, 0.0f));
  const ImRect combo_bb(bb.Min + ImVec2(1.5*w - ImGetWidth(), 3.f), ImVec2(bb.Min.x + w - 10.f, bb.Max.y - 3));
  ItemSize(bb, style.FramePadding.y);
  if (!ItemAdd(total_bb, id, &bb)) return false;

  // Open on click
  bool hovered, held;
  bool pressed = ButtonBehavior(bb, id, &hovered, &held);
  const ImGuiID popup_id = ImHashStr("##ComboPopup", 0, id);
  bool popup_open = IsPopupOpen(popup_id, ImGuiPopupFlags_None);
  if (pressed && !popup_open) {
    OpenPopupEx(popup_id, ImGuiPopupFlags_None);
    popup_open = true;
  }

  struct TabAnimation {
    ImColor ComboFrame;
    ImColor Border;
    ImColor Text;
    float OpenAlpha;
  };

  static std::map<ImGuiID, TabAnimation> TabMap;
  auto TabItemMap = TabMap.find(id);
  if (TabItemMap == TabMap.end()) {
    TabMap.insert({id, TabAnimation()});
    TabItemMap = TabMap.find(id);
  }
  TabItemMap->second.ComboFrame =
      ImLerp(TabItemMap->second.ComboFrame.Value, popup_open ? Colors::FrameOpened : Colors::FrameHovered, g.IO.DeltaTime * 8.f * AnimationSpeed);
  TabItemMap->second.Text =
      ImLerp(TabItemMap->second.Text.Value, popup_open ? Colors::TextActive : Colors::Text, g.IO.DeltaTime * 8.f * AnimationSpeed);
  TabItemMap->second.Border =
      ImLerp(TabItemMap->second.Border.Value, popup_open ? Colors::FrameOpened : Colors::Borders, g.IO.DeltaTime * 8.f * AnimationSpeed);
  TabItemMap->second.OpenAlpha = ImLerp(TabItemMap->second.OpenAlpha, popup_open ? 1.f : 0.f, g.IO.DeltaTime * 8.f * AnimationSpeed);

  // Render shape
  const ImU32 frame_col = GetColorU32(hovered ? ImGuiCol_FrameBgHovered : ImGuiCol_FrameBg);
  const float value_x2 = ImMax(bb.Min.x, bb.Max.x - arrow_size);
  RenderNavHighlight(combo_bb, id);
  if (!(flags & ImGuiComboFlags_NoPreview)) {
    window->DrawList->AddRectFilled(combo_bb.Min, combo_bb.Max, TabItemMap->second.ComboFrame, 4.f,
                                    popup_open ? ImDrawFlags_RoundCornersTop : ImDrawFlags_RoundCornersAll);
    window->DrawList->AddRect(combo_bb.Min, combo_bb.Max, TabItemMap->second.Border, 4.f,
                              popup_open ? ImDrawFlags_RoundCornersTop : ImDrawFlags_RoundCornersAll);
  }
  if (!(flags & ImGuiComboFlags_NoArrowButton)) {
    const auto &arrow_pos = ImVec2(combo_bb.Max.x - 12, combo_bb.Min.y + 10 - 1.5f);

    float rot_angle = 0.75f * IM_PI;

    float cos_a = cosf(rot_angle);
    float sin_a = sinf(rot_angle);

    const ImVec2 points[] = {arrow_pos + ImRotate(ImVec2(-5 * 0.5f, -5 * 0.5f), cos_a, sin_a),
                             arrow_pos + ImRotate(ImVec2(5 * 0.5f, -5 * 0.5f), cos_a, sin_a),
                             arrow_pos + ImRotate(ImVec2(5 * 0.5f, 5 * 0.5f), cos_a, sin_a)};

    ImU32 bg_col = GetColorU32((popup_open || hovered) ? ImGuiCol_ButtonHovered : ImGuiCol_Button);
    window->DrawList->AddPolyline(points, 3, ImColor(Colors::TextActive), 0, 2.f);
  }

  // Custom preview
  if (flags & ImGuiComboFlags_CustomPreview) {
    g.ComboPreviewData.PreviewRect = ImRect(bb.Min.x, bb.Min.y, value_x2, bb.Max.y);
    IM_ASSERT(preview_value == NULL || preview_value[0] == 0);
    preview_value = NULL;
  }

  // Render preview and label
  if (preview_value != NULL && !(flags & ImGuiComboFlags_NoPreview)) {
    if (g.LogEnabled) LogSetNextTextDecoration("{", "}");
    PushStyleColor(ImGuiCol_Text, TabItemMap->second.Text.Value);
    RenderTextClipped(ImVec2(combo_bb.Min.x + 5.f, combo_bb.Min.y + 2.5f), combo_bb.Max - ImVec2(20, 0), preview_value, NULL, NULL);
    PopStyleColor();
  }
  if (label_size.x > 0)
    window->DrawList->AddText(ImVec2(bb.Min.x + 10.f, bb.GetCenter().y - CalcTextSize(label).y / 2.f), ImColor(TabItemMap->second.Text), label);

  if (!popup_open) return false;

  g.NextWindowData.Flags = backup_next_window_data_flags;
  return BeginComboPopupB(popup_id, combo_bb, flags, TabItemMap->second.OpenAlpha);
}

void EndComboB() {
  EndPopup();
  PopStyleVar();
}

// Old API, prefer using BeginCombo() nowadays if you can.
bool ComboB(const char *label, int *current_item, bool (*items_getter)(void *, int, const char **), void *data, int items_count,
            int popup_max_height_in_items) {
  ImGuiContext &g = *GImGui;

  // Call the getter to obtain the preview string which is a parameter to BeginCombo()
  const char *preview_value = NULL;
  if (*current_item >= 0 && *current_item < items_count) items_getter(data, *current_item, &preview_value);

  // The old Combo() API exposed "popup_max_height_in_items". The new more general BeginCombo() API doesn't have/need it, but we emulate it here.
  if (popup_max_height_in_items != -1 && !(g.NextWindowData.Flags & ImGuiNextWindowDataFlags_HasSizeConstraint))
    SetNextWindowSizeConstraints(ImVec2(0, 0), ImVec2(FLT_MAX, CalcMaxPopupHeightFromItemCount(popup_max_height_in_items)));

  if (!BeginComboB(label, preview_value, ImGuiComboFlags_None)) return false;

  // Display items
  // FIXME-OPT: Use clipper (but we need to disable it on the appearing frame to make sure our call to SetItemDefaultFocus() is processed)
  bool value_changed = false;
  for (int i = 0; i < items_count; i++) {
    PushID(i);
    const bool item_selected = (i == *current_item);
    const char *item_text;
    if (!items_getter(data, i, &item_text)) item_text = "*Unknown item*";
    if (Selectable(item_text, item_selected)) {
      value_changed = true;
      *current_item = i;
    }
    if (item_selected) SetItemDefaultFocus();
    PopID();
  }

  EndComboB();

  if (value_changed) MarkItemEdited(g.LastItemData.ID);

  return value_changed;
}

// Combo box helper allowing to pass all items in a single string literal holding multiple zero-terminated items "item1\0item2\0"
bool Combo(const char *label, int *current_item, const char *items_separated_by_zeros, int height_in_items) {
  int items_count = 0;
  const char *p = items_separated_by_zeros;  // FIXME-OPT: Avoid computing this, or at least only when combo is open
  while (*p) {
    p += strlen(p) + 1;
    items_count++;
  }
  bool value_changed = ComboB(label, current_item, Items_SingleStringGetter, (void *)items_separated_by_zeros, items_count, height_in_items);
  return value_changed;
}

bool ToggleA(const char *label, bool *v) {
  ImGuiWindow *window = GetCurrentWindow();
  if (window->SkipItems) return false;

  ImGuiContext &g = *GImGui;
  const ImGuiStyle &style = g.Style;
  const ImGuiID id = window->GetID(label);
  const ImVec2 label_size = CalcTextSize(label, NULL, true);

  float height = ImGui::GetFrameHeight();
  const ImVec2 pos = window->DC.CursorPos;

  float width = height * 2.f;
  float radius = height * 0.50f;

  const ImRect total_bb(
      pos, pos + ImVec2(width + (label_size.x > 0.0f ? style.ItemInnerSpacing.x + label_size.x : 0.0f), label_size.y + style.FramePadding.y * 2.0f));

  ItemSize(total_bb, style.FramePadding.y);
  if (!ItemAdd(total_bb, id)) return false;

  float last_active_id_timer = g.LastActiveIdTimer;

  bool hovered, held;
  bool pressed = ButtonBehavior(total_bb, id, &hovered, &held);
  if (pressed) {
    *v = !(*v);
    MarkItemEdited(id);
    g.LastActiveIdTimer = 0.f;
  }

  if (g.LastActiveIdTimer == 0.f && g.LastActiveId == id && !pressed) g.LastActiveIdTimer = last_active_id_timer;

  float t = *v ? 1.0f : 0.0f;

  if (g.LastActiveId == id) {
    float t_anim = ImSaturate(g.LastActiveIdTimer / 0.16f);
    t = *v ? (t_anim) : (1.0f - t_anim);
  }

  ImU32 col_bg = GetColorU32((held && hovered) ? ImGuiCol_FrameBgActive : hovered ? ImGuiCol_FrameBgHovered : ImGuiCol_FrameBg);

  const ImRect frame_bb(pos, pos + ImVec2(width, height));

  RenderFrame(frame_bb.Min, frame_bb.Max, col_bg, true, height * 0.5f);
  RenderNavHighlight(total_bb, id);

  ImVec2 label_pos = ImVec2(frame_bb.Max.x + style.ItemInnerSpacing.x, frame_bb.Min.y + style.FramePadding.y);
  RenderText(label_pos, label);
  window->DrawList->AddCircleFilled(ImVec2(pos.x + radius + t * (width - radius * 2.0f), pos.y + radius), radius - 1.5f,
                                    ImGui::GetColorU32(ImGuiCol_CheckMark), 36);

  return pressed;
}

bool ButtonScrollableEx(const char *label, const ImVec2 &size_arg, ImGuiButtonFlags flags) {
  ImGuiWindow *window = GetCurrentWindow();
  if (window->SkipItems) return false;

  ImGuiContext &g = *GImGui;
  const ImGuiStyle &style = g.Style;
  const ImGuiID id = window->GetID(label);
  const ImVec2 label_size = CalcTextSize(label, NULL, true);

  ImVec2 pos = window->DC.CursorPos;
  if ((flags & ImGuiButtonFlags_AlignTextBaseLine) &&
      style.FramePadding.y < window->DC.CurrLineTextBaseOffset)  // Try to vertically align buttons that are smaller/have no padding so that text
                                                                 // baseline matches (bit hacky, since it shouldn't be a flag)
    pos.y += window->DC.CurrLineTextBaseOffset - style.FramePadding.y;
  ImVec2 size = CalcItemSize(size_arg, label_size.x + style.FramePadding.x * 2.0f, label_size.y + style.FramePadding.y * 2.0f);

  const ImRect bb(pos, pos + size);
  ItemSize(size, style.FramePadding.y);
  if (!ItemAdd(bb, id)) return false;

  if (g.LastItemData.InFlags & ImGuiItemFlags_ButtonRepeat) flags |= ImGuiButtonFlags_Repeat;

  bool hovered, held;
  bool pressed = ButtonBehavior(bb, id, &hovered, &held, flags);

  const ImU32 col = GetColorU32((held && hovered) ? ImGuiCol_ButtonActive : hovered ? ImGuiCol_ButtonHovered : ImGuiCol_Button);
  RenderNavHighlight(bb, id);
  RenderFrame(bb.Min, bb.Max, col, true, style.FrameRounding);

  const float offset = size.x >= label_size.x + style.FramePadding.x * 2.0f
                           ? size.x + style.FramePadding.x
                           : static_cast<int>(g.Time * 60.f) % static_cast<int>(label_size.x + size.x + style.FramePadding.x * 2.f + 4);
  const ImRect text =
      ImRect(ImVec2(bb.Min.x + size.x - offset + style.FramePadding.x * 2.f, bb.Min.y + style.FramePadding.y), bb.Max - style.FramePadding);

  RenderTextClipped(text.Min, text.Max, label, NULL, &label_size,
                    size.x >= label_size.x + style.FramePadding.x * 2.0f ? g.Style.ButtonTextAlign : ImVec2(0, 0), &bb);
  return pressed;
}

bool ButtonScrollable(const char *label, const ImVec2 &size_arg) { return ButtonScrollableEx(label, size_arg, ImGuiButtonFlags_None); }

bool Toggle(const char *label, bool *v) {
  ImGuiWindow *window = GetCurrentWindow();
  if (window->SkipItems) return false;

  ImGuiContext &g = *GImGui;
  const ImGuiStyle &style = g.Style;
  const ImGuiID id = window->GetID(label);
  const ImVec2 label_size = CalcTextSize(label, NULL, true);

  float height = ImGui::GetFrameHeight();
  const ImVec2 pos = window->DC.CursorPos;

  float width = height * 2.f;
  float radius = height * 0.50f;

  const ImRect total_bb(
      pos, pos + ImVec2(width + (label_size.x > 0.0f ? style.ItemInnerSpacing.x + label_size.x : 0.0f), label_size.y + style.FramePadding.y * 2.0f));

  ItemSize(total_bb, style.FramePadding.y);
  if (!ItemAdd(total_bb, id)) return false;

  float last_active_id_timer = g.LastActiveIdTimer;

  bool hovered, held;
  bool pressed = ButtonBehavior(total_bb, id, &hovered, &held);
  if (pressed) {
    *v = !(*v);
    MarkItemEdited(id);
    g.LastActiveIdTimer = 0.f;
  }

  if (g.LastActiveIdTimer == 0.f && g.LastActiveId == id && !pressed) g.LastActiveIdTimer = last_active_id_timer;

  float t = *v ? 1.0f : 0.0f;

  if (g.LastActiveId == id) {
    float t_anim = ImSaturate(g.LastActiveIdTimer / 0.16f);
    t = *v ? (t_anim) : (1.0f - t_anim);
  }

  ImU32 col_bg = GetColorU32((held && hovered) ? ImGuiCol_FrameBgActive : hovered ? ImGuiCol_FrameBgHovered : ImGuiCol_FrameBg);

  const ImRect frame_bb(pos, pos + ImVec2(width, height));

  RenderFrame(frame_bb.Min, frame_bb.Max, col_bg, true, height * 0.5f);
  RenderNavHighlight(total_bb, id);

  ImVec2 label_pos = ImVec2(frame_bb.Max.x + style.ItemInnerSpacing.x, frame_bb.Min.y + style.FramePadding.y);
  RenderText(label_pos, label);
  window->DrawList->AddCircleFilled(ImVec2(pos.x + radius + t * (width - radius * 2.0f), pos.y + radius), radius - 1.5f,
                                    ImGui::GetColorU32(ImGuiCol_CheckMark), 36);

  return pressed;
}
}  // namespace ImGuiB
