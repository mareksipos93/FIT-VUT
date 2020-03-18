/*
	Marek Šipoš (xsipos03)
	Provedené změny: implementace světelných novin (99 %)
	Datum poslední změny: 28.12 2017
*/
/*******************************************************************************
   main.c: Interaktivni svetelne noviny
   Copyright (C) 2012 Brno University of Technology,
                      Faculty of Information Technology
   Author(s): 
   Marek Sipos (xsipos03 AT stud.fit.vutbr.cz)
   Michal Bidlo <bidlom AT fit.vutbr.cz>

   LICENSE TERMS

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:
   1. Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in
      the documentation and/or other materials provided with the
      distribution.
   3. All advertising materials mentioning features or use of this software
      or firmware must display the following acknowledgement:

        This product includes software developed by the University of
        Technology, Faculty of Information Technology, Brno and its
        contributors.

   4. Neither the name of the Company nor the names of its contributors
      may be used to endorse or promote products derived from this
      software without specific prior written permission.

   This software or firmware is provided ``as is'', and any express or implied
   warranties, including, but not limited to, the implied warranties of
   merchantability and fitness for a particular purpose are disclaimed.
   In no event shall the company or contributors be liable for any
   direct, indirect, incidental, special, exemplary, or consequential
   damages (including, but not limited to, procurement of substitute
   goods or services; loss of use, data, or profits; or business
   interruption) however caused and on any theory of liability, whether
   in contract, strict liability, or tort (including negligence or
   otherwise) arising in any way out of the use of this software, even
   if advised of the possibility of such damage.

   $Id$


*******************************************************************************/

#include <fitkitlib.h>
#include <keyboard/keyboard.h>
#include <lcd/display.h>

#define ROW_SIZE 16	// Max pocet znaku na radek
#define MAX_ROWS 10	// Max pocet radku textu

#define TIMER_PROMPT 8000 // Interval blikani podtrzitka pro vstup (max 65535)
#define TIMER_WRITE 30000 // Interval pro zapsani prave psaneho znaku (max 65535)
#define TIMER_ANIMATION 500 // Interval pro provadeni nejrychlejsi z animaci - ostatni vyuzivaji nasobku hodnoty (max 1024)

char mapping[10][5] = { // Mapovani znaku na klavesy, znak '#' je nevyuzite misto
		{' ', '_', '-', '0', '#'},	// 0
		{'.', ',', ':', '1', '#'},	// 1
		{'A', 'B', 'C', '2', '#'},	// 2
		{'D', 'E', 'F', '3', '#'},	// 3
		{'G', 'H', 'I', '4', '#'},	// 4
		{'J', 'K', 'L', '5', '#'},	// 5
		{'M', 'N', 'O', '6', '#'},	// 6
		{'P', 'Q', 'R', 'S', '7'},	// 7
		{'T', 'U', 'V', '8', '#'},	// 8
		{'W', 'X', 'Y', 'Z', '9'}	// 9
	};

char last_ch = '\0'; 		// Naposledy precteny znak
int input_buffer_size = 0;	// Pocet zadanych znaku
int display_buffer_size = 1;// Pocet zobrazovanych znaku (1 protoze tam je prompt)
int display_buffer_anim_size = 0; // Pocet zobrazovanych znaku (pri animovani)
int is_input = 1;			// Zda je aktivni rezim vkladani
int shown_row = 0;			// Aktualne zobrazovany radek (od 0)
int timer = 0;				// Casovac

int timer_prompt = 0;		// V jakem case timeru dojde k bliknuti podtrzitka pro vstup
int timer_write = -1;		// V jakem case timeru dojde k zapsani prave psaneho znaku

int need_redraw = 0;		// Zda ma byt na konci iterace prekreslen display
int prompt_shown = 1;		// Zda je zrovna podtrzitko pro vstup zobrazeno;

char write_current = '\0';	// Kterou klavesou prave iteruje pri zapisu
int write_iteration = -1;	// Kolikaty znak dane klavesy prave zapisuje, pokud je -1 tak nic nezapisuje

