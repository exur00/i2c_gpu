#define TERMINAL_I2C_ADDRESS 0x37


#define WIDTH	640	// screen width in pixels
#define HEIGHT	480	// screen height
#define FONTW	8	// font width
#define FONTH	8	// font height
#define TEXTW	(WIDTH/FONTW) // text width (=80)
#define VGA_NUM_COLS    TEXTW
#define TEXTH	(HEIGHT/FONTH) // text height (=60)
#define TEXTWB	TEXTW // text width byte (=80)
#define TEXTSIZE (TEXTWB*TEXTH) // text box size in bytes (=4800)

// Single byte transmission write the byte as char
// Multiple byte transmissions have a command code as first byte.
// Commands: 
#define PRINT_STRING        0x0 // print rest of transmission as string
#define CONTROL_COMMAND     0x1 // 2nd byte describes command
#define SET_TEXT_COLOR      0x2 // 2nd byte describes color
#define SET_BACKGROUND_COLOR      0x2 // 2nd byte describes color


// Control commands:
#define CLS         0x0 // Clear Screen, set cursor to top left
#define DELETE      0x1 // Clear current position
#define BACKSPACE   0x2 // Move to previous position and clear
#define SCROLL_DOWN 0x3 // Move all text up 1 line
#define AUTO_SCROLL_ON 0x4
#define AUTO_SCROLL_OFF

// Colors:
#define PC_COLOR(bg,fg) (((bg)<<4)|(fg))

#define PC_BLACK	0x0
#define PC_BLUE		0x1
#define PC_GREEN	0x2
#define PC_CYAN		0x3
#define PC_RED		0x4
#define PC_MAGENTA	0x5
#define PC_BROWN	0x6
#define PC_LTGRAY	0x7
#define PC_GRAY		0x8
#define PC_LTBLUE	0x9
#define PC_LTGREEN	0xa
#define PC_LTCYAN	0xb
#define PC_LTRED	0xc
#define PC_LTMAGENTA	0xd
#define PC_YELLOW	0xe
#define PC_WHITE	0xf
