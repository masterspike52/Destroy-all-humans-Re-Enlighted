#include "button_guide.h"
#include "imgui_utils.h"
#include <gpu/imgui/imgui_snapshot.h>
#include <gpu/video.h>
#include <hid/hid.h>
#include <patches/aspect_ratio_patches.h>
#include <user/config.h>
#include <app.h>
#include <decompressor.h>

#include <res/images/common/controller.dds.h>
#include <res/images/common/kbm.dds.h>

constexpr float DEFAULT_SIDE_MARGINS = 379;

ImFont* g_rodinFont;

std::unique_ptr<GuestTexture> g_upControllerIcons;
std::unique_ptr<GuestTexture> g_upKBMIcons;

float g_sideMargins = DEFAULT_SIDE_MARGINS;

static std::vector<Button> g_buttons;

std::unordered_map<EButtonIcon, float> g_iconWidths =
{
    { EButtonIcon::A, 40 },
    { EButtonIcon::B, 40 },
    { EButtonIcon::X, 40 },
    { EButtonIcon::Y, 40 },
    { EButtonIcon::LB, 70 },
    { EButtonIcon::RB, 70 },
    { EButtonIcon::LBRB, 70 },
    { EButtonIcon::LT, 42 },
    { EButtonIcon::RT, 42 },
    { EButtonIcon::LTRT, 42 },
    { EButtonIcon::Start, 40 },
    { EButtonIcon::Back, 40 },
    { EButtonIcon::LMB, 40 },
    { EButtonIcon::Enter, 40 },
    { EButtonIcon::Escape, 40 },
};

std::unordered_map<EButtonIcon, float> g_iconHeights =
{
    { EButtonIcon::A, 40 },
    { EButtonIcon::B, 40 },
    { EButtonIcon::X, 40 },
    { EButtonIcon::Y, 40 },
    { EButtonIcon::LB, 40 },
    { EButtonIcon::RB, 40 },
    { EButtonIcon::LBRB, 40 },
    { EButtonIcon::LT, 42 },
    { EButtonIcon::RT, 42 },
    { EButtonIcon::LTRT, 42 },
    { EButtonIcon::Start, 40 },
    { EButtonIcon::Back, 40 },
    { EButtonIcon::LMB, 40 },
    { EButtonIcon::Enter, 40 },
    { EButtonIcon::Escape, 40 },
};

std::tuple<std::tuple<ImVec2, ImVec2>, GuestTexture*> GetButtonIcon(EButtonIcon icon)
{
    std::tuple<ImVec2, ImVec2> btn;
    GuestTexture* texture;

    auto isPlayStation = Config::ControllerIcons == EControllerIcons::Auto
        ? hid::g_inputDeviceController == hid::EInputDevice::PlayStation
        : Config::ControllerIcons == EControllerIcons::PlayStation;

    auto yOffsetCmn = isPlayStation ? 42 : 0;
    auto yOffsetStartBack = isPlayStation ? 46 : 0;

    switch (icon)
    {
        case EButtonIcon::A:
            btn = PIXELS_TO_UV_COORDS(512, 128, 0, yOffsetCmn, 40, 40);
            texture = g_upControllerIcons.get();
            break;

        case EButtonIcon::B:
            btn = PIXELS_TO_UV_COORDS(512, 128, 40, yOffsetCmn, 40, 40);
            texture = g_upControllerIcons.get();
            break;

        case EButtonIcon::X:
            btn = PIXELS_TO_UV_COORDS(512, 128, 80, yOffsetCmn, 40, 40);
            texture = g_upControllerIcons.get();
            break;

        case EButtonIcon::Y:
            btn = PIXELS_TO_UV_COORDS(512, 128, 120, yOffsetCmn, 40, 40);
            texture = g_upControllerIcons.get();
            break;

        case EButtonIcon::LB:
            btn = PIXELS_TO_UV_COORDS(512, 128, 166, yOffsetCmn, 70, 40);
            texture = g_upControllerIcons.get();
            break;

        case EButtonIcon::RB:
            btn = PIXELS_TO_UV_COORDS(512, 128, 246, yOffsetCmn, 70, 40);
            texture = g_upControllerIcons.get();
            break;

        case EButtonIcon::LT:
            btn = PIXELS_TO_UV_COORDS(512, 128, 320, yOffsetCmn, 40, 42);
            texture = g_upControllerIcons.get();
            break;

        case EButtonIcon::RT:
            btn = PIXELS_TO_UV_COORDS(512, 128, 360, yOffsetCmn, 40, 42);
            texture = g_upControllerIcons.get();
            break;

        case EButtonIcon::Start:
            btn = PIXELS_TO_UV_COORDS(512, 128, 455, yOffsetStartBack, 46, 46);
            texture = g_upControllerIcons.get();
            break;

        case EButtonIcon::Back:
            btn = PIXELS_TO_UV_COORDS(512, 128, 408, yOffsetStartBack, 46, 46);
            texture = g_upControllerIcons.get();
            break;

        case EButtonIcon::LMB:
            btn = PIXELS_TO_UV_COORDS(384, 128, 0, 0, 128, 128);
            texture = g_upKBMIcons.get();
            break;

        case EButtonIcon::Enter:
            btn = PIXELS_TO_UV_COORDS(384, 128, 128, 0, 128, 128);
            texture = g_upKBMIcons.get();
            break;

        case EButtonIcon::Escape:
            btn = PIXELS_TO_UV_COORDS(384, 128, 256, 0, 128, 128);
            texture = g_upKBMIcons.get();
            break;
    }

    return std::make_tuple(btn, texture);
}