int timer_animation = -1;	// V jakem case timeru dojde k animaci, nastavenim na -1 se animace zastavi
int animation_progress = -1;// Postup animovani, nastavenim na -1 se animace provede od zacatku
int animation_type = -1;	// Jaka animace probiha

char input_buffer[ROW_SIZE*MAX_ROWS];	// Buffer celeho vstupniho textu (vzdy zakonceno podtrzitkem pro vstup)
char display_buffer[ROW_SIZE*2];		// Buffer displaye - 2 radky
char display_buffer_anim[ROW_SIZE*2];	// Buffer displaye (pri animovani) - 2 radky

/********* NUTNE ZLO *********/
void print_user_help(void) { }
unsigned char decode_user_cmd(char *cmd_ucase, char *cmd) { return CMD_UNKNOWN; }
void fpga_initialized() { }
/*****************************/

/****** HLAVICKY FUNKCI ******/
int keyboard_idle();
void init_input_mode();
void input_char(char c);
void input_chars(char *text);
void delete_char();
void show_prompt_immediately();
void set_demo_text(char *text);
void animation();
void display_redraw();
/*****************************/

int main(void)
{

	display_buffer[0] = '_'; // prompt_shown = 1

	initialize_hardware();
	keyboard_init();
	LCD_init();
	LCD_clear();

	while(1)
	{
		timer++;

		// Problikavani prompt (podtrzitka)
		if (is_input && timer == timer_prompt) {
			if (prompt_shown) {
				prompt_shown = 0;
				if (input_buffer_size < MAX_ROWS*ROW_SIZE && timer_write == -1)
					display_buffer[display_buffer_size-1] = ' ';
			} else {
				prompt_shown = 1;
				if (input_buffer_size < MAX_ROWS*ROW_SIZE && timer_write == -1)
					display_buffer[display_buffer_size-1] = '_';
			}
			need_redraw = 1;
			timer_prompt += TIMER_PROMPT;
		}

		// Zapsani prave psaneho znaku
		if (is_input && write_iteration != -1 && timer == timer_write) {
			show_prompt_immediately();
			timer_write = -1;
			write_current = '\0';
			write_iteration = -1;
		}

		// Provedeni animace
		if (!is_input && timer == timer_animation) {
			if (animation_type >= 0 && animation_type <= 3)
				animation();
			else {
				LCD_write_string("POUZIJTE KLAVESUA, B, C, D");
				timer_animation = -1;
				animation_progress = -1;
			}
		}

		keyboard_idle();
		terminal_idle();

		// Pokud nekdo volal po prekresleni displaye, tak se prekresli z display_buffer
		if (need_redraw) {
			need_redraw = 0;
			display_redraw();
		}
	}
	return 0;
}

