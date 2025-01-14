// Copyright 2022-2022 the openage authors. See copying.md for legal info.

#include "game_entity.h"

#include "renderer/stages/world/world_render_entity.h"

namespace openage::gamestate {

GameEntity::GameEntity(uint32_t id,
                       util::Vector3f pos,
                       util::Path &texture_path) :
	id{id},
	pos{pos},
	texture_path{texture_path},
	render_entity{nullptr} {
}

void GameEntity::push_to_render() {
	if (this->render_entity != nullptr) {
		this->render_entity->update(this->id,
		                            this->pos,
		                            this->texture_path);
	}
}

void GameEntity::set_render_entity(const std::shared_ptr<renderer::world::WorldRenderEntity> &entity) {
	this->render_entity = entity;

	this->push_to_render();
}

} // namespace openage::gamestate
