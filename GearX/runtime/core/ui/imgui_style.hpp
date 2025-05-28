#pragma once
#include <imgui/imgui.h>

namespace GearX {
    // 辅助函数：用于实现元素的缩放动画
    float AnimateScale(float targetScale, float currentScale, float speed = 0.1f);
    // 白色风格 
    void SetImGuiWhiteStyle();
    // 在主循环中调用此函数来处理交互反馈
    void HandleInteractionFeedback();
    
}