#include "main.h"

struct App app;

void must_init(bool test, const char *description)
{
    if(test) return;

    printf("couldn't initialize %s\n", description);
    exit(1);
}

void clear_ip(struct IPAddr* ip) {
  ip->a = 0;
  ip->b = 0;
  ip->c = 0;
  ip->d = 0;
}

void init_app() {
  app.input = al_ustr_new("");
  app.result = al_ustr_new("");
  clear_ip(&app.ip);
  clear_ip(&app.net);
  clear_ip(&app.netid);
  app.inputCursorX = 0;
  app.hosts = 0;
  app.sub = 0;
  app.redraw = false;
  app.quit = false;
}

bool handle_key_down(ALLEGRO_EVENT* event) {
  int keyc = event->keyboard.keycode;
  int32_t keyu = event->keyboard.unichar;
  
  if(keyc == ALLEGRO_KEY_ESCAPE) {
    app.quit = true;
    return true;
    
  } else if(keyc == ALLEGRO_KEY_BACKSPACE && al_ustr_prev(app.input, &app.inputCursorX)) {
    al_ustr_truncate(app.input, app.inputCursorX);
  
  } else if(app.inputCursorX < LENGTH_INPUT
            && (isdigit((unsigned char)keyu)
            || keyu == '.'
            || keyu == '/')) {
    app.inputCursorX += al_ustr_append_chr(app.input, keyu);
    
  } else return false;

  app.redraw = true;
  return true;
}

bool addCtoInt(ALLEGRO_USTR* ustr, int32_t* pos, uint* i, uint max) {
  if(!ustr || !pos || !i) return false;
  if(!isdigit((unsigned char)al_ustr_get(ustr, *pos)) || al_ustr_get(ustr, *pos) < 0) return false;
  uint n = 0;
  while(1) {
    n = n * 10 + (unsigned char)al_ustr_get(ustr, (*pos)++) - '0';
    if(n > max) return false;
    if(!isdigit((unsigned char)al_ustr_get(ustr, *pos)) || al_ustr_get(ustr, *pos) < 0) break;
  };
  *i = n;
  return true;
}

bool parse_input() {
  ALLEGRO_USTR* ustr = app.input;
  int pos = 0;
  IPOct* ip[3] = {&app.ip.a, &app.ip.b, &app.ip.c};

  for(int i = 0; i < 3; i++) 
    if(!addCtoInt(ustr, &pos, ip[i], 255) || al_ustr_get(ustr, pos++) != '.') return false;
  if(!addCtoInt(ustr, &pos, &app.ip.d, 255)) return false;

  if(al_ustr_get(ustr, pos++) != '/') return false;
  if(!addCtoInt(ustr, &pos, &app.sub, 31)) return false;
  
  return true;
}

void calcNetHosts() {
  IPOct* net[4] = {&app.net.a, &app.net.b, &app.net.c, &app.net.d};
  IPOct* netid[4] = {&app.netid.a, &app.netid.b, &app.netid.c, &app.netid.d};
  IPOct* ip[4] = {&app.ip.a, &app.ip.b, &app.ip.c, &app.ip.d};

  clear_ip(&app.net);
  clear_ip(&app.netid);
  app.hosts = 0;
  
  uint i = 0;
  for(uint x = 0; x < 4; x++) {
    for(uint y = 0; y < 8 && i < app.sub; y++, i++)
      *net[x] = *net[x] | 0x80 >> y;
    *netid[x] = *net[x] & *ip[x];
  }

  if(app.sub == 31)
    app.hosts = 2;
  else
    app.hosts = pow(2, 32 - app.sub) - 2;
}

void draw_message() {
  al_draw_text(app.font, COLOR, MESSAGE_LEFT, MESSAGE_TOP, 0, MESSAGE);
}
void draw_input_rect() {
  al_draw_filled_rounded_rectangle(INPUT_LEFT, INPUT_TOP, WINX - INPUT_LEFT, INPUT_TOP + LINE_HEIGHT, BORDER_RADIUS, BORDER_RADIUS, SECONDARY_BACKGROUND_COLOR);
}
void draw_input_text() {
  al_draw_ustr(app.font, COLOR, INPUT_LEFT + INPUT_PAD, INPUT_TOP + INPUT_PAD, 0, app.input);
}
void draw_result() {
  al_ustr_truncate(app.result, 0);
  al_ustr_appendf(app.result, RESULT_TMP,
                  app.netid.a, app.netid.b, app.netid.c, app.netid.d,
                  app.net.a, app.net.b, app.net.c, app.net.d,
                  app.hosts);
  al_draw_multiline_ustr(app.font, COLOR, RESULT_LEFT, RESULT_TOP, WINX - 2 * RESULT_LEFT, LINE_HEIGHT, 0, app.result);
}

int main() {
  must_init(al_init(), "al_init()");
  must_init(al_init_ttf_addon(), "al_init_ttf_addon()");
  must_init(al_init_primitives_addon(), "al_init_primitives_addon()");
  must_init(al_install_keyboard(), "al_install_keyboard()");

  ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue();
  must_init(queue, "queue");
  ALLEGRO_DISPLAY* disp = al_create_display(WINX, WINY);
  must_init(disp, "display");
  app.font = al_load_ttf_font(TTF_PATH, FONT_SIZE, 0);
  must_init(app.font, "font file");
 
  init_app(); 

  al_register_event_source(queue, al_get_keyboard_event_source());
  al_register_event_source(queue, al_get_display_event_source(disp));

  ALLEGRO_EVENT event;

  app.redraw = true;
  while(1) {
    if(app.redraw && al_is_event_queue_empty(queue)) {
      al_clear_to_color(BACKGROUND_COLOR);

      draw_message();
      draw_input_rect();
      draw_input_text();
      
      if(parse_input()) {
        calcNetHosts();
      } else {
        clear_ip(&app.ip);
        clear_ip(&app.net);
        clear_ip(&app.netid);
        app.hosts = 0;
        app.sub = 0;
      }
        
      draw_result();

      al_flip_display();

      app.redraw = false;
    }
    
    al_wait_for_event(queue, &event);

    switch(event.type) {
      case ALLEGRO_EVENT_KEY_CHAR:
        handle_key_down(&event);
        break;
      case ALLEGRO_EVENT_DISPLAY_CLOSE:
        app.quit = true;
        break;
    }

    if(app.quit == true) break;    
  }

  al_ustr_free(app.input);
  al_ustr_free(app.result);
  al_destroy_font(app.font);
  al_destroy_display(disp);
  al_destroy_event_queue(queue);

  return 0;
}
