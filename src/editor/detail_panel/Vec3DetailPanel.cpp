#include "Vec3DetailPanel.h"

#include <imgui.h>
#include <imgui_internal.h>

namespace Vox
{
    ImColor Vec3DetailPanel::xAxisColor = ImColor(1.0f, 0.0f, 0.0f);
    ImColor Vec3DetailPanel::yAxisColor = ImColor(0.0f, 1.0f, 0.0f);
    ImColor Vec3DetailPanel::zAxisColor = ImColor(0.0f, 0.0f, 1.0f);
    
    bool Vec3DetailPanel::Draw(const char* label, glm::vec3* vector, const char* format, ImGuiInputTextFlags flags)
    {
        using namespace ImGui;

        ImGuiWindow* window = GetCurrentWindow();
        if (window->SkipItems)
            return false;

        ImGuiContext& g = *GImGui;
        bool value_changed = false;
        BeginGroup();
        PushID(label);
        PushMultiItemsWidths(3, CalcItemWidth());

        for (int i = 0; i < 3; i++)
        {
            void* pData;
            ImColor axisColor;
            
            switch (i)
            {
            default:
            case 0:
                pData = &vector->x;
                axisColor = xAxisColor;
                break;
            case 1:
                pData = &vector->y;
                axisColor = yAxisColor;
                break;
            case 2:
                pData = &vector->z;
                axisColor = zAxisColor;
                break;
            }
            PushID(i);
            if (i > 0)
                SameLine(0, g.Style.ItemInnerSpacing.x);
            value_changed |= InputAxis(pData, axisColor, format);
            PopID();
            PopItemWidth();
        }
        PopID();

        const char* label_end = FindRenderedTextEnd(label);
        if (label != label_end)
        {
            SameLine(0.0f, g.Style.ItemInnerSpacing.x);
            TextEx(label, label_end);
        }

        EndGroup();
        return value_changed;
    }

    bool Vec3DetailPanel::InputAxis(void* pData, ImColor axisColor, const char* format)
    {
        using namespace ImGui;
        ImGuiWindow* window = GetCurrentWindow();
        if (window->SkipItems)
            return false;

        ImGuiInputTextFlags flags = ImGuiInputTextFlags_None;
        ImGuiContext& g = *GImGui;
        ImGuiStyle& style = g.Style;

        void* p_data_default = (g.NextItemData.HasFlags & ImGuiNextItemDataFlags_HasRefVal) ? &g.NextItemData.RefVal : &g.DataTypeZeroValue;

        char buf[64];
        if (flags & ImGuiInputTextFlags_DisplayEmptyRefVal && DataTypeCompare(ImGuiDataType_Float, pData, p_data_default) == 0)
        {
            buf[0] = 0;
        }
        else
        {
            DataTypeFormatString(buf, IM_ARRAYSIZE(buf), ImGuiDataType_Float, pData, format);
        }

        const auto label = "";
        
        g.NextItemData.ItemFlags |= ImGuiItemFlags_NoMarkEdited;
        flags |= ImGuiInputTextFlags_AutoSelectAll | static_cast<ImGuiInputTextFlags>(ImGuiInputTextFlags_LocalizeDecimalPoint) | ImGuiInputTextFlags_ElideLeft;

        bool valueChanged = false;

        const ImVec2 cursor = window->DC.CursorPos;
        const ImVec2 pos =  {cursor.x, cursor.y + 1.0f};
        const ImVec2 size = CalcItemSize(ImVec2(0.0f, 0.0f), 5.0f, g.FontSize + style.FramePadding.y * 2.0f);
        const ImRect bb(pos, ImVec2(pos.x + size.x - 2.0f, pos.y + size.y - 2.0f));
        RenderRectFilledRangeH(window->DrawList, bb, axisColor, 0.0f, 1.0f, 10.0f);
        
        BeginGroup();
        PushID("");
        SetNextItemWidth(ImMax(1.0f, CalcItemWidth()));
        if (InputText(label, buf, IM_ARRAYSIZE(buf), flags))
        {
            valueChanged = DataTypeApplyFromText(buf, ImGuiDataType_Float, pData, "%.3f",
                (flags & ImGuiInputTextFlags_ParseEmptyRefVal) ? p_data_default : nullptr);
        }
        IMGUI_TEST_ENGINE_ITEM_INFO(g.LastItemData.ID, label, g.LastItemData.StatusFlags | ImGuiItemStatusFlags_Inputable);
        
        if (const char* labelEnd = FindRenderedTextEnd(label); label != labelEnd)
        {
            SameLine(0, style.ItemInnerSpacing.x);
            TextEx(label, labelEnd);
        }
        PopID();
        EndGroup();
        g.LastItemData.ItemFlags &= ~ImGuiItemFlags_NoMarkEdited;
        if (valueChanged)
        {
            MarkItemEdited(g.LastItemData.ID);
        }

        return valueChanged;
    }
}
