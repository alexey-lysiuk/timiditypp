#ifndef ___W32G_UTL_H_
#define ___W32G_UTL_H_

// ini & config
#define IniVersion "2.0"
typedef struct SETTING_PLAYER_ {
// Main Window
	int InitMinimizeFlag;
// SubWindow Starting Create Flag
	int DebugWndStartFlag;
	int ConsoleWndStartFlag;
	int ListWndStartFlag;
	int TracerWndStartFlag;
	int DocWndStartFlag;
	int WrdWndStartFlag;
// SubWindow Starting Valid Flag
	int DebugWndFlag;
	int ConsoleWndFlag;
	int ListWndFlag;
	int TracerWndFlag;
	int DocWndFlag;
	int WrdWndFlag;
	int SoundSpecWndFlag;
// SubWindow Max Numer
	int SubWindowMax;
// Default File
	char ConfigFile[MAXPATH + 32];
	char PlaylistFile[MAXPATH + 32];
	char PlaylistHistoryFile[MAXPATH + 32];
// Default Dir
	char MidiFileOpenDir[MAXPATH + 32];
	char ConfigFileOpenDir[MAXPATH + 32];
	char PlaylistFileOpenDir[MAXPATH + 32];
// Thread Priority
	int PlayerThreadPriority;
	int GUIThreadPriority;
// Font
	char SystemFont[256];
	char PlayerFont[256];
	char WrdFont[256];
	char DocFont[256];
	char ListFont[256];
	char TracerFont[256];
	int SystemFontSize;
	int PlayerFontSize;
	int WrdFontSize;
	int DocFontSize;
	int ListFontSize;
	int TracerFontSize;
// Misc.
	int WrdGraphicFlag;
	int TraceGraphicFlag;
	int DocMaxSize;
	char DocFileExt[256];
// End.
} SETTING_PLAYER;

typedef struct SETTING_TIMIDITY_ {
    // Parameter from command line options.

    int32 amplification;	// A
    int antialiasing_allowed;	// a
    int buffer_fragments;	// B
    int32 control_ratio;	// C
				// c (ignore)
    ChannelBitMask default_drumchannels, default_drumchannel_mask; // D
				// d (ignore)

				// E...
    int opt_modulation_wheel;	// E w/W
    int opt_portamento;		// E p/P
    int opt_nrpn_vibrato;  	// E v/V
    int opt_channel_pressure;	// E s/S
    int opt_trace_text_meta_event; // E t/T
    int opt_overlap_voice_allow;// E o/O
    int opt_default_mid;	// E mXX
    int default_tonebank;	// E b
    int special_tonebank;	// E B
    int effect_lr_mode;		// E Fdelay
    int effect_lr_delay_msec;	// E Fdelay
    int opt_reverb_control;	// E Freverb
    int opt_chorus_control;	// E Fchorus
    int noise_sharp_type;	// E Fns

    int opt_evil_mode;		// e
    int adjust_panning_immediately; // F
    int fast_decay;		// f
#ifdef SUPPORT_SOUNDSPEC
    int view_soundspec_flag;	// g
    double spectrogram_update_sec; // g
#endif
				// h (ignore)
    int default_program[MAX_CHANNELS]; // I
    char opt_ctl[16];		// i
    int opt_realtime_playing;	// j
    int reduce_voice_threshold; // k
				// L (ignore)
    char opt_playmode[16];	// O
    char OutputName[MAXPATH + 32]; // o : string
				// P (ignore)
    int voices;			// p
    ChannelBitMask quietchannels; // Q
    char opt_qsize[16];		// q
    int32 modify_release;	// R
    int32 allocate_cache_size;	// S
    int output_rate;		// s
    char output_text_code[16];	// t
    int free_instruments_afterwards; // U
    char opt_wrd[16];		// W
#if defined(__W32__) && defined(SMFCONV)
    int opt_rcpcv_dll;		// wr, wR
#endif
				// x (ignore)
				// Z (ignore)
} SETTING_TIMIDITY;

extern char *OutputName;

extern void LoadIniFile(SETTING_PLAYER *sp,  SETTING_TIMIDITY *st);
extern void SaveIniFile(SETTING_PLAYER *sp,  SETTING_TIMIDITY *st);

extern SETTING_PLAYER *sp_default, *sp_current, *sp_temp;
extern SETTING_TIMIDITY *st_default, *st_current, *st_temp;
extern CHAR *INI_INVALID;
extern CHAR *INI_SEC_PLAYER;
extern CHAR *INI_SEC_TIMIDITY;
extern char *SystemFont;
extern char *PlayerFont;
extern char *WrdFont;
extern char *DocFont;
extern char *ListFont;
extern char *TracerFont;
extern HFONT hSystemFont;
extern HFONT hPlayerFont;
extern HFONT hWrdFont;
extern HFONT hDocFont;
extern HFONT hListFont;
extern HFONT hTracerFont;
extern int SystemFontSize;
extern int PlayerFontSize;
extern int WrdFontSize;
extern int DocFontSize;
extern int ListFontSize;
extern int TracerFontSize;

extern int IniGetKeyInt32(char *section, char *key,int32 *n);
extern int IniGetKeyInt32Array(char *section, char *key, int32 *n, int arraysize);
extern int IniGetKeyInt(char *section, char *key, int *n);
extern int IniGetKeyChar(char *section, char *key, char *c);
extern int IniGetKeyIntArray(char *section, char *key, int *n, int arraysize);
extern int IniGetKeyString(char *section, char *key,char *str);
extern int IniGetKeyStringN(char *section, char *key,char *str, int size);
extern int IniGetKeyFloat(char *section, char *key, FLOAT_T *n);
extern int IniPutKeyInt32(char *section, char *key,int32 *n);
extern int IniPutKeyInt32Array(char *section, char *key, int32 *n, int arraysize);
extern int IniPutKeyInt(char *section, char *key, int *n);
extern int IniPutKeyChar(char *section, char *key, char *c);
extern int IniPutKeyIntArray(char *section, char *key, int *n, int arraysize);
extern int IniPutKeyString(char *section, char *key, char *str);
extern int IniPutKeyStringN(char *section, char *key, char *str, int size);
extern int IniPutKeyFloat(char *section, char *key, FLOAT_T n);
extern void ApplySettingPlayer(SETTING_PLAYER *sp);
extern void SaveSettingPlayer(SETTING_PLAYER *sp);
extern void ApplySettingTiMidity(SETTING_TIMIDITY *st);
extern void SaveSettingTiMidity(SETTING_TIMIDITY *st);
extern int IniVersionCheck(void);
extern void BitBltRect(HDC dst, HDC src, RECT *rc);
extern TmColors tm_colors[ /* TMCC_SIZE */ ];
#define TmCc(c) (tm_colors[c].color)
extern void TmInitColor(void);
extern void TmFreeColor(void);
extern void TmFillRect(HDC hdc, RECT *rc, int color);
extern void w32g_initialize(void);
extern int is_directory(char *path);
extern int directory_form(char *path_in_out);

#endif /* ___W32G_UTL_H_ */
