             /*---,___,                    MC., 19. April 26
                    */
///////////,\//////////////////////////////////////////////|
// {*} '                                           ' {*} '/|
//       Peace, Sleep, and Cats .................        \/|
// {:} _____________________________________________ {:} \/|
//////////////////////////////////////////////////////////.|
  //     ''                                    ''      \\


// Includes Allegro5 and other C libraries
#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/memory.h>
#include <allegro5/utf8.h>

#include <ctype.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

// Windows size
#define WINX 640
#define WINY 480

// Message XY
#define MESSAGE_TOP 20
#define MESSAGE_LEFT 20

// Input XY & cadding
#define INPUT_TOP 80
#define INPUT_LEFT 20
#define INPUT_PAD (LINE_HEIGHT - FONT_SIZE * -1) * 0.5

// Result XY
#define RESULT_TOP 160
#define RESULT_LEFT 20

// Font config
#define TTF_PATH "./font/IBMPlexSans-Light.ttf"
#define FONT_SIZE -32
#define LINE_HEIGHT 40
#define BORDER_RADIUS 10

// Color definitions 
#define BACKGROUND_COLOR al_map_rgb(255, 255, 255)
#define SECONDARY_BACKGROUND_COLOR al_map_rgb(210, 207, 200)
#define COLOR al_map_rgb(0, 0, 0)

// Max length of input
#define LENGTH_INPUT 18

// Message & Printf template for result
#define MESSAGE "Enter IP address and mask (a.b.c.d/m)"
#define RESULT_TMP "Net-ID: %i.%i.%i.%i\nNet-Mask: %i.%i.%i.%i\nMax Hosts: %d"


// IP Octet (0-255)
typedef uint IPOct;

// IP address
struct IPAddr {
  IPOct a;
  IPOct b;
  IPOct c;
  IPOct d;
};

// Subnet Mask (0-31)
typedef uint SubNet;

// Global struct
struct App {
  ALLEGRO_USTR* input;
  int inputCursorX;
  ALLEGRO_USTR* result;
  struct IPAddr ip;
  SubNet sub;

  struct IPAddr netid;
  struct IPAddr net;
  int hosts;

  ALLEGRO_FONT* font;
  
  bool redraw;
  bool quit;
};

// If test false, print description and exit
void must_init(bool test, const char *description);

// Clears IPAdress at pointer ip
void clear_ip(struct IPAddr* ip);

// Mainly zeroes everything
void init_app();

// Logic for key presses
// Sets app.input and app.sub
bool handle_key_down(ALLEGRO_EVENT* event);

// Helper function for parse_input()
bool addCtoInt(ALLEGRO_USTR* ustr, int32_t* pos, uint* i, uint max);
// Sets app.ip based on app.input
bool parse_input();
// Sets app.net, app.netid, app.hosts
void calcNetHosts();

// Draws new line on '\n'
void draw_ustr_ml(ALLEGRO_FONT* font, ALLEGRO_COLOR color, float x, float y, float line_height, int flags, ALLEGRO_USTR* ustr);

// Drawing functions
void draw_message();
void draw_input_rect();
void draw_input_text();
void draw_result();

// main() gets called first
int main();
