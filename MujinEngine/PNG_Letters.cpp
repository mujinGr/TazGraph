#include "PNG_Letters.h"

SDL_Rect getLetterRect(char letter) {
	switch (letter) {
	case 'a': return RECT_a;
	case 'b': return RECT_b;
	case 'c': return RECT_c;
	case 'd': return RECT_d;
	case 'e': return RECT_e;
	case 'f': return RECT_f;
	case 'g': return RECT_g;
	case 'h': return RECT_h;
	case 'i': return RECT_i;
	case 'j': return RECT_j;
	case 'k': return RECT_k;
	case 'l': return RECT_l;
	case 'm': return RECT_m;
	case 'n': return RECT_n;
	case 'o': return RECT_o;
	case 'p': return RECT_p;
	case 'q': return RECT_q;
	case 'r': return RECT_r;
	case 's': return RECT_s;
	case 't': return RECT_t;
	case 'u': return RECT_u;
	case 'v': return RECT_v;
	case 'w': return RECT_w;
	case 'x': return RECT_x;
	case 'y': return RECT_y;
	case 'z': return RECT_z;

	case 'A': return RECT_A;
	case 'B': return RECT_B;
	case 'C': return RECT_C;
	case 'D': return RECT_D;
	case 'E': return RECT_E;
	case 'F': return RECT_F;
	case 'G': return RECT_G;
	case 'H': return RECT_H;
	case 'I': return RECT_I;
	case 'J': return RECT_J;
	case 'K': return RECT_K;
	case 'L': return RECT_L;
	case 'M': return RECT_M;
	case 'N': return RECT_N;
	case 'O': return RECT_O;
	case 'P': return RECT_P;
	case 'Q': return RECT_Q;
	case 'R': return RECT_R;
	case 'S': return RECT_S;
	case 'T': return RECT_T;
	case 'U': return RECT_U;
	case 'V': return RECT_V;
	case 'W': return RECT_W;
	case 'X': return RECT_X;
	case 'Y': return RECT_Y;
	case 'Z': return RECT_Z;

	case '0': return RECT_0;
	case '1': return RECT_1;
	case '2': return RECT_2;
	case '3': return RECT_3;
	case '4': return RECT_4;
	case '5': return RECT_5;
	case '6': return RECT_6;
	case '7': return RECT_7;
	case '8': return RECT_8;
	case '9': return RECT_9;

	case '.':return RECT_DOT;
	case ':': return RECT_COLON;
	case ',':return RECT_COMMA;
	case ';':return RECT_SEMICOLON;
	case '(':return RECT_LEFT_PARENTHESIS;
	case '*':return RECT_ASTERISK;
	case '!':return RECT_EXCLAMATION;
	case '?':return RECT_QUESTION;
	case '}':return RECT_RIGHT_BRACK;
	case '^':return RECT_CARET;
	case ')':return RECT_RIGHT_PARENTHESIS;
	case '#':return RECT_HASHTAG;
	case '$':return RECT_DOLLAR;
	case '{':return RECT_LEFT_BRACK;
	case '%':return RECT_PERCENTAGE;
	case '&':return RECT_AMPERSAND;
	case '-':return RECT_DASH;
	case '+':return RECT_PLUS;
	case '@':return RECT_AT;

	case ' ':return RECT_SPACE;

	default:
		// Return a default SDL_Rect in case of an unsupported character.
		return SDL_Rect{ 0, 0, 0, 10 };
	}
}