static void DrawGuide(float* offset, ImVec2 regionMin, ImVec2 regionMax, EButtonIcon icon,
    EButtonAlignment alignment, ImVec2 iconMin, ImVec2 iconMax, EFontQuality fontQuality,
    float textWidth, float maxTextWidth, float textScale, float fontSize, const char* text)
{
    auto drawList = ImGui::GetBackgroundDrawList();
    auto iconWidth = Scale(g_iconWidths[icon]);
    auto textMarginY = regionMin.y + Scale(9.0f);

    ImVec2 textPos;

    if (icon == EButtonIcon::LBRB || icon == EButtonIcon::LTRT)
    {
        auto iconMarginX = Scale(16);

        iconMin.x = alignment == EButtonAlignment::Left
            ? regionMin.x + *offset - maxTextWidth + (iconWidth / 2)
            : regionMax.x - *offset - maxTextWidth - (iconWidth / 2) - iconMarginX;

        iconMax.x = iconMin.x + iconWidth;

        // Left button.
        auto btnIcon = GetButtonIcon(icon == EButtonIcon::LBRB ? EButtonIcon::LB : EButtonIcon::LT);
        drawList->AddImage(std::get<1>(btnIcon), iconMin, iconMax, GET_UV_COORDS(std::get<0>(btnIcon)));

        auto dualIconMin = alignment == EButtonAlignment::Left
            ? ImVec2(iconMax.x + maxTextWidth + iconMarginX, iconMin.y)
            : ImVec2(regionMax.x - *offset + maxTextWidth - iconWidth, iconMin.y);

        auto dualIconMax = ImVec2(dualIconMin.x + iconWidth, iconMax.y);

        // Right button.
        btnIcon = GetButtonIcon(icon == EButtonIcon::LBRB ? EButtonIcon::RB : EButtonIcon::RT);
        drawList->AddImage(std::get<1>(btnIcon), dualIconMin, dualIconMax, GET_UV_COORDS(std::get<0>(btnIcon)));

        textPos = { (iconMax.x + ((dualIconMin.x - iconMax.x) - maxTextWidth + std::max(0.0f, maxTextWidth - textWidth)) / 2) + Scale(2), textMarginY};

        *offset += iconWidth;
    }
    else
    {
        auto btnIcon = GetButtonIcon(icon);

        drawList->AddImage(std::get<1>(btnIcon), iconMin, iconMax, GET_UV_COORDS(std::get<0>(btnIcon)));

        auto textMarginX = alignment == EButtonAlignment::Left
            ? regionMin.x + *offset
            : regionMax.x - *offset;

        textPos = { textMarginX, textMarginY };
    }

    SetScale({ textScale, 1.0f });
    SetOrigin(textPos);

    if (fontQuality == EFontQuality::Low)
        SetShaderModifier(IMGUI_SHADER_MODIFIER_LOW_QUALITY_TEXT);

    DrawTextBasic(g_rodinFont, fontSize, textPos, IM_COL32_WHITE, text);

    if (fontQuality == EFontQuality::Low)
    {
        // Add extra luminance to low quality text.
        drawList->AddText(g_rodinFont, fontSize, textPos, IM_COL32(255, 255, 255, 127), text);
        SetShaderModifier(IMGUI_SHADER_MODIFIER_NONE);
    }

    SetScale({ 1.0f, 1.0f });
    SetOrigin({ 0.0f, 0.0f });
}

