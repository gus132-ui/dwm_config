/* See LICENSE file for copyright and license details. */
#define FORCE_VSPLIT 1  /* 1: nrowgrid forces two clients vertical split, 0: auto */
/* appearance */
static const unsigned int borderpx  = 2;        /* border pixel of windows */
static const unsigned int snap      = 32;       /* snap pixel */
static const int showbar            = 1;        /* 0 means no bar */
static const int topbar             = 1;        /* 0 means bottom bar */
static const unsigned int gappih    = 10;  /* horiz inner gap between windows */
static const unsigned int gappiv    = 10;  /* vert inner gap between windows */
static const unsigned int gappoh    = 10;  /* horiz outer gap between windows and screen edge */
static const unsigned int gappov    = 10;  /* vert outer gap between windows and screen edge */
static const int smartgaps          = 0;   /* 1 means no outer gap when there is only one window */
static const char *fonts[]          = { "Hack Nerd Font:size=10" };
static const char dmenufont[]       = "Hack Nerd Font:size=10";
static const char col_bg[]          = "#1d2021";
static const char col_fg[]          = "#ebdbb2";
static const char col_fg_dim[]      = "#a88984";
static const char col_accent[]      = "#d79921";

static const char *colors[][3]      = {
	/*               fg         bg         border   */
	[SchemeNorm] = { col_fg_dim,  col_bg, col_bg    },
	[SchemeSel]  = { col_accent, col_bg, col_accent },
};

/* tagging */
static const char *tags[] = { "1", "2", "3", "4", "5", "6", "7", "8", "9" };

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class        instance  title      tags mask     isfloating   monitor */
	{ "firefox",   NULL,  NULL,       1 << 0,       0,           -1 },
	{ "Protonvpn-app", NULL,  NULL,       1 << 8,       1,            1 },
};

/* layout(s) */
static const float mfact     = 0.55; /* factor of master area size [0.05..0.95] */
static const int nmaster     = 1;    /* number of clients in master area */
static const int resizehints = 1;    /* 1 means respect size hints in tiled resizals */
static const int lockfullscreen = 1; /* 1 will force focus on the fullscreen window */
static const int refreshrate = 120;  /* refresh rate (per second) for client move/resize */

static const Layout layouts[] = {
	/* symbol     arrange function */
    { "[]=",      tile },                /* first entry is default */
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
    { "><>",      NULL },                /* floating */
    { NULL,       NULL },
};

/* key definitions */
#define MODKEY Mod4Mask
#define TAGKEYS(KEY,TAG) \
       &((Keychord){1, {{MODKEY, KEY}},                                        view,           {.ui = 1 << TAG} }), \
       &((Keychord){1, {{MODKEY|ControlMask, KEY}},                            toggleview,     {.ui = 1 << TAG} }), \
       &((Keychord){1, {{MODKEY|ShiftMask, KEY}},                              tag,            {.ui = 1 << TAG} }), \
       &((Keychord){1, {{MODKEY|ControlMask|ShiftMask, KEY}},                  toggletag,      {.ui = 1 << TAG} }),
/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[] = {
       	"dmenu_run", "-m", dmenumon, "-fn", dmenufont,
       	"-nb", col_bg,
       	"-nf", col_fg_dim,
       	"-sb", col_accent,
       	"-sf", col_fg,
       	NULL
};
static const char *termcmd[]  = {"sh", "-lc", "cd \"$HOME\" && exec st", NULL};
static const char scratchpadname[] = "scratchpad";
static const char *scratchpadcmd[] = { "st", "-t", scratchpadname, "-g", "120x34", NULL };
static const char *rofi[] = {"rofi", "-show", "drun", "-theme", "/home/lukasz/.config/rofi/gruvbox-dwm.rasi", NULL };
static const char *vimwikicmd[] = { "st", "-e", "vim", "+VimwikiIndex", NULL };
static const char *notes_cmd[] = { "st", "-e", "vim", "/home/lukasz/.notes", NULL };
static const char *clips_cmd[] = { "st", "-e", "vim", "/home/lukasz/.clips", NULL };

