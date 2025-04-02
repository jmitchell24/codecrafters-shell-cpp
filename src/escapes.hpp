//
// Created by james on 02/04/25.
//

/**
 * @file escapes.h
 * @brief Common ANSI/VT100 terminal escape sequences for text formatting and cursor control
 *
 * This header provides preprocessor macros for commonly used terminal escape sequences,
 * enabling colored text, cursor manipulation, and screen control in terminal applications.
 *
 * Note: These escape sequences are compatible with most modern terminals that support ANSI codes,
 * including Linux terminals, macOS Terminal, Windows Terminal, and many others.
 * Windows command prompt may require enabling ANSI support via SetConsoleMode().
 */

#pragma once

// ESC character and common sequence beginnings
#define ESC                     "\033"  // ASCII escape character
#define CSI                     ESC "[" // Control Sequence Introducer

// ==================== Text Formatting ====================

// Text style/attributes
#define TERM_RESET             CSI "0m"_sv    // Reset all text attributes to default
#define TERM_BOLD              CSI "1m"_sv    // Bold or increased intensity
#define TERM_DIM               CSI "2m"_sv    // Dimmed/decreased intensity
#define TERM_ITALIC            CSI "3m"_sv    // Italic text (not widely supported)
#define TERM_UNDERLINE         CSI "4m"_sv    // Underlined text
#define TERM_BLINK             CSI "5m"_sv    // Blinking text (not widely supported)
#define TERM_REVERSE           CSI "7m"_sv    // Reverse video (swap foreground/background)
#define TERM_HIDDEN            CSI "8m"_sv    // Hidden text (invisible)
#define TERM_STRIKETHROUGH     CSI "9m"_sv    // Strikethrough text (not widely supported)

// Disable text styles
#define TERM_BOLD_OFF          CSI "22m"_sv   // Normal intensity (not bold or dim)
#define TERM_ITALIC_OFF        CSI "23m"_sv   // Not italic
#define TERM_UNDERLINE_OFF     CSI "24m"_sv   // Not underlined
#define TERM_BLINK_OFF         CSI "25m"_sv   // Not blinking
#define TERM_REVERSE_OFF       CSI "27m"_sv   // Not reversed
#define TERM_HIDDEN_OFF        CSI "28m"_sv   // Not hidden
#define TERM_STRIKETHROUGH_OFF CSI "29m"_sv   // Not strikethrough

// Foreground (text) colors - standard
#define TERM_FG_BLACK          CSI "30m"_sv   // Black foreground
#define TERM_FG_RED            CSI "31m"_sv   // Red foreground
#define TERM_FG_GREEN          CSI "32m"_sv   // Green foreground
#define TERM_FG_YELLOW         CSI "33m"_sv   // Yellow foreground
#define TERM_FG_BLUE           CSI "34m"_sv   // Blue foreground
#define TERM_FG_MAGENTA        CSI "35m"_sv   // Magenta foreground
#define TERM_FG_CYAN           CSI "36m"_sv   // Cyan foreground
#define TERM_FG_WHITE          CSI "37m"_sv   // White foreground
#define TERM_FG_DEFAULT        CSI "39m"_sv   // Default foreground color

// Background colors - standard
#define TERM_BG_BLACK          CSI "40m"_sv   // Black background
#define TERM_BG_RED            CSI "41m"_sv   // Red background
#define TERM_BG_GREEN          CSI "42m"_sv   // Green background
#define TERM_BG_YELLOW         CSI "43m"_sv   // Yellow background
#define TERM_BG_BLUE           CSI "44m"_sv   // Blue background
#define TERM_BG_MAGENTA        CSI "45m"_sv   // Magenta background
#define TERM_BG_CYAN           CSI "46m"_sv   // Cyan background
#define TERM_BG_WHITE          CSI "47m"_sv   // White background
#define TERM_BG_DEFAULT        CSI "49m"_sv   // Default background color

// Foreground (text) colors - bright variants
#define TERM_FG_BRIGHT_BLACK   CSI "90m"_sv   // Bright black foreground (gray)
#define TERM_FG_BRIGHT_RED     CSI "91m"_sv   // Bright red foreground
#define TERM_FG_BRIGHT_GREEN   CSI "92m"_sv   // Bright green foreground
#define TERM_FG_BRIGHT_YELLOW  CSI "93m"_sv   // Bright yellow foreground
#define TERM_FG_BRIGHT_BLUE    CSI "94m"_sv   // Bright blue foreground
#define TERM_FG_BRIGHT_MAGENTA CSI "95m"_sv   // Bright magenta foreground
#define TERM_FG_BRIGHT_CYAN    CSI "96m"_sv   // Bright cyan foreground
#define TERM_FG_BRIGHT_WHITE   CSI "97m"_sv   // Bright white foreground

