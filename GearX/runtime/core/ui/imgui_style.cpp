#include "imgui_style.hpp"
#include <cmath>
#include <string>
#include <filesystem>
float GearX::AnimateScale(float targetScale, float currentScale, float speed) {
	if (std::abs(targetScale - currentScale) < 0.001f) {
		return targetScale;
	}
	return currentScale + (targetScale - currentScale) * speed;
}

void GearX::SetImGuiWhiteStyle() {
	ImGuiStyle& style = ImGui::GetStyle();
	//设置字体
	std::string font_path = "./asset/default/Alibaba.ttf";
	if (std::filesystem::exists(std::filesystem::path(font_path))) {
		ImFont* font = ImGui::GetIO().Fonts->AddFontFromFileTTF(font_path.c_str(), 30.0f, nullptr,
			ImGui::GetIO().Fonts->GetGlyphRangesChineseFull());
		ImGui::GetIO().FontDefault = font;
	}
	else if (std::filesystem::exists(std::filesystem::path("C:/Windows/Fonts/simhei.ttf"))) {
		ImFont* font = ImGui::GetIO().Fonts->AddFontFromFileTTF("C:/Windows/Fonts/simhei.ttf", 30.0f, nullptr,
			ImGui::GetIO().Fonts->GetGlyphRangesChineseFull());
		ImGui::GetIO().FontDefault = font;
	
	}
	// 全局样式参数
	style.WindowPadding = ImVec2(20, 20);
	style.WindowRounding = 12.0f;
	style.WindowBorderSize = 1.0f;
	style.WindowMinSize = ImVec2(640, 400);
	style.WindowTitleAlign = ImVec2(0.5f, 0.5f);
	style.WindowMenuButtonPosition = ImGuiDir_Right;
	style.FramePadding = ImVec2(12, 10);
	style.FrameRounding = 10.0f;
	style.ItemSpacing = ImVec2(16, 12);
	style.ItemInnerSpacing = ImVec2(12, 10);
	style.IndentSpacing = 30.0f;
	style.ScrollbarSize = 20.0f;
	style.ScrollbarRounding = 10.0f;
	style.GrabMinSize = 14.0f;
	style.GrabRounding = 7.0f;
	style.TabRounding = 7.0f;
	style.ChildRounding = 10.0f;
	style.PopupRounding = 10.0f;
	style.DisplaySafeAreaPadding = ImVec2(4, 4);
	style.AntiAliasedLines = true;
	style.AntiAliasedFill = true;

	// 颜色设置
	ImVec4* colors = style.Colors;

	// 背景色
	colors[ImGuiCol_WindowBg] = ImVec4(0.99f, 0.99f, 0.99f, 1.00f);
	colors[ImGuiCol_ChildBg] = ImVec4(1.00f, 1.00f, 1.00f, 0.00f);
	colors[ImGuiCol_PopupBg] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	colors[ImGuiCol_MenuBarBg] = ImVec4(0.96f, 0.96f, 0.96f, 1.00f);
	colors[ImGuiCol_ScrollbarBg] = ImVec4(0.97f, 0.97f, 0.97f, 1.00f);
	colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.80f, 0.80f, 0.80f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.70f, 0.70f, 0.70f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
	colors[ImGuiCol_ResizeGrip] = ImVec4(0.93f, 0.93f, 0.93f, 0.25f);
	colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.83f, 0.83f, 0.83f, 0.67f);
	colors[ImGuiCol_ResizeGripActive] = ImVec4(0.73f, 0.73f, 0.73f, 0.95f);

	// 边框色
	colors[ImGuiCol_Border] = ImVec4(0.88f, 0.88f, 0.88f, 0.40f);
	colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);

	// 文本色
	colors[ImGuiCol_Text] = ImVec4(0.12f, 0.12f, 0.12f, 1.00f);
	colors[ImGuiCol_TextDisabled] = ImVec4(0.70f, 0.70f, 0.70f, 1.00f);

	// 控件背景色
	colors[ImGuiCol_FrameBg] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	colors[ImGuiCol_FrameBgHovered] = ImVec4(0.98f, 0.98f, 0.98f, 1.00f);
	colors[ImGuiCol_FrameBgActive] = ImVec4(0.96f, 0.96f, 0.96f, 1.00f);

	// 标题栏色
	colors[ImGuiCol_TitleBg] = ImVec4(0.96f, 0.96f, 0.96f, 1.00f);
	colors[ImGuiCol_TitleBgActive] = ImVec4(0.93f, 0.93f, 0.93f, 1.00f);
	colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.96f, 0.96f, 0.96f, 0.51f);

	// 按钮色
	colors[ImGuiCol_Button] = ImVec4(0.92f, 0.92f, 0.92f, 1.00f);
	colors[ImGuiCol_ButtonHovered] = ImVec4(0.90f, 0.90f, 0.90f, 1.00f);
	colors[ImGuiCol_ButtonActive] = ImVec4(0.88f, 0.88f, 0.88f, 1.00f);

	// 选中和激活状态色
	colors[ImGuiCol_CheckMark] = ImVec4(0.18f, 0.65f, 0.98f, 1.00f);
	colors[ImGuiCol_SliderGrab] = ImVec4(0.18f, 0.65f, 0.98f, 1.00f);
	colors[ImGuiCol_SliderGrabActive] = ImVec4(0.18f, 0.65f, 0.98f, 1.00f);
	colors[ImGuiCol_Header] = ImVec4(0.93f, 0.93f, 0.93f, 1.00f);
	colors[ImGuiCol_HeaderHovered] = ImVec4(0.90f, 0.90f, 0.90f, 1.00f);
	colors[ImGuiCol_HeaderActive] = ImVec4(0.87f, 0.87f, 0.87f, 1.00f);

	// 分割线色
	colors[ImGuiCol_Separator] = ImVec4(0.88f, 0.88f, 0.88f, 0.40f);
	colors[ImGuiCol_SeparatorHovered] = ImVec4(0.78f, 0.78f, 0.78f, 0.78f);
	colors[ImGuiCol_SeparatorActive] = ImVec4(0.78f, 0.78f, 0.78f, 1.00f);

	// 标签页色
	colors[ImGuiCol_Tab] = ImVec4(0.96f, 0.96f, 0.96f, 0.86f);
	colors[ImGuiCol_TabHovered] = ImVec4(0.93f, 0.93f, 0.93f, 1.00f);
	colors[ImGuiCol_TabActive] = ImVec4(0.90f, 0.90f, 0.90f, 1.00f);
	colors[ImGuiCol_TabUnfocused] = ImVec4(0.99f, 0.99f, 0.99f, 0.97f);
	colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.93f, 0.93f, 0.93f, 1.00f);

	// 图表色
	colors[ImGuiCol_PlotLines] = ImVec4(0.32f, 0.32f, 0.32f, 1.00f);
	colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.35f, 0.25f, 1.00f);
	colors[ImGuiCol_PlotHistogram] = ImVec4(0.92f, 0.72f, 0.00f, 1.00f);
	colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.62f, 0.00f, 1.00f);

	// 文本选中背景色
	colors[ImGuiCol_TextSelectedBg] = ImVec4(0.18f, 0.65f, 0.98f, 0.35f);

	// 拖放目标色
	colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);

	// 导航高亮色
	colors[ImGuiCol_NavHighlight] = ImVec4(0.18f, 0.65f, 0.98f, 1.00f);
	colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
	colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
	colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);

	// 菜单和弹出窗口的颜色渐变效果
	ImVec4 menuGradientStart = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	ImVec4 menuGradientEnd = ImVec4(0.98f, 0.98f, 0.98f, 1.00f);

	// 增强阴影效果
	ImVec4 shadowColor = ImVec4(0.01f, 0.01f, 0.01f, 0.08f);
	for (int i = 0; i < ImGuiCol_COUNT; i++) {
		if (i == ImGuiCol_WindowBg || i == ImGuiCol_ChildBg || i == ImGuiCol_PopupBg) {
			colors[i].x -= shadowColor.x;
			colors[i].y -= shadowColor.y;
			colors[i].z -= shadowColor.z;
			colors[i].w += shadowColor.w;
		}
	}
	ImGui::StyleColorsDark();
}

