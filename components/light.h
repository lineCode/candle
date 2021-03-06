#ifndef LIGHT_H
#define LIGHT_H

#include <utils/glutil.h>
#include <ecs/ecm.h>
#include <utils/texture.h>
#include <utils/material.h>
#include <utils/drawable.h>
#include <utils/renderer.h>

typedef struct
{
	c_t super;

	vec4_t color;
	uint32_t shadow_size;
	float radius;

	uint32_t id;
	drawable_t draw;
	drawable_t widget;
	renderer_t *renderer;
	uint32_t visible;
	uint32_t ambient_group;
	uint32_t light_group;
	uint32_t modified;
	uint32_t visible_group;
	uint32_t shadow_cooldown;
	int32_t frames_passed;
} c_light_t;

DEF_CASTER("light", c_light, c_light_t)

c_light_t *c_light_new(float radius, vec4_t color, uint32_t shadow_size);
void c_light_visible(c_light_t *self, uint32_t visible);
void c_light_set_groups(c_light_t *self, uint32_t visible_group,
		uint32_t ambient_group, uint32_t light_group);
void c_light_set_shadow_cooldown(c_light_t *self, uint32_t cooldown);

#endif /* !LIGHT_H */
