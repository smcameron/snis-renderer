#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <SDL2/SDL.h>

#define SNIS_TYPEFACE_DECLARE_GLOBALS
#include <snis-renderer.h> 
#undef SNIS_TYPEFACE_DECLARE_GLOBALS

static int real_screen_width, real_screen_height;
static int time_to_quit = 0;
static struct mesh *unitcube;
static struct entity_context *ecx;
static struct entity *box;
union quat box_orientation = IDENTITY_QUAT_INITIALIZER; 

static struct material box_material;

static int start_sdl(void)
{
	SDL_SetHint(SDL_HINT_NO_SIGNAL_HANDLERS, "1"); /* Suppress default SDL2 SIGTERM and SIGINT signal handlers */
	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		fprintf(stderr, "Unable to initialize SDL (Video):  %s\n", SDL_GetError());
		return 1;
	}
	if (SDL_Init(SDL_INIT_EVENTS) != 0) {
		fprintf(stderr, "Unable to initialize SDL (Events):  %s\n", SDL_GetError());
		return 1;
	}
	atexit(SDL_Quit);
	return 0;
}

static void set_sdl_attributes(void)
{
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 5);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 5);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 5);
	/* SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16); */
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 0);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 0);

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
}

static unsigned int load_texture(char *filename, int linear_colorspace)
{
	char fname[PATH_MAX + 1];

	snprintf(fname, sizeof(fname), "%s/%s", ".", filename);
	return graph_dev_load_texture(fname, linear_colorspace);
}

static void load_texture_mapped_material(struct material *m, char *diffuse_file, char *emit_file)
{
	material_init_texture_mapped(m);
	m->texture_mapped.texture_id = load_texture(diffuse_file, 0);
	if (emit_file)
		m->texture_mapped.emit_texture_id = load_texture(emit_file, 0);
}

static void load_textures(void)
{
	static int loaded = 0;

	if (loaded)
		return;

	load_texture_mapped_material(&box_material, "textures/box-diffuse.png", NULL);
	loaded = 1;
}

static void init_meshes(void)
{
	unitcube = mesh_unit_cube(20);
	mesh_unit_cube_uv_map(unitcube);
	mesh_graph_dev_init(unitcube);
}

static int configure_window(SDL_Window *window)
{
	SDL_GL_GetDrawableSize(window, &real_screen_width, &real_screen_height);
	sng_set_screen_size(real_screen_width, real_screen_height);

	static int gl_is_setup = 0;
	if (!gl_is_setup) {
		graph_dev_setup("./shaders");
		gl_is_setup = 1;
	}

	load_textures();

	static int meshes_loaded = 0;
	if (!meshes_loaded) {
		init_meshes();
		meshes_loaded = 1;
	}
	return 1;
}

static int key_press_cb(SDL_Window *window, SDL_Keysym *keysym, int key_repeat)
{
	switch (keysym->sym) {
		case SDLK_ESCAPE:
			time_to_quit = 1;
			break;
		default:
			break;
	}
}

static int key_release_cb(SDL_Keysym *keysym)
{
}

static void handle_window_event(SDL_Window *window, SDL_Event event)
{
	int width, height;

	switch (event.window.event) {
	case SDL_WINDOWEVENT_RESIZED:
	case SDL_WINDOWEVENT_SIZE_CHANGED:
		break;
	default:
		break;
	}
}

static void process_events(SDL_Window *window)
{
	SDL_Event event;

	/* Grab all the events off the queue. */
	while (SDL_PollEvent(&event)) {
		switch (event.type) {
		case SDL_KEYDOWN:
			key_press_cb(window, &event.key.keysym, event.key.repeat);
			break;
		case SDL_KEYUP:
			key_release_cb(&event.key.keysym);
			break;
		case SDL_QUIT:
			/* By default SDL2 installs signal handlers for SIGINT and SIGTERM and converts
			 * both of them into SDL_QUIT events.  We suppress these default signal handlers
			 * and install our own, so we should not get here except by e.g. clicking the 'X'
			 * on the window border.
			 */
			time_to_quit = 1;
			break;
		case SDL_WINDOWEVENT:
			handle_window_event(window, event);
			break;
		case SDL_MOUSEBUTTONDOWN:
			/* handle_button_press(&event.button); */
			break;
		case SDL_MOUSEBUTTONUP:
			/* handle_button_release(&event.button); */
			break;
		case SDL_MOUSEMOTION:
			/* handle_motion_notify(window, &event.motion); */
			break;
		case SDL_MOUSEWHEEL:
			/* handle_mouse_scroll(&event.wheel); */
			break;
		}
	}
}

