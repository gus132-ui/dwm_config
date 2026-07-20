/* See LICENSE file for copyright and license details. */
#include <X11/XF86keysym.h>	/* XF86XK_* media/brightness keysyms */

/* appearance */
static const unsigned int borderpx  = 2;
static const unsigned int snap      = 32;
static const int swallowfloating    = 0;        /* 1 means swallow floating windows by default */
static const int showbar            = 1;
static const int topbar             = 1;
static const char *fonts[]          = { "Terminus:pixelsize=16:antialias=false" };
/* Transfiguration palette -- slots match ~/git/st/config.def.h colorname[].
 * dmenu's own colors/font are NOT here; they live in ~/.local/bin/dmenu. */
static const char col_bg[]          = "#262e28"; /* st 257: default bg (dark hills) */
static const char col_fg[]          = "#e6dec6"; /* st 256: bar text, 10.4:1 on bg  */
static const char col_sel_fg[]      = "#f0e9d8"; /* st 15:  title + selected tag    */
static const char col_accent[]      = "#6e7d54"; /* st 2:   focused window border   */
static const char col_olive[]       = "#4a5742"; /* muted olive: title area + chip  */
static const char *colors[][3]      = {
        /*               fg           bg      border   */
        [SchemeNorm] = { col_fg,      col_bg,    col_bg     },
        /* SchemeSel's bg is what drw_text paints across the WHOLE title width,
         * so the olive here is both the selected-tag chip and the title area. */
        [SchemeSel]  = { col_sel_fg,  col_olive, col_accent },
        /* used by drawbar when a tag has no client at all */
        [SchemeBarEmpty] = { col_fg,  col_olive, col_olive  },
};

/* tagging */
static const char *tags[] = { "1", "2", "3", "4", "5", "6", "7", "8", "9" };

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class    instance  title  tags mask  isfloating  isterminal  noswallow  monitor */
{ "st-256color",     NULL,     NULL,  0,         0,          1,          0,         -1 },

};

/* layout(s) */
static const float mfact     = 0.55; /* factor of master area size [0.05..0.95] */
static const int nmaster     = 1;    /* number of clients in master area */
static const int resizehints = 1;    /* 1 means respect size hints in tiled resizals */
static const int lockfullscreen = 1; /* 1 will force focus on the fullscreen window */
static const int refreshrate = 120;  /* refresh rate (per second) for client move/resize */

#define FORCE_VSPLIT 1  /* nrowgrid layout: force two clients to always split vertically */

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "[]=",      tile },    /* first entry is default */
	{ "[M]",      monocle },
	{ "[@]",      spiral },
	{ "[\\]",     dwindle },
	{ "H[]",      deck },
	{ "TTT",      bstack },
	{ "===",      bstackhoriz },
	{ "HHH",      grid },
	{ "###",      nrowgrid },
	{ "---",      horizgrid },
	{ ":::",      gaplessgrid },
	{ "|M|",      centeredmaster },
	{ ">M>",      centeredfloatingmaster },
	{ "><>",      NULL },    /* no layout function means floating behavior */
	{ NULL,       NULL },
};

/* key definitions */
#define MODKEY Mod4Mask
#define TAGKEYS(KEY,TAG) \
	&((Keychord){1, {{MODKEY, KEY}},                         view,       {.ui = 1 << TAG} }), \
	&((Keychord){1, {{MODKEY|ControlMask, KEY}},             toggleview, {.ui = 1 << TAG} }), \
	&((Keychord){1, {{MODKEY|ShiftMask, KEY}},               tag,        {.ui = 1 << TAG} }), \
	&((Keychord){1, {{MODKEY|ControlMask|ShiftMask, KEY}},   toggletag,  {.ui = 1 << TAG} }),

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
/* Colors/font deliberately omitted: ~/.local/bin/dmenu (PATH-shadowing wrapper)
 * supplies the Transfiguration theme. Passing them here would override it,
 * since dmenu keeps the LAST value parsed per flag. Retheme dmenu there. */
