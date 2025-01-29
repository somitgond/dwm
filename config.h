/* See LICENSE file for copyright and license details. */
#include <X11/XF86keysym.h>

/* appearance */
static const unsigned int borderpx       = 2;        /* border pixel of windows */
static const unsigned int snap           = 16;       /* snap pixel */
static const unsigned int gappx          = 3;             /* gaps between windows */
static const unsigned int systraypinning = 0;   /* 0: sloppy systray follows selected monitor, >0: pin systray to monitor X */
static const unsigned int systrayonleft  = 0;    /* 0: systray in the right corner, >0: systray on left of status text */
static const unsigned int systrayspacing = 4;   /* systray spacing */
static const int systraypinningfailfirst = 1;   /* 1: if pinning fails, display systray on the first monitor, False: display systray on the last monitor*/
static const int showsystray        = 1;        /* 0 means no systray */
static const int showbar            = 1;        /* 0 means no bar */
static const int topbar             = 1;        /* 0 means bottom bar */
static const int focusonwheel       = 0;
static const char *fonts[]          = { "IBM Plex Mono:style=Bold:size=11:antialias=true:autohint=true" };
static const char dmenufont[]       = "IBM Plex Mono:style=Bold:size=11:antialias=true:autohint=true";
static const char col_gray1[]       = "#222222";
static const char col_gray2[]       = "#444444";
static const char col_gray3[]       = "#e7d7ad";
static const char col_gray4[]       = "#eeeeee";
static const char col_cyan[]        = "#005577"; 
static const char col_gray[]        = "#a89985"; 
static const char col_black[]       = "#1d2021";  
static const char col_orange[]      = "#d79921";  
static const char col_greendim[]   = "#689d6a";  
static const char *colors[][3]      = {
	/*               fg         bg         border   */
	[SchemeNorm] = { col_gray3, col_black, col_black},
	[SchemeSel]  = { col_black, col_gray,  col_greendim},
};

/* tagging  Japanese/chinese characters*/
//static const char *tags[] = { "一", "二", "三", "四", "五", "六", "七", "八", "九" };
static const char *tags[] = { "1", "2", "3", "4", "5", "6", "7", "8", "9"};
//static const char *tags[] = { "A", "B", "C", "D", "E", "F", "G", "H", "I" };

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class      instance    title       tags mask     isfloating   monitor */
	{ "Firefox",          NULL,       NULL,       2,            0,           -1 },
};

/* window following */
#define WFACTIVE '>'
#define WFINACTIVE '|'
#define WFDEFAULT WFINACTIVE

/* layout(s) */
static const float mfact     = 0.5; /* factor of master area size [0.05..0.95] */
static const int nmaster     = 1;    /* number of clients in master area */
static const int resizehints = 0;    /* 1 means respect size hints in tiled resizals */
static const int lockfullscreen = 1; /* 1 will force focus on the fullscreen window */

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "[]=",      tile },    /* first entry is default */
	{ "><>",      NULL },    /* no layout function means floating behavior */
	{ "[M]",      monocle },
   	{ "|M|",      centeredmaster },
	{ ">M>",      centeredfloatingmaster },
   	{ "|||",      tcl },
	{ NULL,       NULL },
};

/* key definitions */
#define MODKEY Mod4Mask
#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[] = { "dmenu_run", "-m", dmenumon, "-fn", dmenufont, "-nb", col_black, "-nf", col_gray3, "-sb", col_gray, "-sf", col_black, NULL };
static const char *termcmd[]  = { "st", "-e", "tmux", "-u", NULL };
//static const char *termcmd[]  = { "alacritty", "-e", "tmux", NULL };
static const char scratchpadname[] = "scratchpad";
static const char *scratchpadcmd[] = { "st", "-t", scratchpadname, "-g", "80x24","-e", "tmux", "-u", NULL };
static const char *firefoxcmd[]  = { "firefox", NULL };
static const char *browserChromecmd[]  = { "google-chrome-stable", NULL };
static const char *filecmd[]  = { "thunar", NULL };
// volume and brightness keys and other Fn shortcuts
static const char *up_vol[]   = { "pactl", "set-sink-volume", "@DEFAULT_SINK@", "+5%",   NULL };
static const char *down_vol[] = { "pactl", "set-sink-volume", "@DEFAULT_SINK@", "-5%",   NULL };
static const char *mute_vol[] = { "pactl", "set-sink-mute",   "@DEFAULT_SINK@", "toggle", NULL };
static const char *mute_mic[] = { "pactl", "set-source-mute",   "@DEFAULT_SOURCE@", "toggle", NULL };
static const char *brighter[] = { "brightnessctl", "set", "5%+", NULL };
static const char *dimmer[]   = { "brightnessctl", "set", "4%-", NULL };
static const char *networkManagerOff[]   = { "nmcli", "networking", "off", NULL };
static const char *lck[]   = { "slock", NULL };

// change keyboard layout to colemak
static const char *us_keyboard[]   = { "setxkbmap", "us", NULL };

