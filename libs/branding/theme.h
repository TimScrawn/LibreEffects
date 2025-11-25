#pragma once

#include <string>
#include <array>

namespace LibreEffects::Branding {

    // LibreEffects Brand Colors - Modern Design System
    struct BrandColors {
        // Primary brand color - Modern deep blue/purple gradient
        static constexpr std::array<int, 3> PRIMARY = {67, 97, 238};      // #4361EE - Modern vibrant blue
        static constexpr std::array<int, 3> PRIMARY_DARK = {52, 75, 185}; // #344BB9 - Darker variant
        static constexpr std::array<int, 3> PRIMARY_LIGHT = {120, 140, 255}; // #788CFF - Lighter variant
        
        // Secondary brand color - Vibrant green accent
        static constexpr std::array<int, 3> SECONDARY = {0, 230, 118};   // #00E676 - Modern green
        static constexpr std::array<int, 3> SECONDARY_DARK = {0, 200, 100}; // #00C864
        
        // Accent colors - Modern palette
        static constexpr std::array<int, 3> ACCENT_BLUE = {0, 191, 255};  // #00BFFF - Sky blue
        static constexpr std::array<int, 3> ACCENT_PURPLE = {147, 51, 234}; // #9333EA - Modern purple
        static constexpr std::array<int, 3> ACCENT_ORANGE = {255, 152, 0}; // #FF9800 - Vibrant orange
        
        // UI Colors - Modern dark theme
        static constexpr std::array<int, 3> BACKGROUND_DARK = {18, 18, 23};   // Almost black - modern dark
        static constexpr std::array<int, 3> BACKGROUND_DARK_ALT = {24, 24, 30}; // Slightly lighter
        static constexpr std::array<int, 3> BACKGROUND_LIGHT = {250, 250, 255}; // Off-white
        static constexpr std::array<int, 3> PANEL_DARK = {28, 28, 35}; // Modern panel color
        static constexpr std::array<int, 3> PANEL_DARK_HOVER = {35, 35, 42}; // Hover state
        static constexpr std::array<int, 3> PANEL_LIGHT = {255, 255, 255};
        static constexpr std::array<int, 3> TEXT_PRIMARY = {255, 255, 255};
        static constexpr std::array<int, 3> TEXT_SECONDARY = {170, 170, 180}; // Softer secondary text
        static constexpr std::array<int, 3> TEXT_TERTIARY = {120, 120, 130}; // Muted text
        static constexpr std::array<int, 3> BORDER = {45, 45, 55}; // Subtle borders
        static constexpr std::array<int, 3> BORDER_HOVER = {70, 70, 85}; // Hover borders
        
        // Interactive colors
        static constexpr std::array<int, 3> BUTTON_HOVER = {40, 40, 50};
        static constexpr std::array<int, 3> BUTTON_ACTIVE = {52, 75, 185};
        static constexpr std::array<int, 3> SELECTION = {67, 97, 238}; // Primary color for selection
        static constexpr std::array<int, 3> SELECTION_HOVER = {80, 110, 255};
    };

    // Application Theme Configuration
    class Theme {
    public:
        enum class ThemeMode {
            Dark,
            Light
        };

        Theme(ThemeMode mode = ThemeMode::Dark);
        
        // Get theme colors
        std::array<int, 3> getBackgroundColor() const;
        std::array<int, 3> getPanelColor() const;
        std::array<int, 3> getTextColor() const;
        std::array<int, 3> getBorderColor() const;
        std::array<int, 3> getPrimaryColor() const;
        std::array<int, 3> getSecondaryColor() const;
        
        // Theme mode
        ThemeMode getMode() const { return m_mode; }
        void setMode(ThemeMode mode) { m_mode = mode; }
        
        // Font configuration
        std::string getFontFamily() const { return "Inter, Segoe UI, system-ui, sans-serif"; }
        int getFontSize() const { return 13; }
        int getFontSizeSmall() const { return 11; }
        int getFontSizeLarge() const { return 16; }
        
        // Spacing
        int getSpacing() const { return 8; }
        int getSpacingSmall() const { return 4; }
        int getSpacingLarge() const { return 16; }
        
        // Border radius - Modern rounded corners
        int getBorderRadius() const { return 8; }
        int getBorderRadiusSmall() const { return 6; }
        int getBorderRadiusLarge() const { return 12; }
        int getBorderRadiusXLarge() const { return 16; }
        
        // Get modern color variants
        std::array<int, 3> getPrimaryDark() const { return BrandColors::PRIMARY_DARK; }
        std::array<int, 3> getPrimaryLight() const { return BrandColors::PRIMARY_LIGHT; }
        std::array<int, 3> getSecondaryDark() const { return BrandColors::SECONDARY_DARK; }
        std::array<int, 3> getButtonHoverColor() const { return BrandColors::BUTTON_HOVER; }
        std::array<int, 3> getButtonActiveColor() const { return BrandColors::BUTTON_ACTIVE; }
        std::array<int, 3> getSelectionColor() const { return BrandColors::SELECTION; }
        std::array<int, 3> getSelectionHoverColor() const { return BrandColors::SELECTION_HOVER; }
        std::array<int, 3> getPanelHoverColor() const { return BrandColors::PANEL_DARK_HOVER; }
        std::array<int, 3> getTextTertiaryColor() const { return BrandColors::TEXT_TERTIARY; }
        std::array<int, 3> getBorderHoverColor() const { return BrandColors::BORDER_HOVER; }

    private:
        ThemeMode m_mode;
    };

    // Brand Information
    struct BrandInfo {
        static constexpr const char* NAME = "LibreEffects";
        static constexpr const char* TAGLINE = "Professional Creative Tools for Everyone";
        static constexpr const char* VERSION = "0.1.0";
        static constexpr const char* WEBSITE = "https://libreeffects.org";
    };

} // namespace LibreEffects::Branding