static const char *dmenucmd[] = { "dmenu_run", "-m", dmenumon, NULL };
static const char *termcmd[]  = {"sh", "-lc", "cd \"$HOME\" && exec st", NULL};
static const char scratchpadname[] = "scratchpad";
static const char *scratchpadcmd[] = { "st", "-t", scratchpadname, "-g", "120x34", NULL };
static const char *rofi[] = {"rofi", "-show", "drun", "-theme", "/home/lukasz/.config/rofi/gruvbox-dwm.rasi", NULL };
static const char *vimwikicmd[] = { "st", "-e", "vim", "+VimwikiIndex", NULL };
static const char *surflaunch[] = { "surf-launch", NULL };

static Keychord *keychords[] = {
    /* modifier / key / function / argument */

    /* cfacts + gaps */
    &((Keychord){1, {{MODKEY, XK_Up}},     setcfact,       {.f = +0.25} }),
    &((Keychord){1, {{MODKEY, XK_Down}},   setcfact,       {.f = -0.25} }),
    &((Keychord){1, {{MODKEY, XK_0}},      setcfact,       {.f =  0.00} }),
    &((Keychord){1, {{MODKEY, XK_equal}},  incrgaps,       {.i = +1 } }),
    &((Keychord){1, {{MODKEY, XK_minus}},  incrgaps,       {.i = -1 } }),
    &((Keychord){1, {{Mod1Mask, XK_0}},    togglegaps,     {0} }),
    &((Keychord){1, {{Mod1Mask|ShiftMask, XK_0}}, defaultgaps, {0} }),

    /* movestack */
    &((Keychord){1, {{MODKEY|ShiftMask, XK_j}}, movestack, {.i = +1 } }),
    &((Keychord){1, {{MODKEY|ShiftMask, XK_k}}, movestack, {.i = -1 } }),

    /* launcher + terminal */
    &((Keychord){1, {{MODKEY, XK_d}},      spawn,          {.v = dmenucmd } }),
    &((Keychord){1, {{MODKEY, XK_r}},      spawn,          {.v = rofi     } }),
    &((Keychord){1, {{MODKEY, XK_Return}}, spawn,          {.v = termcmd } }),
    &((Keychord){3, {{MODKEY, XK_c}, {0, XK_w}, {0, XK_w}}, spawn, {.v = vimwikicmd}}),

    /* basic wm controls */
    &((Keychord){1, {{MODKEY, XK_j}},      focusstack,     {.i = +1 } }),
    &((Keychord){1, {{MODKEY, XK_k}},      focusstack,     {.i = -1 } }),
    &((Keychord){1, {{MODKEY, XK_i}},      incnmaster,     {.i = +1 } }),
    &((Keychord){1, {{MODKEY, XK_p}},      incnmaster,     {.i = -1 } }),
    &((Keychord){1, {{MODKEY, XK_h}},      setmfact,       {.f = -0.05} }),
    &((Keychord){1, {{MODKEY, XK_l}},      setmfact,       {.f = +0.05} }),
    &((Keychord){1, {{MODKEY, XK_z}},      zoom,           {0} }),
    &((Keychord){1, {{MODKEY, XK_Tab}},    view,           {0} }),
    &((Keychord){1, {{MODKEY, XK_q}},      killclient,     {0} }),

    /* application launchers – single key */
    /* application launchers */

    &((Keychord){3, {{MODKEY, XK_a}, {0, XK_f}, {0, XK_f}}, 
    spawn, SHCMD("setsid firefox-esr >/dev/null 2>&1 &")
}),
    &((Keychord){3, {{MODKEY, XK_a}, {0, XK_m}, {0, XK_m}},
    spawn, SHCMD("setsid mullvad-browser >/dev/null 2>&1 &")
}),
    &((Keychord){2, {{MODKEY, XK_a}, {0, XK_t}},
    spawn, SHCMD("setsid thunderbird >/dev/null 2>&1 &")
}),
    &((Keychord){2, {{MODKEY, XK_a}, {0, XK_d}},
    spawn, SHCMD("setsid dino >/dev/null 2>&1 &")
}),

    &((Keychord){1, {{MODKEY, XK_s}},      spawn,          SHCMD("flameshot gui -r | xclip -selection clipboard -t image/png") }),
    &((Keychord){1, {{Mod1Mask, XK_s}}, spawn, SHCMD("flameshot gui") }),
    &((Keychord){1, {{0, XK_Print}}, spawn, SHCMD("flameshot gui -r | xclip -selection clipboard -t image/png") }), /* as on `retro` */
    /* scratchpad: MOD + ` (grave) */
    &((Keychord){1, {{MODKEY, XK_grave}},
        togglescratch, {.v = scratchpadcmd} }),

    /* sticky: mod + a + s */
    &((Keychord){ 2, {{ MODKEY, XK_c }, { 0, XK_s }}, togglesticky, {0} }),

    /* layouts */
    &((Keychord){1, {{MODKEY, XK_m}},      setlayout,      {.v = &layouts[2]} }),
    &((Keychord){1, {{MODKEY, XK_space}},  setlayout,      {0} }),
    &((Keychord){1, {{MODKEY|ShiftMask, XK_space}}, togglefloating, {0} }),

    /* view/tag all */
    &((Keychord){1, {{MODKEY, XK_0}},      view,           {.ui = ~0 } }),
    &((Keychord){1, {{MODKEY|ShiftMask, XK_0}}, tag,       {.ui = ~0 } }),

    /* monitors */
    &((Keychord){1, {{MODKEY, XK_comma}},  focusmon,       {.i = -1 } }),
    &((Keychord){1, {{MODKEY, XK_period}}, focusmon,       {.i = +1 } }),
    &((Keychord){1, {{MODKEY|ShiftMask, XK_comma}}, tagmon, {.i = -1 } }),
    &((Keychord){1, {{MODKEY|ShiftMask, XK_period}}, tagmon, {.i = +1 } }),


    &((Keychord){2, {{MODKEY, XK_v}, {0, XK_m}},
        spawn, SHCMD("pamixer -t") }),           /* mute toggle */
    &((Keychord){2, {{MODKEY, XK_v}, {0, XK_j}},
        spawn, SHCMD("pamixer -d 10") }),           
    &((Keychord){2, {{MODKEY, XK_v}, {0, XK_k}},
        spawn, SHCMD("pamixer -i 10") }),           
    &((Keychord){2, {{MODKEY, XK_v}, {0, XK_p}},
        spawn, SHCMD("st -e pulsemixer") }),         /* pulsemixer */
    &((Keychord){2, {{MODKEY, XK_v}, {0, XK_t}},
        spawn, SHCMD("media-toggle") }),           
    &((Keychord){2, {{MODKEY, XK_b}, {0, XK_k}},
        spawn, SHCMD("brightnessctl set +10%") }),   /* brighter */
    &((Keychord){2, {{MODKEY, XK_b}, {0, XK_j}},
        spawn, SHCMD("brightnessctl set 10%-") }),   /* dimmer */

    /* Keychron Fn-layer spares; F15-F24 keysyms named via xmodmap in .xinitrc.
       F20 = ex-Cortana key (right of PrtSc); Fn+1..4 = F21..F24;
       Fn+5..9 = F15..F19 and F21 (Fn+1) unbound. */
    &((Keychord){1, {{0, XK_F20}}, spawn, SHCMD("slock") }),                  /* ex-Cortana: lock */
    &((Keychord){1, {{0, XK_F22}}, spawn, SHCMD("sink-cycle") }),             /* Fn+2 audio output */
    &((Keychord){1, {{0, XK_F23}}, spawn, SHCMD("dunstctl history-pop") }),   /* Fn+3 notif history */
    &((Keychord){1, {{0, XK_F24}}, spawn, SHCMD("st -e pulsemixer") }),       /* Fn+4 mixer */

    /* Restored from the `retro` branch, which the 6.8 migration left behind.
     * Laptop Fn layer emits F13/F14; media + brightness keys emit XF86XK_*. */
    &((Keychord){1, {{0, XK_F13}}, spawn, SHCMD("$HOME/.local/bin/dfm-drag") }), /* Fn+F3 pick file + drag */
    &((Keychord){1, {{0, XK_F14}}, spawn, SHCMD("st -e vifm") }),             /* Fn+F4 file manager */
    &((Keychord){1, {{0, XK_F21}}, spawn, SHCMD("st -e pulsemixer") }),       /* mixer (Keychron) */
    &((Keychord){1, {{0, XF86XK_AudioMute}},         spawn, SHCMD("pamixer -t") }),
    &((Keychord){1, {{0, XF86XK_AudioRaiseVolume}},  spawn, SHCMD("pamixer -i 10") }),
    &((Keychord){1, {{0, XF86XK_AudioLowerVolume}},  spawn, SHCMD("pamixer -d 10") }),
    &((Keychord){1, {{0, XF86XK_AudioPlay}},         spawn, SHCMD("media-toggle") }),
    &((Keychord){1, {{0, XF86XK_AudioNext}},         spawn, SHCMD("media-next") }),
    &((Keychord){1, {{0, XF86XK_AudioPrev}},         spawn, SHCMD("media-prev") }),
    &((Keychord){1, {{0, XF86XK_MonBrightnessUp}},   spawn, SHCMD("brightnessctl set +10%") }),
    &((Keychord){1, {{0, XF86XK_MonBrightnessDown}}, spawn, SHCMD("brightnessctl set 10%-") }),

/* Mod + c + r + v -> rofi naming conventions*/
&((Keychord){2, {{MODKEY, XK_c}, {0, XK_r}},
    spawn, SHCMD("rofi-cheat")
}),
/* Mod + c + q + q -> shutdown */
&((Keychord){3, {{MODKEY, XK_c}, { ShiftMask, XK_q }, { ShiftMask, XK_q }},
    spawn, SHCMD("loginctl poweroff")
}),

/* Mod + c + p + p -> passmenu */
&((Keychord){3, {{MODKEY, XK_c}, {0, XK_p}, {0, XK_p}},
  spawn, SHCMD("passmenu")
}),
	/* Mod + c + o + o -> passmenu-otp */
&((Keychord){3, {{MODKEY, XK_c}, {0, XK_o}, {0, XK_o}},
  spawn, SHCMD("passmenu-otp")
}),
	/* Mod + c + l + l -> passmenu-otp */
&((Keychord){3, {{MODKEY, XK_c}, {0, XK_l}, {0, XK_l}},
  spawn, SHCMD("clipmenu")
}),

&((Keychord){ 2, {{ MODKEY, XK_c }, { 0, XK_k }},
    spawn, SHCMD("setsid citekey-menu >/dev/null 2>&1 &") }),

&((Keychord){ 2, {{ MODKEY, XK_c }, { 0, XK_c }},
    spawn, SHCMD("setsid dm-edit-config >/dev/null 2>&1 &") }),

&((Keychord){ 3, {{ MODKEY, XK_c }, { 0, XK_f }, { 0, XK_r }},
    spawn, SHCMD("~/.local/bin/dm-frecent") }),

&((Keychord){ 2, {{ MODKEY, XK_c }, { 0, XK_n }},
    spawn, SHCMD("setsid clip-capture >/dev/null 2>&1 &") }),
&((Keychord){ 2, {{ MODKEY, XK_c }, { ShiftMask, XK_n }},
    spawn, SHCMD("setsid st -e vim ~/docs/vimwiki/inbox/clips.md >/dev/null 2>&1 &") }),
&((Keychord){ 2, {{ MODKEY, XK_c }, { 0, XK_m }},
    spawn, SHCMD("setsid dm-memo >/dev/null 2>&1 &") }),
&((Keychord){ 2, {{ MODKEY, XK_c }, { ShiftMask, XK_m }},
    spawn, SHCMD("setsid st -e vim ~/docs/vimwiki/inbox/memo.md  >/dev/null 2>&1 &") }),

&((Keychord){ 3, {{ MODKEY, XK_c }, {0, XK_v}, { 0, XK_o }},
    spawn, SHCMD("setsid dm-vimwiki-open >/dev/null 2>&1 &") }),
&((Keychord){ 3, {{ MODKEY, XK_c }, { 0, XK_v }, { 0, XK_s }},
    spawn, SHCMD("st -e ~/.local/bin/wiki-search") }),

&((Keychord){ 2, {{ MODKEY, XK_c }, { 0, XK_b }},
    spawn, SHCMD("setsid bookmark-type >/dev/null 2>&1 &") }),

/* bookmarks: MOD + c, then Shift+b (add/bookmark) */
&((Keychord){ 2, {{ MODKEY, XK_c }, { ShiftMask, XK_b }},
    spawn, SHCMD("setsid bookmark-clip >/dev/null 2>&1 &") }),

&((Keychord){ 2, {{ MODKEY, XK_c }, { 0, XK_h }},
    spawn, SHCMD("hledger-category-select") }),

&((Keychord){3, {{MODKEY, XK_c}, {0, XK_s}, {0, XK_f}},
  spawn, SHCMD("st -e ~/.sfeed/sfeed.sh")
}),

&((Keychord){1, {{Mod1Mask, XK_o}},
    spawn, SHCMD("opener")
}),
&((Keychord){ 3, {{ MODKEY, XK_c }, { 0, XK_v }, { 0, XK_Return }},
    spawn, SHCMD("setsid vps-term >/dev/null 2>&1 &")
}),
&((Keychord){ 3, {{ MODKEY, XK_c }, { 0, XK_t }, { 0, XK_Return }},
    spawn, SHCMD("setsid sanctum-term >/dev/null 2>&1 &")
}),	
&((Keychord){2, {{MODKEY, XK_a}, {0, XK_s}},
  spawn, SHCMD("surf-launch")
}),
    /* layouts on Alt+1..8 */
    &((Keychord){1, {{Mod1Mask, XK_1}},    setlayout,      {.v = &layouts[0]} }),  /* tile        */
    &((Keychord){1, {{Mod1Mask, XK_2}},    setlayout,      {.v = &layouts[1]} }),  /* monocle     */
    &((Keychord){1, {{Mod1Mask, XK_3}},    setlayout,      {.v = &layouts[5]} }),  /* bstack      */
    &((Keychord){1, {{Mod1Mask, XK_4}},    setlayout,      {.v = &layouts[8]} }),  /* nrowgrid    */
    &((Keychord){1, {{Mod1Mask, XK_5}},    setlayout,      {.v = &layouts[10]} }), /* gaplessgrid */
    &((Keychord){1, {{Mod1Mask, XK_6}},    setlayout,      {.v = &layouts[11]} }), /* centeredmaster */
    &((Keychord){1, {{Mod1Mask, XK_7}},    setlayout,      {.v = &layouts[3]} }),  /* dwindle     */
    &((Keychord){1, {{Mod1Mask, XK_8}},    setlayout,      {.v = &layouts[9]} }),  /* horizgrid   */

    /* tags (from TAGKEYS macro – already keychord-aware) */
    TAGKEYS( XK_1, 0)
    TAGKEYS( XK_2, 1)
    TAGKEYS( XK_3, 2)
    TAGKEYS( XK_4, 3)
    TAGKEYS( XK_5, 4)
    TAGKEYS( XK_6, 5)
    TAGKEYS( XK_7, 6)
    TAGKEYS( XK_8, 7)
    TAGKEYS( XK_9, 8)

    /* quit */
    &((Keychord){1, {{MODKEY|ShiftMask, XK_q}}, quit, {0} }),
    &((Keychord){3, {{MODKEY, XK_c}, {0, XK_l}, {0, XK_o}},
    spawn, SHCMD("slock &") }),
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static const Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ ClkLtSymbol,          0,              Button1,        setlayout,      {0} },
	{ ClkLtSymbol,          0,              Button3,        setlayout,      {.v = &layouts[2]} },
	{ ClkWinTitle,          0,              Button2,        zoom,           {0} },
	{ ClkStatusText,        0,              Button2,        spawn,          {.v = termcmd } },
	{ ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
	{ ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },
	{ ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },
	{ ClkTagBar,            0,              Button1,        view,           {0} },
	{ ClkTagBar,            0,              Button3,        toggleview,     {0} },
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
};
