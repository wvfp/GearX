#pragma once
#include <imgui/imgui.h>

namespace GearX {
    // ��������������ʵ��Ԫ�ص����Ŷ���
    float AnimateScale(float targetScale, float currentScale, float speed = 0.1f);
    // ��ɫ��� 
    void SetImGuiWhiteStyle();
    // ����ѭ���е��ô˺���������������
    void HandleInteractionFeedback();
    
}