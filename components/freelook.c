#include "freelook.h"
#include <systems/window.h>
#include <components/spacial.h>
#include <components/node.h>
#include <systems/mouse.h>
#include <math.h>


static void c_freelook_init(c_freelook_t *self)
{
	self->win_min_side = 1080;

	self->x_control = entity_null;
	self->y_control = entity_null;
	self->force_down = entity_null;
}

c_freelook_t *c_freelook_new(entity_t force_down, float sensitivity)
{
	c_freelook_t *self = component_new("freelook");
	self->sensitivity = sensitivity;

	self->force_down = force_down;

	return self;
}

void c_freelook_set_controls(c_freelook_t *self,
		entity_t x_control, entity_t y_control)
{
	self->x_control = x_control;
	self->y_control = y_control;
}

static int c_freelook_window_resize(c_freelook_t *self,
		window_resize_data *event)
{
	self->win_min_side = (event->width < event->height) ?
		event->width : event->height;
	return CONTINUE;
}

static void c_freelook_update(c_freelook_t *self)
{
	const float max_up = M_PI / 2.0 - 0.01;
	const float max_down = -M_PI / 2.0 + 0.01;

	if(!entity_exists(self->x_control) || !entity_exists(self->y_control))
	{
		return;
	}

	c_spacial_t *sc = c_spacial(self);

	float rot = sc->rot.x;
	if(rot > max_up) rot = max_up;
	if(rot < max_down) rot = max_down;
	c_spacial_set_rot(sc, 1, 0, 0, rot);

	/* c_spacial_update_model_matrix(c_spacial(self->x_control)); */
	/* c_spacial_update_model_matrix(c_spacial(self->y_control)); */
}

static int c_freelook_mouse_move(c_freelook_t *self, mouse_move_data *event)
{
	float frac = self->sensitivity / self->win_min_side;

	c_spacial_t *sc_x = c_spacial(&self->x_control);
	float new_rot = sc_x->rot.y - event->sx * frac;
	c_spacial_set_rot(sc_x, 0, 1, 0, new_rot);


	c_spacial_t *sc_y = c_spacial(&self->y_control);
	new_rot = sc_y->rot.x - event->sy * frac;
	c_spacial_set_rot(sc_y, 1, 0, 0, new_rot);

	c_freelook_update(self);

	return CONTINUE;
}

REG()
{
	ct_t *ct = ct_new("freelook", sizeof(c_freelook_t),
			c_freelook_init, NULL, 1, ref("spacial"));

	ct_listener(ct, WORLD, sig("mouse_move"), c_freelook_mouse_move);

	ct_listener(ct, ENTITY, sig("entity_created"), c_freelook_update);

	ct_listener(ct, WORLD, sig("window_resize"), c_freelook_window_resize);
}

