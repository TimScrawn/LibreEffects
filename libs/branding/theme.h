#pragma once

#include <string>
#include <array>

namespace LibreEffects::Branding {

    // LibreEffects Brand Colors
    struct BrandColors {
        // Primary brand color - Deep blue/purple representing creativity and professionalism
        static constexpr std::array<int, 3> PRIMARY = {45, 62, 120};      // #2D3E78
        
        // Secondary brand color - Vibrant accent
        static constexpr std::array<int, 3> SECONDARY = {118, 185, 0};   // #76B900
        
        // Accent colors
        static constexpr std::array<int, 3> ACCENT_BLUE = {0, 162, 255};  // #00A2FF
        static constexpr std::array<int, 3> ACCENT_PURPLE = {138, 43, 226}; // #8A2BE2
        
        // UI Colors
        static constexpr std::array<int, 3> BACKGROUND_DARK = {30, 30, 35};   // Dark theme background
        static constexpr std::array<int, 3> BACKGROUND_LIGHT = {245, 245, 250}; // Light theme background
        static constexpr std::array<int, 3> PANEL_DARK = {40, 40, 45};
        static constexpr std::array<int, 3> PANEL_LIGHT = {255, 255, 255};
        static constexpr std::array<int, 3> TEXT_PRIMARY = {255, 255, 255};
        static constexpr std::array<int, 3> TEXT_SECONDARY = {180, 180, 185};
        static constexpr std::array<int, 3> BORDER = {60, 60, 65};
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
        
        // Border radius
        int getBorderRadius() const { return 6; }
        int getBorderRadiusSmall() const { return 4; }
        int getBorderRadiusLarge() const { return 8; }

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