#include "movestack.c"
static Keychord *keychords[] = {
    /* modifier / key / function / argument */

    /* cfacts + gaps */
    &((Keychord){1, {{MODKEY, XK_Up}},     setcfact,       {.f = +0.25} }),
    &((Keychord){1, {{MODKEY, XK_Down}},   setcfact,       {.f = -0.25} }),
    &((Keychord){1, {{MODKEY, XK_r}},      setcfact,       {.f =  0.00} }),
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

    /* Mod + a + f + f → Firefox ESR (with firejail) */
    &((Keychord){3, {{MODKEY, XK_a}, {0, XK_f}, {0, XK_f}}, 
    spawn, SHCMD("setsid /home/lukasz/.local/bin/firefox >/dev/null 2>&1 &")
}),
    /* Mod + a + f + r → Firefox (without firejail) */
    &((Keychord){3, {{MODKEY, XK_a}, {0, XK_f}, {0, XK_r}}, 
    spawn, SHCMD("setsid /home/lukasz/.local/bin/firefox-raw >/dev/null 2>&1 &")
}),
    /* Mod + c + f + f -> Reset ChatGPT (reset-gpt) */
&((Keychord){3, {{MODKEY, XK_c}, {0, XK_f}, {0, XK_f}},
    spawn, SHCMD("setsid /home/lukasz/.local/bin/reset-gpt >/dev/null 2>&1 &")
}),
    /* Mod + a + m + v → Mullvad (set correct binary name) */
    &((Keychord){3, {{MODKEY, XK_a}, {0, XK_m}, {0, XK_m}},
    spawn, SHCMD("setsid mullvad-browser >/dev/null 2>&1 &")
}),
    /* Mod + a + t → Thunderbird */
    &((Keychord){2, {{MODKEY, XK_a}, {0, XK_t}},
    spawn, SHCMD("setsid thunderbird >/dev/null 2>&1 &")
}),
    /* Mod + a + o → Obsidian */
    &((Keychord){2, {{MODKEY, XK_a}, {0, XK_o}},
    spawn, SHCMD("setsid obsidian >/dev/null 2>&1 &")
}),

    &((Keychord){1, {{MODKEY, XK_s}},      spawn,          SHCMD("flameshot gui -r | xclip -selection clipboard -t image/png") }),
    /* Proton suite chords: MOD + a, then p + letter */
    &((Keychord){3, {{MODKEY, XK_a}, {0, XK_p}, {0, XK_m}},
        spawn, SHCMD("setsid proton-mail >/dev/null 2&1 &") }),      
    /* scratchpad: MOD + ` (grave) */
    &((Keychord){1, {{MODKEY, XK_grave}},
        togglescratch, {.v = scratchpadcmd} }),

    /* sticky: mod + a + s */
    &((Keychord){ 2, {{ MODKEY, XK_a }, { 0, XK_s }}, togglesticky, {0} }),

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


    /* Volume: Mod + a, v, k/j/m */
    &((Keychord){3, {{MODKEY, XK_a}, {0, XK_v}, {0, XK_k}},
        spawn, SHCMD("pamixer -i 10") }),         /* volume up */
    &((Keychord){3, {{MODKEY, XK_a}, {0, XK_v}, {0, XK_j}},
        spawn, SHCMD("pamixer -d 10") }),         /* volume down */
    &((Keychord){3, {{MODKEY, XK_a}, {0, XK_v}, {0, XK_m}},
        spawn, SHCMD("pamixer -t") }),           /* mute toggle */

    /* Brightness: Mod + a, b, k/j */
    &((Keychord){3, {{MODKEY, XK_a}, {0, XK_b}, {0, XK_k}},
        spawn, SHCMD("brightnessctl set +10%") }),   /* brighter */
    &((Keychord){3, {{MODKEY, XK_a}, {0, XK_b}, {0, XK_j}},
        spawn, SHCMD("brightnessctl set 10%-") }),   /* dimmer */

/* Mod + c + r + v -> rofi vim power */
&((Keychord){3, {{MODKEY, XK_c}, {0, XK_r}, {0, XK_v}},
    spawn, SHCMD("rofi-vim-power")
}),
/* Mod + c + r + v -> rofi notmuch */
&((Keychord){3, {{MODKEY, XK_c}, {0, XK_r}, {0, XK_n}},
    spawn, SHCMD("rofi-notmuch")
}),

/* Mod + c + r + v -> rofi notmuch */
&((Keychord){3, {{MODKEY, XK_c}, {0, XK_r}, {0, XK_t}},
    spawn, SHCMD("rofi-task-names")
}),
/* Mod + c + q + q -> shutdown */
&((Keychord){3, {{MODKEY, XK_c}, {0, XK_q}, {0, XK_q}},
    spawn, SHCMD("systemctl poweroff")
}),

/* Mod + c + p + p -> passmenu */
&((Keychord){3, {{MODKEY, XK_c}, {0, XK_p}, {0, XK_p}},
  spawn, SHCMD("passmenu")
}),
	/* Mod + c + p + o -> passmenu-otp */
&((Keychord){3, {{MODKEY, XK_c}, {0, XK_o}, {0, XK_o}},
  spawn, SHCMD("passmenu-otp")
}),

&((Keychord){ 2, {{ MODKEY, XK_c }, { 0, XK_k }},
    spawn, SHCMD("setsid citekey-menu >/dev/null 2>&1 &") }),

&((Keychord){ 2, {{ MODKEY, XK_c }, { 0, XK_s }},
    spawn, SHCMD("setsid dmenu-scripts >/dev/null 2>&1 &") }),

&((Keychord){ 2, {{ MODKEY, XK_c }, { 0, XK_c }},
    spawn, SHCMD("setsid dmenu-edit-config >/dev/null 2>&1 &") }),

&((Keychord){ 2, {{ MODKEY, XK_c }, { 0, XK_n }},
    spawn, SHCMD("setsid capture-clip >/dev/null 2>&1 &") }),
&((Keychord){ 2, {{ MODKEY, XK_c }, { ShiftMask, XK_n }},
    spawn, SHCMD("setsid st -e vim ~/.clips >/dev/null 2>&1 &") }),
&((Keychord){ 2, {{ MODKEY, XK_c }, { 0, XK_m }},
    spawn, SHCMD("setsid dmenu-memo >/dev/null 2>&1 &") }),
&((Keychord){ 2, {{ MODKEY, XK_c }, { ShiftMask, XK_m }},
    spawn, SHCMD("setsid st -e vim ~/.notes  >/dev/null 2>&1 &") }),

&((Keychord){ 2, {{ MODKEY, XK_c }, { 0, XK_v }},
    spawn, SHCMD("setsid dmenu-vimwiki-open >/dev/null 2>&1 &") }),

&((Keychord){ 2, {{ MODKEY, XK_c }, { 0, XK_b }},
    spawn, SHCMD("setsid typebookmarks >/dev/null 2>&1 &") }),

/* bookmarks: MOD + c, then Shift+b (add/bookmark) */
&((Keychord){ 2, {{ MODKEY, XK_c }, { ShiftMask, XK_b }},
    spawn, SHCMD("setsid bookmarkthis >/dev/null 2>&1 &") }),
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
    &((Keychord){3, {{MODKEY, XK_a}, {0, XK_l}, {0, XK_o}},
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

