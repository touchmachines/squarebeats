#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

namespace SquareBeats {

/**
 * Global font configuration for SquareBeats.
 * 
 * The application uses modern system fonts for a clean, professional look
 * that's guaranteed to work on all systems without requiring font installation.
 * 
 * Windows: Segoe UI (modern, clean, excellent readability)
 * macOS: SF Pro / Helvetica Neue (native macOS look)
 * Linux: Ubuntu / Roboto (common Linux fonts)
 * 
 * These fonts are pre-installed on their respective platforms, ensuring
 * consistent appearance without requiring users to install additional fonts.
 */
class AppFont
{
public:
    /**
     * Get the best available modern font for the current platform.
     * Returns a font name that's guaranteed to exist on the system.
     */
    static juce::String getPlatformFont()
    {
        #if JUCE_WINDOWS
            return "Segoe UI";
        #elif JUCE_MAC
            return "SF Pro Text";  // Falls back to Helvetica Neue if not available
        #elif JUCE_LINUX
            return "Ubuntu";  // Falls back to system default
        #else
            return juce::Font::getDefaultSansSerifFontName();
        #endif
    }
    
    // Standard font sizes for different UI contexts
    static constexpr float titleSize = 14.0f;
    static constexpr float labelSize = 13.0f;
    static constexpr float smallLabelSize = 12.0f;
    static constexpr float tinySize = 10.0f;
    static constexpr float microSize = 9.0f;
    
    /**
     * Get the application font at the specified size.
     * @param size Font size in points
     * @param bold Whether to use bold style
     * @return juce::Font configured with platform-appropriate modern font
     */
    static juce::Font getFont(float size, bool bold = false)
    {
        if (bold)
            return juce::Font(juce::FontOptions(getPlatformFont(), size, juce::Font::plain).withStyle("Bold"));
        else
            return juce::Font(juce::FontOptions(getPlatformFont(), size, juce::Font::plain));
    }
    
    // Convenience methods for common font configurations
    static juce::Font title()       { return getFont(titleSize, true); }
    static juce::Font label()       { return getFont(labelSize); }
    static juce::Font smallLabel()  { return getFont(smallLabelSize, true); }
    static juce::Font tiny()        { return getFont(tinySize); }
    static juce::Font micro()       { return getFont(microSize); }
};

} // namespace SquareBeats