void GearX::HandleInteractionFeedback() {
	static float buttonScale = 1.0f;
	static float frameScale = 1.0f;
	static float menuItemScale = 1.0f;

	ImGuiStyle& style = ImGui::GetStyle();
	ImGuiIO& io = ImGui::GetIO();

	// 按钮缩放反馈
	if (ImGui::IsItemHovered()) {
		if (ImGui::IsItemActive()) {
			buttonScale = AnimateScale(0.95f, buttonScale);
		}
		else {
			buttonScale = AnimateScale(1.05f, buttonScale);
		}
	}
	else {
		buttonScale = AnimateScale(1.0f, buttonScale);
	}

	// 输入框等框架元素缩放反馈
	if (ImGui::IsItemHovered()) {
		frameScale = AnimateScale(1.03f, frameScale);
	}
	else {
		frameScale = AnimateScale(1.0f, frameScale);
	}

	// 菜单项缩放反馈
	if (ImGui::IsItemHovered()) {
		menuItemScale = AnimateScale(1.03f, menuItemScale);
	}
	else {
		menuItemScale = AnimateScale(1.0f, menuItemScale);
	}

	//// 应用缩放效果
	//float originalScale = 1.0f;
	//if (ImGui::IsItemHovered()) {
	//    if (ImGui::IsMouseDown(0) && ImGui::IsItemVisible()) {
	//        // 模拟按钮点击
	//        style.ScaleAllSizes(buttonScale);
	//    }
	//    else if (ImGui::IsWindowAppearing()) {
	//        // 模拟框架元素
	//        style.ScaleAllSizes(frameScale);
	//    }
	//    else if (ImGui::IsPopupOpen(std::to_string(ImGui::GetID("Menu")).c_str())) {
	//        // 模拟菜单项
	//        style.ScaleAllSizes(menuItemScale);
	//    }
	//}
	//else {
	//    style.ScaleAllSizes(originalScale);
	//}
}