int keyboard_idle()
{
	char ch;
	ch = key_decode(read_word_keyboard_4x4());
	if (ch != last_ch) { // Osetreni drzeni klavesy
		last_ch = ch;
		if (ch != 0) { // Pokud byla stisknuta klavesa
			if (is_input) { // Rezim zapisu
				if (ch >= '0' && ch <= '9') {
					/* Nemel rozepsany zadny znak */
					if (write_iteration == -1 && input_buffer_size < ROW_SIZE*MAX_ROWS) {
						write_current = ch;
						write_iteration = 0;
						timer_write = timer + TIMER_WRITE;
						input_char(mapping[ch - '0'][0]);
					} else {
						/* Mel rozepsany znak a iteruje jim */
						if (write_current == ch) {
							timer_write = timer + TIMER_WRITE;
							if (write_iteration < 4 && mapping[ch - '0'][write_iteration+1] != '#')
								write_iteration++;
							else
								write_iteration = 0;
							delete_char();
							input_char(mapping[ch - '0'][write_iteration]);
						/* Mel rozepsany znak a ted chce napsat dalsi */
						} else if (input_buffer_size < ROW_SIZE*MAX_ROWS) {
							write_current = ch;
							write_iteration = 0;
							timer_write = timer + TIMER_WRITE;
							input_char(mapping[ch - '0'][0]);
						}
					}
				} else if (ch == '*') { // Smazani znaku
					if (input_buffer_size > 0) {
						delete_char();
						timer_write = -1;
						write_current = '\0';
						write_iteration = -1;
					}
				} else if (ch == '#') { // Prepnuti do rezimu cteni
					int can_switch = 0;
					int i;
					for (i = 0; i < input_buffer_size; i++) {
						if (input_buffer[i] != ' ') {
							can_switch = 1;
							break;
						}
					}
					if (can_switch) {
						is_input = 0;
						LCD_write_string("POUZIJTE KLAVESUA, B, C, D");
					}
				} else if (ch == 'A') { // Testovaci text 1
					set_demo_text("ZAKLADNI TEXT...");
				} else if (ch == 'B') { // Testovaci text 2
					set_demo_text("TATO VETA JE NA DVA RADKY");
				} else if (ch == 'C') { // Testovaci text 3
					set_demo_text("TOTO JE VELMI DLOUHY TEXT DEMONSTRUJICI TEXT DELSI NEZ 32 ZNAKU");
				} else if (ch == 'D') { // Testovaci text 4
					set_demo_text("0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ _-.,:");
				}
			} else { // Rezim cteni

				// Preruseni animace
				if (animation_progress != -1) {
					timer_animation = -1;
					animation_progress = -1;
					animation_type = -1;
					display_buffer_anim_size = 0;
					LCD_write_string("POUZIJTE KLAVESUA, B, C, D");
				}

				if (ch == '*') {
					init_input_mode();
					show_prompt_immediately();
				} else if (ch == 'A') {
					animation_progress = -1;
					animation_type = 0;
					animation();
				} else if (ch == 'B') {
					animation_progress = -1;
					animation_type = 1;
					animation();
				} else if (ch == 'C') {
					animation_progress = -1;
					animation_type = 2;
					animation();
				} else if (ch == 'D') {
					animation_progress = -1;
					animation_type = 3;
					animation();
				}
			}
		}
	}
	return 0;
}

/* Nastavi program do vkladaciho rezimu, nijak neovlivnuje buffery ani zobrazovani */
void init_input_mode() {
	timer = 0;
	timer_prompt = 0;
	timer_write = -1;
	prompt_shown = 1;
	write_current = '\0';
	write_iteration = -1;
	is_input = 1;
}

/* Prida znak do vstupu a upravi display */
void input_char(char c) {
	show_prompt_immediately();

	if (input_buffer_size < ROW_SIZE*MAX_ROWS) {
		input_buffer[input_buffer_size] = c;
		input_buffer_size++;

		if (display_buffer_size < ROW_SIZE*2) {
			display_buffer[display_buffer_size-1] = c;
			display_buffer_size++;
		} else {
			if (input_buffer_size < ROW_SIZE*MAX_ROWS) {
				display_buffer[display_buffer_size-1] = c;
				int i;
				for (i = 0; i < ROW_SIZE; i++) {
					display_buffer[i] = display_buffer[i+ROW_SIZE];
				}
				shown_row++;
				display_buffer_size -= ROW_SIZE-1;
			}
		}
	}
}

/* Prida vice znaku do vstupu a upravi display */
void input_chars(char *text) {
	int i = 0;
	while (text[i] != '\0') {
		input_char(text[i]);
		i++;
	}
}

/* Odstrani znak ze vstupu a upravi display */
void delete_char() {
	show_prompt_immediately();

	if (input_buffer_size > 0) {
		input_buffer_size--;
		if (display_buffer_size > ROW_SIZE+1 || shown_row == 0)
			display_buffer_size--;
		else if (shown_row > 0) {
			int i;
			for (i = 0; i < ROW_SIZE; i++) {
				display_buffer[i+ROW_SIZE] = display_buffer[i];
				display_buffer[i] = input_buffer[((shown_row-1)*ROW_SIZE)+i];
			}
			shown_row--;
			display_buffer_size = ROW_SIZE*2;
		}
	}
}

/* Zobrazi podtrzitko okamzite */
void show_prompt_immediately() {
	timer_prompt = timer+1;
	prompt_shown = 0;
	need_redraw = 1;
}

/* Nastavi nejaky demonstracni vstupni text v rezimu uprav */
void set_demo_text(char *text) {
	init_input_mode();
	input_buffer_size = 0;
	display_buffer_size = 1;
	shown_row = 0;
	input_chars(text);
}