// Background colors - bright variants
#define TERM_BG_BRIGHT_BLACK   CSI "100m"_sv  // Bright black background (gray)
#define TERM_BG_BRIGHT_RED     CSI "101m"_sv  // Bright red background
#define TERM_BG_BRIGHT_GREEN   CSI "102m"_sv  // Bright green background
#define TERM_BG_BRIGHT_YELLOW  CSI "103m"_sv  // Bright yellow background
#define TERM_BG_BRIGHT_BLUE    CSI "104m"_sv  // Bright blue background
#define TERM_BG_BRIGHT_MAGENTA CSI "105m"_sv  // Bright magenta background
#define TERM_BG_BRIGHT_CYAN    CSI "106m"_sv  // Bright cyan background
#define TERM_BG_BRIGHT_WHITE   CSI "107m"_sv  // Bright white background

// 256-color mode - templated values shown, replace n with color number (0-255)
// #define TERM_FG_256(n)        CSI "38;5;nm"_sv  // 256-color foreground
// #define TERM_BG_256(n)        CSI "48;5;nm"_sv  // 256-color background

// RGB color - templated values shown, replace r,g,b with values 0-255
// #define TERM_FG_RGB(r,g,b)    CSI "38;2;r;g;bm"_sv  // RGB foreground
// #define TERM_BG_RGB(r,g,b)    CSI "48;2;r;g;bm"_sv  // RGB background

// ==================== Cursor Control ====================

// Move cursor by a specified number of positions
#define TERM_CURSOR_UP(n)        CSI #n "A"_sv  // Move cursor up n lines
#define TERM_CURSOR_DOWN(n)      CSI #n "B"_sv  // Move cursor down n lines
#define TERM_CURSOR_RIGHT(n)     CSI #n "C"_sv  // Move cursor right n columns
#define TERM_CURSOR_LEFT(n)      CSI #n "D"_sv  // Move cursor left n columns
#define TERM_CURSOR_NEXT_LINE(n) CSI #n "E"_sv  // Move cursor to beginning of line n lines down
#define TERM_CURSOR_PREV_LINE(n) CSI #n "F"_sv  // Move cursor to beginning of line n lines up
#define TERM_CURSOR_COLUMN(n)    CSI #n "G"_sv  // Move cursor to column n

// Set cursor position (1-based coordinates)
#define TERM_CURSOR_POS(_row,_col) CSI #_row ";" #_col "H"_sv  // Move cursor to row, col
#define TERM_CURSOR_HOME         CSI "H"_sv     // Move cursor to home position (1,1)

// Cursor visibility
#define TERM_CURSOR_SAVE       ESC "7"_sv     // Save cursor position
#define TERM_CURSOR_RESTORE    ESC "8"_sv     // Restore cursor position
#define TERM_CURSOR_HIDE       CSI "?25l"_sv  // Hide cursor
#define TERM_CURSOR_SHOW       CSI "?25h"_sv  // Show cursor

// ==================== Terminal Bell ====================
#define TERM_BELL              "\a"_sv       // Audible bell
#define TERM_VISUAL_BELL       CSI "?5h"_sv  // Enable visual bell mode (flash screen)
#define TERM_VISUAL_BELL_OFF   CSI "?5l"_sv  // Disable visual bell mode

// ==================== Screen Control ====================

// Clear screen parts
#define TERM_CLEAR_SCREEN      CSI "2J"_sv    // Clear entire screen
#define TERM_CLEAR_SCREEN_TO_END CSI "0J"_sv  // Clear from cursor to end of screen
#define TERM_CLEAR_SCREEN_TO_BEGIN CSI "1J"_sv // Clear from cursor to beginning of screen

// Clear line parts
#define TERM_CLEAR_LINE        CSI "2K"_sv    // Clear entire line
#define TERM_CLEAR_LINE_TO_END CSI "0K"_sv    // Clear from cursor to end of line
#define TERM_CLEAR_LINE_TO_BEGIN CSI "1K"_sv  // Clear from cursor to beginning of line

// Scrolling
#define TERM_SCROLL_UP(_n)      CSI #_n "S"_sv  // Scroll up n lines
#define TERM_SCROLL_DOWN(_n)    CSI #_n "T"_sv  // Scroll down n lines

// ==================== Terminal Control ====================

// Terminal window title (for xterm compatible terminals)
#define TERM_SET_TITLE(_title)  ESC "]0;" _title ESC "\\"_sv  // Set terminal window title

// Terminal modes
#define TERM_ALTERNATE_SCREEN_ON  CSI "?1049h"_sv  // Enable alternate screen buffer
#define TERM_ALTERNATE_SCREEN_OFF CSI "?1049l"_sv  // Disable alternate screen buffer

// ==================== Helper Macros ====================

// Convenience macros for common color combinations
#define TERM_ERROR             TERM_FG_RED       // Error messages
#define TERM_WARNING           TERM_FG_YELLOW    // Warning messages
#define TERM_SUCCESS           TERM_FG_GREEN     // Success messages
#define TERM_INFO              TERM_FG_CYAN      // Informational messages
#define TERM_HIGHLIGHT         TERM_FG_MAGENTA   // Highlighted text


namespace sh
{

}