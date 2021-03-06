#ifndef TEXTURE_H
#define TEXTURE_H

#include "glutil.h"
#include "mafs.h"

typedef struct
{
	GLuint id;
	uint32_t fb_ready;
	int dims;
	uint32_t format;
	uint32_t internal;
	char *name;
	GLubyte *data;
	uint64_t handle;
	uint32_t ready;
} buffer_t;

typedef struct
{
	char name[256];
	int id;

	uint32_t width;
	uint32_t height;
	uint32_t depth;
	uint32_t depth_buffer;
	void *last_depth;
	uint32_t repeat;
	uint32_t force_handle;

	buffer_t bufs[16]; /* 0 is depth, 1 is color, >= 2 are color buffers */

	int32_t bufs_size;

	float brightness;
	uint32_t track_brightness; 
	uint32_t target;

	uint32_t frame_buffer[8];
	uvec2_t sizes[8];

	char *filename;
	int32_t framebuffer_ready;
	int32_t draw_id;
	int32_t prev_id;
	int32_t mipmaped;
	int32_t interpolate;
	int32_t loading;
} texture_t;

texture_t *texture_from_file(const char *filename);
texture_t *texture_from_memory(void *buffer, int32_t len);
int32_t texture_load(texture_t *self, const char *filename);
int32_t texture_load_from_memory(texture_t *self, void *buffer, int32_t len);
texture_t *texture_from_buffer(void *buffer, int32_t width, int32_t height,
		int32_t Bpp);
#define TEX_MIPMAP			(1 << 0x01)
#define TEX_FORCE_HANDLE	(1 << 0x02)
#define TEX_REPEAT			(1 << 0x03)
#define TEX_INTERPOLATE		(1 << 0x04)

texture_t *_texture_new_2D_pre
(
	uint32_t width,
	uint32_t height,
	uint32_t flags
);

extern __thread texture_t *_g_tex_creating;
#define texture_new_2D(w, h, f, ...) \
	(_texture_new_2D_pre(w, h, f),_texture_new(0, ##__VA_ARGS__))

texture_t *_texture_new(int32_t ignore, ...);

texture_t *texture_new_3D
(
	uint32_t width,
	uint32_t height,
	uint32_t depth,
	int32_t dims
);

texture_t *texture_cubemap
(
	uint32_t width,
	uint32_t height,
	uint32_t depth_buffer
);

int32_t texture_2D_resize(texture_t *self, int32_t width, int32_t height);

void texture_bind(texture_t *self, int32_t tex);
uint32_t texture_handle(texture_t *self, int32_t tex);

int32_t texture_target(texture_t *self, texture_t *depth, int32_t fb);
int32_t texture_target_sub(texture_t *self, int32_t width, int32_t height,
		texture_t *depth, int32_t fb);
void texture_destroy(texture_t *self);

void texture_set_xy(texture_t *self, int32_t x, int32_t y,
		GLubyte r, GLubyte g, GLubyte b, GLubyte a);
void texture_set_xyz(texture_t *self, int32_t x, int32_t y, int32_t z,
		GLubyte r, GLubyte g, GLubyte b, GLubyte a);
void texture_update_gl(texture_t *self);

void texture_update_brightness(texture_t *self);
uint32_t texture_get_pixel(texture_t *self, int32_t buffer, int32_t x, int32_t y,
		float *depth);

void texture_draw_id(texture_t *self, int32_t tex);

int32_t buffer_new(const char *name, int32_t is_float, int32_t dims);

void textures_reg(void);

#endif /* !TEXTURE_H */
