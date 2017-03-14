/* nuklear - 1.32.0 - public domain */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <math.h>
#include <limits.h>
#include <time.h>

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_opengl.h>

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_IMPLEMENTATION
#define NK_SDL_GL2_IMPLEMENTATION
#include "nuklear.h"
#include "nuklear_sdl_gl2.h"

#define WINDOW_WIDTH 1200
#define WINDOW_HEIGHT 800

#define MAX_VERTEX_MEMORY 512 * 1024
#define MAX_ELEMENT_MEMORY 128 * 1024

#define UNUSED(a) (void)a
#define MIN(a,b) ((a) < (b) ? (a) : (b))
#define MAX(a,b) ((a) < (b) ? (b) : (a))
#define LEN(a) (sizeof(a)/sizeof(a)[0])

/* ===============================================================
 *
 *                          DEMO
 *
 * ===============================================================*/
static struct nk_image load_image(const char *filename, int *w, int *h);
static void draw_tex(
	GLuint texid, const float x, const float y, const float w, const float h);
int
main(int argc, char* argv[])
{
	// Check args
	if (argc != 2)
	{
		printf("Usage: nuklear_sdl_gl2_cmake <filename>\n");
		return 0;
	}

    /* Platform */
    SDL_Window *win;
    SDL_GLContext glContext;
	struct nk_image tex;
	int tex_w, tex_h;
    struct nk_color background;
    int win_width, win_height;
    int running = 1;

    /* GUI */
    struct nk_context *ctx;

    /* SDL setup */
    SDL_SetHint(SDL_HINT_VIDEO_HIGHDPI_DISABLED, "1");
    SDL_Init(SDL_INIT_VIDEO);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    win = SDL_CreateWindow("Demo",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_OPENGL|SDL_WINDOW_SHOWN);
    SDL_GetWindowSize(win, &win_width, &win_height);
    glContext = SDL_GL_CreateContext(win);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	tex = load_image(argv[1], &tex_w, &tex_h);
	if (tex.handle.id == 0)
	{
		goto cleanup;
	}

    /* GUI */
    ctx = nk_sdl_init(win);

    /* Load Fonts: if none of these are loaded a default font will be used  */
    /* Load Cursor: if you uncomment cursor loading please hide the cursor */
    {struct nk_font_atlas *atlas;
    nk_sdl_font_stash_begin(&atlas);
    nk_sdl_font_stash_end();
    /*nk_style_load_all_cursors(ctx, atlas->cursors);*/
    /*nk_style_set_font(ctx, &roboto->handle)*/;}

    background = nk_rgb(28,48,62);
    while (running)
    {
        /* Input */
        SDL_Event evt;
        nk_input_begin(ctx);
        while (SDL_PollEvent(&evt)) {
            if (evt.type == SDL_QUIT) goto cleanup;
            nk_sdl_handle_event(&evt);
        }
        nk_input_end(ctx);

        /* GUI */
        if (nk_begin(ctx, "Demo", nk_rect(50, 50, 210, 250),
            NK_WINDOW_BORDER|NK_WINDOW_MOVABLE|NK_WINDOW_SCALABLE|
            NK_WINDOW_MINIMIZABLE|NK_WINDOW_TITLE))
        {
            enum {EASY, HARD};
            static int op = EASY;
            static int property = 20;

            nk_layout_row_static(ctx, 30, 80, 1);
            if (nk_button_label(ctx, "button"))
                fprintf(stdout, "button pressed\n");
            nk_layout_row_dynamic(ctx, 30, 2);
            if (nk_option_label(ctx, "easy", op == EASY)) op = EASY;
            if (nk_option_label(ctx, "hard", op == HARD)) op = HARD;
            nk_layout_row_dynamic(ctx, 25, 1);
            nk_property_int(ctx, "Compression:", 0, &property, 100, 10, 1);

            nk_layout_row_dynamic(ctx, 20, 1);
            nk_label(ctx, "background:", NK_TEXT_LEFT);
            nk_layout_row_dynamic(ctx, 25, 1);
            if (nk_combo_begin_color(ctx, background, nk_vec2(nk_widget_width(ctx),400))) {
                nk_layout_row_dynamic(ctx, 120, 1);
                background = nk_color_picker(ctx, background, NK_RGBA);
                nk_layout_row_dynamic(ctx, 25, 1);
                background.r = (nk_byte)nk_propertyi(ctx, "#R:", 0, background.r, 255, 1,1);
                background.g = (nk_byte)nk_propertyi(ctx, "#G:", 0, background.g, 255, 1,1);
                background.b = (nk_byte)nk_propertyi(ctx, "#B:", 0, background.b, 255, 1,1);
                background.a = (nk_byte)nk_propertyi(ctx, "#A:", 0, background.a, 255, 1,1);
                nk_combo_end(ctx);
            }

			nk_layout_row_dynamic(ctx, 20, 1);
            nk_label(ctx, "Image:", NK_TEXT_LEFT);
			nk_layout_row_static(
				ctx,
				tex_h, tex_w,
				1);
			nk_image(ctx, tex);
        }
        nk_end(ctx);

        /* Draw */
        {float bg[4];
        nk_color_fv(bg, background);
        SDL_GetWindowSize(win, &win_width, &win_height);
        glViewport(0, 0, win_width, win_height);
        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(bg[0], bg[1], bg[2], bg[3]);

		// Blit the texture to screen
		draw_tex(tex.handle.id, -0.5f, -0.5f, 1.f, 1.f);

        /* IMPORTANT: `nk_sdl_render` modifies some global OpenGL state
         * with blending, scissor, face culling, depth test and viewport and
         * defaults everything back into a default state.
         * Make sure to either a.) save and restore or b.) reset your own state after
         * rendering the UI. */
        nk_sdl_render(NK_ANTI_ALIASING_ON, MAX_VERTEX_MEMORY, MAX_ELEMENT_MEMORY);

		// Display
		SDL_GL_SwapWindow(win);}
    }

cleanup:
    nk_sdl_shutdown();
	glDeleteTextures(1, (const GLuint *)&tex.handle.id);
    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 0;
}

