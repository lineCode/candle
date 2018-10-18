#include "camera.h"
#include <systems/window.h>
#include <systems/editmode.h>
#include "spacial.h"
#include "node.h"
#include <utils/nk.h>
#include <candle.h>
#include <systems/renderer.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int g_camera_num;

c_camera_t *c_camera_new(float fov, float near, float far)
{
	c_camera_t *self = component_new("camera");

	self->near = near;
	self->far = far;
	self->fov = fov;
	self->view_cached = 0;
	self->exposure = 0.25f;
	self->id = g_camera_num++;

	c_camera_update(self, NULL);

	return self;
}

c_camera_t *c_camera_clone(c_camera_t *self)
{
	c_camera_t *clone = malloc(sizeof *clone);

	clone->near = self->near;
	clone->far = self->far;
	clone->fov = self->fov;
	clone->view_cached = 0;
	clone->exposure = self->exposure;

	c_camera_update(self, NULL);

	return clone;
}

int c_camera_changed(c_camera_t *self)
{
	self->view_cached = 0;
	return CONTINUE;
}

vec3_t c_camera_screen_pos(c_camera_t *self, vec3_t pos)
{
	mat4_t VP = mat4_mul(self->projection_matrix, self->view_matrix); 

	vec4_t viewSpacePosition = mat4_mul_vec4(VP, vec4(_vec3(pos), 1.0f));
	viewSpacePosition = vec4_div_number(viewSpacePosition, viewSpacePosition.w);

	viewSpacePosition.xyz = vec3_scale(vec3_add_number(viewSpacePosition.xyz, 1.0f), 0.5);
    return viewSpacePosition.xyz;
}

/* float c_camera_linearize(c_camera_t *self, float depth) */
/* { */
    /* return = 2.0 * zNear * zFar / (zFar + zNear - (2.0 * z_b - 1.0) * (zFar - zNear)); */
/* } */

float c_camera_unlinearize(c_camera_t *self, float depth)
{
	return self->far * (1.0f - (self->near / depth)) / (self->far - self->near);
}

vec3_t c_camera_real_pos(c_camera_t *self, float depth, vec2_t coord)
{
	/* float z = depth; */
	if(depth < 0.01f) depth *= 100.0f;
    float z = depth * 2.0 - 1.0;
	coord = vec2_sub_number(vec2_scale(coord, 2.0f), 1.0);

	mat4_t projInv = mat4_invert(self->projection_matrix);
	mat4_t viewInv = mat4_invert(self->view_matrix);

    vec4_t clipSpacePosition = vec4(_vec2(coord), z, 1.0);
    vec4_t viewSpacePosition = mat4_mul_vec4(projInv, clipSpacePosition);

    // Perspective division
    viewSpacePosition = vec4_div_number(viewSpacePosition, viewSpacePosition.w);

    vec4_t worldSpacePosition = mat4_mul_vec4(viewInv, viewSpacePosition);

    return worldSpacePosition.xyz;
}


void c_camera_update_view(c_camera_t *self)
{
	if(self->view_cached) return;
	c_node_t *n = c_node(self);
	c_node_update_model(n);
	self->model_matrix = n->model;

	self->pos = mat4_mul_vec4(n->model, vec4(0.0, 0.0, 0.0, 1.0)).xyz;
	self->view_matrix = mat4_invert(n->model);
	self->view_cached = 1;
}

void c_camera_activate(c_camera_t *self)
{
	c_camera_update(self, NULL);
}

int c_camera_component_menu(c_camera_t *self, void *ctx)
{
	float before = self->fov;
	nk_property_float(ctx, "fov:", 1, &self->fov, 179, 1, 1);
	if(self->fov != before)
	{
		c_camera_update(self, NULL);
	}
	before = self->near;
	nk_property_float(ctx, "near:", 0.01, &self->near, self->far - 0.01, 0.01, 0.1);
	if(self->near != before)
	{
		c_camera_update(self, NULL);
	}
	before = self->far;
	nk_property_float(ctx, "far:", self->near + 0.01, &self->far, 10000, 0.01, 0.1);
	if(self->far != before)
	{
		c_camera_update(self, NULL);
	}
	return CONTINUE;
}

int c_camera_update(c_camera_t *self, void *event)
{
	/* TODO: remove renderer reference, camera should update on render resize,
	 * not window */
	self->width = c_window(&SYS)->width;
	self->height = c_window(&SYS)->height;
	
	self->projection_matrix = mat4_perspective(
		self->fov * (M_PI / 180.0f),
		((float)self->width) / self->height,
		/* ((float)event->width / 2) / event->height, */
		self->near, self->far
	);
	self->view_cached = 0;
	return CONTINUE;
}

REG()
{
	ct_t *ct = ct_new("camera", sizeof(c_camera_t), NULL,
			NULL, 1, ref("node"));

	ct_listener(ct, ENTITY, sig("node_changed"), c_camera_changed);
	ct_listener(ct, WORLD, sig("window_resize"), c_camera_update);

	ct_listener(ct, WORLD, sig("component_menu"), c_camera_component_menu);
}
