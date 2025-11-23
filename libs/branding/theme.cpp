#include "theme.h"

namespace LibreEffects::Branding {

    Theme::Theme(ThemeMode mode) : m_mode(mode) {}

    std::array<int, 3> Theme::getBackgroundColor() const {
        return m_mode == ThemeMode::Dark 
            ? BrandColors::BACKGROUND_DARK 
            : BrandColors::BACKGROUND_LIGHT;
    }

    std::array<int, 3> Theme::getPanelColor() const {
        return m_mode == ThemeMode::Dark 
            ? BrandColors::PANEL_DARK 
            : BrandColors::PANEL_LIGHT;
    }

    std::array<int, 3> Theme::getTextColor() const {
        return m_mode == ThemeMode::Dark 
            ? BrandColors::TEXT_PRIMARY 
            : std::array<int, 3>{30, 30, 35};
    }

    std::array<int, 3> Theme::getBorderColor() const {
        return BrandColors::BORDER;
    }

    std::array<int, 3> Theme::getPrimaryColor() const {
        return BrandColors::PRIMARY;
    }

    std::array<int, 3> Theme::getSecondaryColor() const {
        return BrandColors::SECONDARY;
    }

}

