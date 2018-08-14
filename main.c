#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <assert.h>

#define SDL_FAIL(fn) fprintf(stderr, "[ERROR] %s failed with: %s\n", fn, SDL_GetError())
#define TTF_FAIL(fn) fprintf(stderr, "[ERROR] %s failed with: %s\n", fn, TTF_GetError())

char * grow_buffer(char *buffer, int len, int *new_len) {
    return (char *)realloc(buffer, sizeof(char) * len * 2);
}

int main() {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        SDL_FAIL("SDL_Init");
        exit(1);
    }
    atexit(SDL_Quit);

    if (TTF_Init() != 0) {
        TTF_FAIL("TTF_Init");
        exit(1);
    }
    atexit(TTF_Quit);

    TTF_Font * font = TTF_OpenFont("resources/font.ttf", 12);
    if (font == NULL) {
        TTF_FAIL("TTF_OpenFont");
        exit(1);
    }

    SDL_Window *wnd = SDL_CreateWindow("Hello SDL", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 500, 500,
                                       SDL_WINDOW_SHOWN);
    if (wnd == NULL) {
        SDL_FAIL("SDL_CreateWindow");
        exit(1);
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(wnd, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer == NULL) {
        SDL_FAIL("SDL_Renderer");
        SDL_DestroyWindow(wnd);
        TTF_CloseFont(font);
        exit(1);
    }

    bool running = true;
    SDL_Event event;

    int cx = 0, cy = 0;
    int nrows = 0;
    char **rows = NULL;

    SDL_StartTextInput();

    bool cmd_focus = false;
    int cmdcx = 0;
    char *cmdline = calloc(50, sizeof(char));
    size_t cmdline_len = 0, cmdline_cap = 50;
    SDL_Keymod keymod = SDL_GetModState();
    while (running) {
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_KEYDOWN:
                    keymod = SDL_GetModState();

                    if (cmd_focus) {
                        switch (event.key.keysym.sym) {
                            case SDLK_RETURN:
                                switch (cmdline[0]) {
                                    case ':':
                                    {
                                        printf("CMD: %s\n", cmdline+1);
                                        break;
                                    }
                                    default:
                                    {
                                        printf("FILE: %s\n", cmdline);
                                        break;
                                    }
                                }
                                cmdline[0] = '\0';
                                cmdline_len = 0;
                                cmdcx = 0;
                                cmd_focus = false;
                                break;

                            case SDLK_BACKSPACE:
                                if (cmdcx == 0)
                                    break;

                                // cmdline :1234c5678 len=9 cmdcx=5
                                //
                                if (cmdline_len - cmdcx > 0) {
                                    memmove(cmdline+cmdcx-1, cmdline+cmdcx, cmdline_len-cmdcx);
                                }

                                cmdline_len--;
                                cmdcx--;
                                cmdline[cmdline_len] = '\0';
                                break;

                            case SDLK_LEFT:
                                if (cmdcx > 0)
                                    cmdcx--;
                                break;

                            case SDLK_RIGHT:
                                if (cmdcx < cmdline_len)
                                    cmdcx++;
                                break;
                        }
                    } else if (keymod & KMOD_CTRL) {
                        switch (event.key.keysym.sym) {
                            case SDLK_p:
                                cmd_focus = true;
                                cmdline[0] = ':';
                                cmdline[1] = '\0';
                                cmdline_len = 1;
                                cmdcx = 1;
                                break;
                            default:
                                break;
                        }
                    }
                    break;
                case SDL_TEXTINPUT:
                    if (cmd_focus) {
                        int chindex = 0;
                        while (event.text.text[chindex] != '\0' && chindex < 32) {
                            // append key presses to cmdline
                            if (cmdline_len == cmdline_cap)
                                cmdline = realloc(cmdline, sizeof(char) * cmdline_cap * 2);

                            if (cmdcx != cmdline_len)
                                memmove(cmdline+cmdcx+1, cmdline+cmdcx, cmdline_len-cmdcx+1);
                            else
                                cmdline[cmdcx+1] = '\0';

                            cmdline[cmdcx] = event.text.text[chindex];
                            cmdline_len++;
                            cmdcx++;
                            chindex++;
                        }
                    } else {

                    }
                    break;
                case SDL_QUIT:
                    running = false;
                    break;
                default:
                    break;
            }
        }

        SDL_SetRenderDrawColor(renderer, 20, 20, 20, SDL_ALPHA_OPAQUE);
        SDL_RenderClear(renderer);

        if (cmd_focus) {
            SDL_Rect cmdline_rect;
            cmdline_rect.x = 0;
            cmdline_rect.y = 500-40;
            cmdline_rect.w = 500;
            cmdline_rect.h = 40;

            SDL_SetRenderDrawColor(renderer, 40, 40, 40, 255);
            SDL_RenderFillRect(renderer, &cmdline_rect);

            SDL_Color fg = {.a = 255, .r = 255, .g = 255, .b = 255};

            SDL_Surface *cmdline_surface = TTF_RenderUTF8_Solid(font, cmdline, fg);
            SDL_Texture *cmdline_tex = SDL_CreateTextureFromSurface(renderer, cmdline_surface);
            SDL_FreeSurface(cmdline_surface);

            int width, height;
            SDL_QueryTexture(cmdline_tex, NULL, NULL, &width, &height);

            SDL_Rect dest = {.x = 5, .y = 500-40+5, .w = width, .h = height};
            SDL_RenderCopy(renderer, cmdline_tex, NULL, &dest);
            SDL_DestroyTexture(cmdline_tex);
        }
        SDL_RenderPresent(renderer);
    }

    free(cmdline);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(wnd);
    exit(0);
}