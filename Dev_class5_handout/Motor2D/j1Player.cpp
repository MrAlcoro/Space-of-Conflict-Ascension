#include "p2Defs.h"
#include "p2Log.h"
#include "j1Player.h"
#include "j1App.h"
#include "j1Textures.h"
#include "j1Render.h"
#include "j1Input.h"
#include "j1Scene.h"
#include "j1Collisions.h"
#include "j1Map.h"


j1Player::j1Player() : j1Module()
{
	name.create("player");	

	player_rect = { 0, 0, 16, 27 };

	direction = 1; // 1 - right, -1 - left

	jumping = false;
	current_jump_distance = 0;
}

// Destructor ---------------------------------
j1Player::~j1Player()
{
}

// Called before render is available ----------
bool j1Player::Awake(pugi::xml_node& config)
{
	LOG("Init player");
	bool ret = true;
	current_map = 1;

	position.SetToZero();
	LOG(" INITIAL Position = (%i, %i)", position.x, position.y);

	speed = config.child("speed").attribute("value").as_int();
	gravity = config.child("gravity").attribute("value").as_int();
	jump_force = config.child("jump_force").attribute("value").as_int();
	jump_distance = config.child("jump_distance").attribute("value").as_int();

	return ret;
}

// Update ------------------------------------
bool j1Player::Update(float dt)
{
	bool ret = true;

	player_rect.x = position.x;
	player_rect.y = position.y;

	// Player Controls
	if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
	{
		player_rect.x -= 1 * speed;
		if (CheckCollisions() == false)
		{
			position.x = player_rect.x;
		}
		else
		{
			player_rect.x = position.x;
		}
		direction = -1;
	}

	if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
	{
		player_rect.x += 1 * speed;
		if (CheckCollisions() == false)
		{
			position.x = player_rect.x;
		}
		else
		{
			player_rect.x = position.x;
		}
		direction = 1;
	}

	if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN)
	{
		jumping = true;
	}

	if (jumping)
	{
		player_rect.y -= jump_force;
		if (CheckCollisions() == false)
		{
			position.y = player_rect.y;
		}
		else
		{
			player_rect.y = position.y;
		}
		if (jump_distance <= current_jump_distance)
		{
			current_jump_distance = 0.f;
			jumping = false;
		}
		else
		{
			current_jump_distance += jump_force;
		}
	}
	else
	{
		player_rect.y += gravity;
		if (CheckCollisions() == false)
		{
			position.y = player_rect.y;
		}
		else
		{
			player_rect.y = position.y;
		}
	}

	if (CheckDeath() == true)
	{
		position = App->map->current_spawn_point;
	}

	return ret;
}

// Set texture
void j1Player::SetTexture(SDL_Texture* texture)
{
	this->texture = texture;

	int w, h;
	SDL_QueryTexture(texture, NULL, NULL, &w, &h);

	player_rect.h = 16;
	player_rect.w = 27;
}

// Called before quitting --------------------
bool j1Player::CleanUp()
{
	LOG("Destroying player");

	return true;
}

// Collisions
bool j1Player::CheckCollisions()
{
	bool ret = false;
	p2List_item<SDL_Rect>* item = App->collision->no_walkable_tiles.start;

	for (item; item != App->collision->no_walkable_tiles.end; item = item->next)
	{
		ret = App->collision->CheckCollision(player_rect, item->data);
		if (ret)
			return ret;
	}

	return ret;
}

// Death
bool j1Player::CheckDeath()
{
	bool ret = false;
	p2List_item<SDL_Rect>* item = App->collision->death_triggers.start;

	for (item; item != App->collision->death_triggers.end; item = item->next)
	{
		ret = App->collision->CheckCollision(player_rect, item->data);
		if (ret)
			return ret;
	}

	return ret;
}

// Victory
bool j1Player::CheckWin()
{
	bool ret = false;
	p2List_item<SDL_Rect>* item = App->collision->win_triggers.start;

	for (item; item != App->collision->win_triggers.end; item = item->next)
	{
		ret = App->collision->CheckCollision(player_rect, item->data);
		if (ret)
			return ret;
	}

	return ret;
}

// Save & Load ------------------------------
bool j1Player::Save(pugi::xml_node& node)
{
	bool ret = true;

	SavePlayerState(node);

	return true;
}

bool j1Player::Load(pugi::xml_node& node)
{
	bool ret = true;

	LoadState(node);

	return true;
}

bool j1Player::LoadState(pugi::xml_node& node)
{
	bool ret = true;

	position.x = node.child("position").attribute("positionx").as_int();
	position.y = node.child("position").attribute("positiony").as_int();

	return true;
}

bool j1Player::SavePlayerState(pugi::xml_node& node)
{
	bool ret = true;

	pugi::xml_node player_node = node.append_child("position");

	player_node.append_attribute("positionx").set_value(position.x);
	player_node.append_attribute("positiony").set_value(position.y);

	return ret;
}