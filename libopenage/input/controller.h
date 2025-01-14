// Copyright 2021-2023 the openage authors. See copying.md for legal info.

#pragma once

#include <unordered_set>

#include "curve/discrete.h"

namespace openage::input {

/**
 * Interface for game controllers.
 *
 * Controllers handle inputs from outside of a game (e.g. GUI, AI, scripts, ...)
 * and pass the resulting events to game entities. They also act as a form of
 * access control for using in-game functionality of game entities.
 *
 * TODO: Connection to engine
 */
class Controller {
public:
	Controller(const std::unordered_set<size_t> &controlled_factions,
	           size_t active_faction_id);

	/**
     * Switch the actively controlled faction by the controller.
     * The ID must be in the list of controlled factions.
     *
     * @param faction_id ID of the new active faction.
     */
	void set_control(size_t faction_id);

	/**
     * Get the ID of the faction actively controlled by the controller.
     *
     * @return ID of the active faction.
     */
	size_t get_controlled();

private:
	/**
     * List of factions controllable by this controller.
     */
	std::unordered_set<size_t> controlled_factions;

	/**
     * ID of the currently active faction.
     */
	size_t active_faction_id;
};

} // namespace openage::input