void ButtonGuide::Init()
{
    auto& io = ImGui::GetIO();

    g_rodinFont = ImFontAtlasSnapshot::GetFont("FOT-RodinPro-DB.otf");
    g_upControllerIcons = LOAD_ZSTD_TEXTURE(g_controller);
    g_upKBMIcons = LOAD_ZSTD_TEXTURE(g_kbm);
}

void ButtonGuide::Draw()
{
    if (!s_isVisible)
        return;

    auto drawList = ImGui::GetBackgroundDrawList();
    auto& res = ImGui::GetIO().DisplaySize;

    ImVec2 regionMin = { g_aspectRatioOffsetX + Scale(g_sideMargins), g_aspectRatioOffsetY * 2.0f + Scale(720.0f - 102.0f) };
    ImVec2 regionMax = { g_aspectRatioOffsetX + Scale(1280.0f - g_sideMargins), g_aspectRatioOffsetY * 2.0f + Scale(720.0f) };

    auto textMarginX = Scale(21.25f);
    auto iconMarginX = Scale(4);
    auto fontSize = Scale(21.8f);

    auto offsetLeft = 0.0f;
    auto offsetRight = 0.0f;

    // Draw left aligned icons.
    for (int i = 0; i < g_buttons.size(); i++)
    {
        auto& btn = g_buttons[i];

        if (btn.Alignment != EButtonAlignment::Left)
            continue;

        if (btn.Visibility && !*btn.Visibility)
            continue;

        auto str = Localise(btn.Name).c_str();
        auto iconWidth = Scale(g_iconWidths[btn.Icon]);
        auto iconHeight = Scale(g_iconHeights[btn.Icon]);
        auto textWidth = g_rodinFont->CalcTextSizeA(fontSize, FLT_MAX, 0, str).x;
        auto maxWidth = btn.MaxWidth == FLT_MAX ? textWidth : Scale(btn.MaxWidth);
        auto textScale = std::min(1.0f, maxWidth / textWidth);

        if (i > 0)
            offsetLeft += maxWidth + iconWidth + textMarginX;

        ImVec2 iconMin = { regionMin.x + offsetLeft - iconWidth - iconMarginX, regionMin.y };
        ImVec2 iconMax = { regionMin.x + offsetLeft - iconMarginX, regionMin.y + iconHeight };

        DrawGuide(&offsetLeft, regionMin, regionMax, btn.Icon, btn.Alignment, iconMin, iconMax, btn.FontQuality, textWidth, maxWidth, textScale, fontSize, str);
    }

    // Draw right aligned icons.
    for (int i = g_buttons.size() - 1; i >= 0; i--)
    {
        auto& btn = g_buttons[i];

        if (btn.Alignment != EButtonAlignment::Right)
            continue;

        if (btn.Visibility && !*btn.Visibility)
            continue;

        auto str = Localise(btn.Name).c_str();
        auto iconWidth = Scale(g_iconWidths[btn.Icon]);
        auto iconHeight = Scale(g_iconHeights[btn.Icon]);
        auto textWidth = g_rodinFont->CalcTextSizeA(fontSize, FLT_MAX, 0, str).x;
        auto maxWidth = btn.MaxWidth == FLT_MAX ? textWidth : Scale(btn.MaxWidth);
        auto textScale = std::min(1.0f, maxWidth / textWidth);

        if (i < g_buttons.size() - 1)
            offsetRight += maxWidth + iconWidth + textMarginX;

        ImVec2 iconMin = { regionMax.x - offsetRight - iconWidth - iconMarginX, regionMin.y };
        ImVec2 iconMax = { regionMax.x - offsetRight - iconMarginX, regionMin.y + iconHeight };

        DrawGuide(&offsetRight, regionMin, regionMax, btn.Icon, btn.Alignment, iconMin, iconMax, btn.FontQuality, textWidth, maxWidth, textScale, fontSize, str);
    }
}

void ButtonGuide::Open(Button button)
{
    s_isVisible = true;
    g_sideMargins = DEFAULT_SIDE_MARGINS;

    g_buttons = {};
    g_buttons.push_back(button);
}

void ButtonGuide::Open(const std::span<Button> buttons)
{
    s_isVisible = true;
    g_sideMargins = DEFAULT_SIDE_MARGINS;
    g_buttons = std::vector(buttons.begin(), buttons.end());
}

void ButtonGuide::SetSideMargins(float width = DEFAULT_SIDE_MARGINS)
{
    g_sideMargins = width;
}

void ButtonGuide::Close()
{
    s_isVisible = false;
}