static const Key keys[] = {
	/* modifier                     key        function        argument */
	{ MODKEY,                       XK_p,      spawn,          {.v = dmenucmd } },
	{ MODKEY,                       XK_Return, spawn,          {.v = termcmd } },
    { MODKEY,                       XK_grave,  togglescratch,  {.v = scratchpadcmd } },
    { MODKEY|ShiftMask,             XK_f,      spawn,          {.v = firefoxcmd } },
    { MODKEY|ShiftMask,             XK_c,      spawn,          {.v = browserChromecmd } },
    { MODKEY|ShiftMask,             XK_e,      spawn,          {.v = filecmd } },
    { MODKEY|ShiftMask,             XK_l,      spawn,          {.v = lck} },

	{ MODKEY|ShiftMask,             XK_b,      togglebar,      {0} },
	{ MODKEY|ShiftMask,             XK_n,      togglefollow,   {0} },
	{ MODKEY,                       XK_j,      focusstack,     {.i = +1 } },
	{ MODKEY,                       XK_k,      focusstack,     {.i = -1 } },
	{ MODKEY,                       XK_i,      incnmaster,     {.i = +1 } },
	{ MODKEY,                       XK_d,      incnmaster,     {.i = -1 } },
	{ MODKEY,                       XK_h,      setmfact,       {.f = -0.05} },
	{ MODKEY,                       XK_l,      setmfact,       {.f = +0.05} },
   	{ MODKEY|ShiftMask,             XK_j,      movestack,      {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_k,      movestack,      {.i = -1 } },
	{ MODKEY,                       XK_semicolon,    zoom,     {0} },
	{ MODKEY,                       XK_Tab,    view,           {0} },
	{ MODKEY,                       XK_c,      killclient,     {0} },
	{ MODKEY,                       XK_t,      setlayout,      {.v = &layouts[0]} },
	{ MODKEY,                       XK_f,      setlayout,      {.v = &layouts[1]} },
    { MODKEY,                       XK_m,      setlayout,      {.v = &layouts[2]} },
   	//{ MODKEY,                       XK_u,      setlayout,      {.v = &layouts[3]} },
	//{ MODKEY,                       XK_o,      setlayout,      {.v = &layouts[4]} },
   	{ MODKEY,           	       	XK_space,  cyclelayout,    {.i = -1 } },
	//{ MODKEY|ControlMask,           XK_period, cyclelayout,    {.i = +1 } },
	//{ MODKEY|ControlMask,           XK_space,  setlayout,      {0} },
	{ MODKEY|ShiftMask,             XK_space,  togglefloating, {0} },
	{ MODKEY,                       XK_0,      view,           {.ui = ~0 } },
	{ MODKEY|ShiftMask,             XK_0,      tag,            {.ui = ~0 } },
	{ MODKEY,                       XK_comma,  focusmon,       {.i = -1 } },
	{ MODKEY,                       XK_period, focusmon,       {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_comma,  tagmon,         {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_period, tagmon,         {.i = +1 } },
    { MODKEY|ShiftMask,             XK_q,      quit,           {0} },
   	{ MODKEY,                       XK_n,	shiftview,         { .i = +1 } },
	{ MODKEY,                       XK_b,	shiftview,         { .i = -1 } },
    { 0,                            XF86XK_AudioMicMute,     spawn, {.v = mute_mic } },
    { 0,                            XF86XK_AudioMute,        spawn, {.v = mute_vol } },
    { 0,                            XF86XK_AudioLowerVolume, spawn, {.v = down_vol } },
    { 0,                            XF86XK_AudioRaiseVolume, spawn, {.v = up_vol } },
    { 0,                            XF86XK_MonBrightnessDown, spawn, {.v = dimmer } },
    { 0,                            XF86XK_MonBrightnessUp,   spawn, {.v = brighter } },
    { 0,                            XF86XK_WLAN,              spawn, {.v = networkManagerOff } },
    { 0,                            XF86XK_Tools,             spawn, {.v = firefoxcmd } },
    { 0,                            XF86XK_Bluetooth,         spawn, {.v = browserChromecmd } },
    { 0,                            XF86XK_Favorites,         spawn, {.v = scratchpadcmd } },
	TAGKEYS(                        XK_1,                      0)
	TAGKEYS(                        XK_2,                      1)
	TAGKEYS(                        XK_3,                      2)
	TAGKEYS(                        XK_4,                      3)
	TAGKEYS(                        XK_5,                      4)
	TAGKEYS(                        XK_6,                      5)
	TAGKEYS(                        XK_7,                      6)
	TAGKEYS(                        XK_8,                      7)
	TAGKEYS(                        XK_9,                      8)
	{ MODKEY|ControlMask|ShiftMask, XK_q,      quit,           {1} }, 
	{ MODKEY|ControlMask,           XK_k,                      spawn, {.v = us_keyboard}}, 
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static const Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ ClkLtSymbol,          0,              Button1,        setlayout,      {0} },
	{ ClkLtSymbol,          0,              Button3,        setlayout,      {.v = &layouts[2]} },
	{ ClkFollowSymbol,      0,              Button1,        togglefollow,   {0} },
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

