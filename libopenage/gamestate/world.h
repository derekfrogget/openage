// Copyright 2018-2023 the openage authors. See copying.md for legal info.

#pragma once

#include "util/path.h"

namespace openage {

namespace renderer {
class RenderFactory;
}

namespace gamestate {
class GameEntity;

/**
 * Entity for managing "physical" things (units, buildings) inside
 * the game.
 */
class World {
public:
	/**
	 * Create a new world.
	 *
	 * @param root_dir openage root directory.
	 */
	World(const util::Path &root_dir);
	~World() = default;

	/**
	 * Attach a renderer which enables graphical display options for all ingame entities.
	 *
	 * @param render_factory Factory for creating connector objects for gamestate->renderer
	 *                       communication.
	 */
	void attach_renderer(const std::shared_ptr<renderer::RenderFactory> &render_factory);

private:
	/**
	 * List of game entities inside the game world.
	 */
	std::vector<std::shared_ptr<GameEntity>> game_entities;

	/**
	 * Factory for creating connector objects to the renderer which make game entities displayable.
	 */
	std::shared_ptr<renderer::RenderFactory> render_factory;
};

} // namespace gamestate
} // namespace openage
