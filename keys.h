/* ----------- keys.h ------------ */
#ifndef KEYS_H
#define KEYS_H
#define RUBOUT        8
#define BELL          7
#define ESC          27
#define ALT_BS      197
#define SHIFT_DEL   198
#define CTRL_INS    186
#define SHIFT_INS   185
#define F1          187
#define F2          188
#define F3          189
#define F4          190
#define F5          191
#define F6          192
#define F7          193
#define F8          194
#define F9          195
#define F10         196
#define CTRL_F1     222
#define CTRL_F2     223
#define CTRL_F3     224
#define CTRL_F4     225
#define CTRL_F5     226
#define CTRL_F6     227
#define CTRL_F7     228
#define CTRL_F8     229
#define CTRL_F9     230
#define CTRL_F10    231
#define ALT_F1      232
#define ALT_F2      233
#define ALT_F3      234
#define ALT_F4      235
#define ALT_F5      236
#define ALT_F6      237
#define ALT_F7      238
#define ALT_F8      239
#define ALT_F9      240
#define ALT_F10     241
#define HOME        199
#define UP          200
#define PGUP        201
#define BS          203
#define FWD         205
#define END         207
#define DN          208
#define PGDN        209
#define INS         210
#define DEL         211
#define CTRL_HOME   247
#define CTRL_PGUP   132
#define CTRL_BS     243
#define CTRL_FIVE   143
#define CTRL_FWD    244
#define CTRL_END    245
#define CTRL_PGDN   246
#define SHIFT_HT    143
#define ALT_A       158
#define ALT_B       176
#define ALT_C       174
#define ALT_D       160
#define ALT_E       146
#define ALT_F       161
#define ALT_G       162
#define ALT_H       163
#define ALT_I       151
#define ALT_J       164
#define ALT_K       165
#define ALT_L       166
#define ALT_M       178
#define ALT_N       177
#define ALT_O       152
#define ALT_P       153
#define ALT_Q       144
#define ALT_R       147
#define ALT_S       159
#define ALT_T       148
#define ALT_U       150
#define ALT_V       175
#define ALT_W       145
#define ALT_X       173
#define ALT_Y       149
#define ALT_Z       172
#define ALT_1      0xf8
#define ALT_2      0xf9
#define ALT_3      0xfa
#define ALT_4      0xfb
#define ALT_5      0xfc
#define ALT_6      0xfd
#define ALT_7      0xfe
#define ALT_8      0xff
#define ALT_9      0x80
#define ALT_0      0x81
#define ALT_HYPHEN  130

#define RIGHTSHIFT 0x01
#define LEFTSHIFT  0x02
#define CTRLKEY    0x04
#define ALTKEY     0x08
#define SCROLLLOCK 0x10
#define NUMLOCK    0x20
#define CAPSLOCK   0x40
#define INSERTKEY  0x80

struct keys {
    int keycode;
    char *keylabel;
};
int getkey(void);
int getshift(void);
int keyhit(void);
void beep(void);
extern struct keys keys[];
extern char altconvert[];

#endif