static struct nk_image load_image(const char *filename, int *w, int *h)
{
	GLuint texid = 0;
	// Load image from file
	SDL_Surface *s = IMG_Load(filename);
	if (s == NULL)
	{
		printf("Failed to load image from file %s\n", filename);
		goto cleanup;
	}
	// Convert to RGBA
	SDL_Surface *tmp = SDL_ConvertSurfaceFormat(s, SDL_PIXELFORMAT_ABGR8888, 0);
	if (tmp == NULL)
	{
		printf("Failed to convert surface: %s\n", SDL_GetError());
		goto cleanup;
	}
	SDL_FreeSurface(s);
	s = tmp;
	// Convert to texture
	glGenTextures(1, &texid);
	glBindTexture(GL_TEXTURE_2D, texid);
	int mode = GL_RGBA;
	glTexImage2D(
		GL_TEXTURE_2D, 0, mode, s->w, s->h, 0, mode, GL_UNSIGNED_BYTE,
		s->pixels);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	*w = s->w;
	*h = s->h;

cleanup:
	SDL_FreeSurface(s);
	return nk_image_id((int)texid);
}

static void draw_tex(
	GLuint texid, const float x, const float y, const float w, const float h)
{
	glBindTexture(GL_TEXTURE_2D, texid);

	glBegin(GL_QUADS);
		glTexCoord2f(0, 1); glVertex3f(x, y, 0);
		glTexCoord2f(1, 1); glVertex3f(x + w, y, 0);
		glTexCoord2f(1, 0); glVertex3f(x + w, y + h, 0);
		glTexCoord2f(0, 0); glVertex3f(x, y + h, 0);
	glEnd();
}
