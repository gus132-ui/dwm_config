/* Transfiguration colours for dwm.
 *
 * Include this from your config.h and delete the corresponding definitions
 * there -- fonts[], the col_* strings, and the colors[][3] array:
 *
 *     #include "colors.h"
 *
 * It must be included AFTER dwm.c's `enum { SchemeNorm, SchemeSel, ... }`,
 * which is automatic: dwm.c includes config.h below that enum.
 *
 * SchemeBarEmpty requires a dwm.c patch (see the repo README). Without it the
 * enum has only two entries and the third initialiser below is a compile
 * error, so patch dwm.c first or drop that line.
 *
 * NOTE ON SchemeSel: drw_text() paints the FULL width it is given with the
 * scheme's background before drawing any glyphs. dwm draws the window title
 * across all remaining bar width, so SchemeSel's background colours the entire
 * title area -- not just behind the text. That is deliberate here.
 *
 * ZERO hardcoded hexes: every value comes from palettes/<scheme>.json.
 */

static const char *fonts[]          = { "Terminus:pixelsize=16:antialias=false" };

static const char col_bar_bg[]      = "#262e28";
static const char col_bar_text[]    = "#e6dec6";
static const char col_sel_bg[]      = "#4a5742";
static const char col_sel_text[]    = "#f0e9d2";
static const char col_border[]      = "#6e7d54";
static const char col_border_norm[] = "#262e28";

static const char *colors[][3]      = {
	/*                    fg             bg           border          */
	[SchemeNorm]     = { col_bar_text,  col_bar_bg,  col_border_norm },
	[SchemeSel]      = { col_sel_text,  col_sel_bg,  col_border      },
	[SchemeBarEmpty] = { col_bar_text,  col_sel_bg,  col_sel_bg      },
};