static void move_objects(void)
{
	static int timer = 0;
	union quat orientation;

	quat_init_axis(&orientation, 1, 1, 1, (timer % 360) * M_PI / 180.0);
	quat_normalize_self(&orientation);
	update_entity_orientation(box, &orientation);
	timer++;
	if (timer == 360)
		timer = 0;
}

static void draw_window(SDL_Window *window)
{
	graph_dev_start_frame();
	render_entities(ecx);
	graph_dev_end_frame();
	glFinish();
	SDL_GL_SwapWindow(window);
}

int main(__attribute__((unused)) int argc, __attribute__((unused))  char *argv[])
{
	struct snis_object_pool *pool;
	static int SCREEN_WIDTH = 800;
	static int SCREEN_HEIGHT = 600;

	snis_object_pool_setup(&pool, 100);
	if (!pool) {
		fprintf(stderr, "Failed to allocated object pool, exiting.\n");
		exit(1);
	}

	ecx = entity_context_new(100, 100);
	if (start_sdl()) {
		fprintf(stderr, "Failed to start SDL, exiting\n");
		exit(1);
	}

	set_sdl_attributes();

	SDL_Window *window = SDL_CreateWindow("snis-renderer Basic Example Program",
		SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		0, 0, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN);
	if (!window) {
		fprintf(stderr, "Could not create window: %s\n", SDL_GetError());
		exit(1);
	}
	SDL_SetWindowSize(window, SCREEN_WIDTH, SCREEN_HEIGHT);
	sng_set_clip_window(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	SDL_GLContext gl_context = SDL_GL_CreateContext(window);
	(void) gl_context;

	configure_window(window);
	SDL_ShowWindow(window);
	graph_dev_clear_window();
	SDL_GL_SwapWindow(window);

	double currentTime = time_now_double();
	double nextTime = currentTime + 1.0 / 60.0;

	const float angle_of_view = 30.0 * M_PI / 180.0;
	const float near = 1.0;
	const float far = 5000.0;
	union vec3 camera_up = { {0, 1, 0}, };
	union vec3 camera_pos = { { -4.0, 4.0, 0 }, };
	union vec3 camera_lookat = { { 0, 0, 0 } };

	camera_assign_up_direction(ecx, camera_up.v.x, camera_up.v.y, camera_up.v.z);	
	camera_set_pos(ecx, camera_pos.v.x, camera_pos.v.y, camera_pos.v.z);
	camera_look_at(ecx, camera_lookat.v.x, camera_lookat.v.y, camera_lookat.v.z);
	set_renderer(ecx, FLATSHADING_RENDERER);
	camera_set_parameters(ecx, near, far, SCREEN_WIDTH, SCREEN_HEIGHT, angle_of_view); 
	calculate_camera_transform(ecx);
	entity_context_set_hi_lo_poly_pixel_threshold(ecx, 200.0);

	set_lighting(ecx, 20.0, -20.0, -20.0);
	set_ambient_light(ecx, 0.05);

	box = add_entity(ecx, unitcube, 0, 0, 0, WHITE);
	update_entity_material(box, &box_material);
	update_entity_orientation(box, &box_orientation);

	while (1) {
		currentTime = time_now_double();
		if (currentTime >= nextTime) {
			process_events(window);
			move_objects();
			draw_window(window);
			currentTime = time_now_double();
			nextTime = currentTime + 1.0 / 60.0;	
		} else {
			double timeToSleep = nextTime - currentTime;
			if (timeToSleep > 0)
				sleep_double(timeToSleep);
		}
		if (time_to_quit) {
			/* probably should do some clean up */
			break;	
		}
	}
	entity_context_free(ecx);
	return 0;
}