/* Provede krok animace */
void animation() {
	static int animation_subprogress = -1; // Pomocna promenna pro nektere animace
	static int animation_pass = -1; // Pomocna promenna pro dvoji pruchod
	if (animation_type == 0) { // Postupne vypisovani

		// Animace zacina od zacatku, inicializace
		if (animation_progress == -1) {
			animation_progress = 0;
			timer_animation = timer + (TIMER_ANIMATION * 3);
			display_buffer_anim_size = 0;
			display_redraw();
		// Animace jiz probiha
		} else {
			// Animace konci
			if (input_buffer_size == animation_progress) {
				timer_animation += TIMER_ANIMATION * 60; // Chvilku pocka nez zobrazi vyzvu k opetovne animaci pomoci A,B,C,D
				animation_type = -1;
				display_buffer_anim_size = 0;
			// Animace pokracuje
			} else {
				if (display_buffer_anim_size == ROW_SIZE*2) {
					display_buffer_anim_size -= ROW_SIZE;
					int i;
					for (i = 0; i < ROW_SIZE; i++) {
						display_buffer_anim[i] = display_buffer_anim[ROW_SIZE+i];
					}
				}
				timer_animation = timer + (TIMER_ANIMATION * 3);
				display_buffer_anim[display_buffer_anim_size] = input_buffer[animation_progress];
				display_buffer_anim_size++;
				animation_progress++;
				display_redraw();
			}
		}

	} else if (animation_type == 1) { // Postupne vypisovani odzadu

		// Animace zacina od zacatku, inicializace
		if (animation_progress == -1) {
			animation_progress = 0;
			animation_subprogress = 0; // pocet skutecne vykreslovanych znaku (ne ty mezery pred tim)
			timer_animation = timer + (TIMER_ANIMATION * 3);
			display_buffer_anim_size = ROW_SIZE*2;
			display_redraw();
		// Animace jiz probiha
		} else {
			// Animace konci
			if (input_buffer_size == animation_progress) {
				timer_animation += TIMER_ANIMATION * 60; // Chvilku pocka nez zobrazi vyzvu k opetovne animaci pomoci A,B,C,D
				animation_type = -1;
				display_buffer_anim_size = 0;
			// Animace pokracuje
			} else {
				if (animation_subprogress == ROW_SIZE*2) {
					animation_subprogress -= ROW_SIZE;
					int i;
					for (i = 0; i < ROW_SIZE; i++) {
						display_buffer_anim[ROW_SIZE+i] = display_buffer_anim[i];
						display_buffer_anim[i] = ' ';
					}
				}
				timer_animation = timer + (TIMER_ANIMATION * 3);
				int i;
				for (i = 0; i < ROW_SIZE*2 - (animation_subprogress+1); i++) {
					display_buffer_anim[i] = ' ';
				}
				display_buffer_anim[ROW_SIZE*2-animation_subprogress-1] = input_buffer[input_buffer_size-animation_progress-1];
				animation_subprogress++;
				animation_progress++;
				display_redraw();
			}
		}

	} else if (animation_type == 2) { // Postupne vypisovani dvojim pruchodem
		
		// Animace zacina od zacatku, inicializace
		if (animation_progress == -1) {
			animation_progress = 0;
			animation_subprogress = 0; // kolikaty znak na dane obrazovce se vypisuje
			animation_pass = 0; // kolikaty pruchod to je
			timer_animation = timer + (TIMER_ANIMATION * 3);
			display_buffer_anim_size = 0;
			display_redraw();
		// Animace jiz probiha
		} else {			
			// Animace konci
			if (animation_progress >= input_buffer_size) {
				timer_animation += TIMER_ANIMATION * 60; // Chvilku pocka nez zobrazi vyzvu k opetovne animaci pomoci A,B,C,D
				animation_type = -1;
				display_buffer_anim_size = 0;
			// Animace pokracuje
			} else {
				// Prvni pruchod dane obrazovky
				if (animation_pass == 0) {
					display_buffer_anim[display_buffer_anim_size] = input_buffer[animation_progress + animation_subprogress];
					display_buffer_anim_size++;
					animation_subprogress++;
					if (display_buffer_anim_size < ROW_SIZE*2) {
						display_buffer_anim[display_buffer_anim_size] = ' ';
						display_buffer_anim_size++;
						animation_subprogress++;
					}
					if (display_buffer_anim_size == ROW_SIZE*2 || (animation_progress + animation_subprogress) >= input_buffer_size) {
						animation_subprogress = 1;
						animation_pass = 1;
					}
				// Druhy pruchod dane obrazovky
				} else {
					if (animation_subprogress >= ROW_SIZE*2) {
						animation_subprogress = 0;
						animation_pass = 0;
						display_buffer_anim_size = 0;
					} else {
						if (input_buffer_size >= animation_progress+2)
							display_buffer_anim[animation_subprogress] = input_buffer[animation_progress+1];
						animation_subprogress += 2;
						animation_progress += 2;
					}
				}
				timer_animation = timer + (TIMER_ANIMATION * 3);
				display_redraw();
			}
		}

	} else if (animation_type == 3) { // Postupne vypisovani "vytacenim"
		
		// Animace zacina od zacatku, inicializace
		if (animation_progress == -1) {
			animation_progress = 0;
			timer_animation = timer + TIMER_ANIMATION;
			display_buffer_anim_size = 1;
			display_buffer_anim[0] = ' ';
			display_redraw();
		// Animace jiz probiha
		} else {
			// Animace konci
			if (input_buffer_size == animation_progress) {
				timer_animation += TIMER_ANIMATION * 60; // Chvilku pocka nez zobrazi vyzvu k opetovne animaci pomoci A,B,C,D
				animation_type = -1;
				display_buffer_anim_size = 0;
			// Animace pokracuje
			} else {
				// Dosahl vytaceneho znaku
				if (display_buffer_anim[display_buffer_anim_size-1] == input_buffer[animation_progress]) {
					animation_progress++;
					if (display_buffer_anim_size < ROW_SIZE*2 && input_buffer_size != animation_progress) {
						if (input_buffer[animation_progress] >= '{') {
							display_buffer_anim[display_buffer_anim_size] = '{';
						} else if (input_buffer[animation_progress] >= 'a') {
							display_buffer_anim[display_buffer_anim_size] = 'a';
						} else if (input_buffer[animation_progress] >= '[') {
							display_buffer_anim[display_buffer_anim_size] = '[';
						} else if (input_buffer[animation_progress] >= 'A') {
							display_buffer_anim[display_buffer_anim_size] = 'A';
						} else if (input_buffer[animation_progress] >= ':') {
							display_buffer_anim[display_buffer_anim_size] = ':';
						} else if (input_buffer[animation_progress] >= '0') {
							display_buffer_anim[display_buffer_anim_size] = '0';
						} else {
							display_buffer_anim[display_buffer_anim_size] = ' ';
						}
						display_buffer_anim_size++;
					}
					// Odradkovani
					else if (display_buffer_anim_size == ROW_SIZE*2) {
						display_buffer_anim_size -= ROW_SIZE;
						int i;
						for (i = 0; i < ROW_SIZE; i++) {
							display_buffer_anim[i] = display_buffer_anim[ROW_SIZE+i];
						}
					}
				} else {
					display_buffer_anim[display_buffer_anim_size-1]++;
				}
				timer_animation = timer + TIMER_ANIMATION;
				display_redraw();
			}
		}
	}
}

/* Vypise urcite znaky z bufferu */
void display_redraw()
{
	LCD_clear();
	if (is_input) {
		int i;
		for (i = 0; i < display_buffer_size; i++) {
			// Tato podminka osetruje vypisovani prompt jen tehdy, je-li to potreba
			if (i < display_buffer_size-1 || (i == display_buffer_size-1 && input_buffer_size < ROW_SIZE*MAX_ROWS && write_iteration == -1))
				LCD_append_char(display_buffer[i]);
		}
	} else {
		int i;
		for (i = 0; i < display_buffer_anim_size; i++) {
			LCD_append_char(display_buffer_anim[i]);
		}
	}
}
