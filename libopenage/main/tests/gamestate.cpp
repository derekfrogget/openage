// Copyright 2019-2019 the openage authors. See copying.md for legal info.

#include "gamestate.h"

#include <sstream>

#include "../../util/strings.h"


namespace openage::main::tests::pong {


using namespace std::placeholders;
PongPlayer::PongPlayer(const std::shared_ptr<event::Loop> &mgr, size_t id)
	:
	EventTarget{mgr},
	speed(std::make_shared<curve::Discrete<float>>(
		      mgr,
		      (id << 4) + 1,
		      util::sformat("PongPlayer(%zu).speed", id),
		      std::bind(&PongPlayer::child_changes, this, _1))),
	position(std::make_shared<curve::Continuous<float>>(
		         mgr,
		         (id << 4) + 2,
		         util::sformat("PongPlayer(%zu).position", id),
		         std::bind(&PongPlayer::child_changes, this, _1))),
	lives(std::make_shared<curve::Discrete<int>>(
		      mgr,
		      (id << 4) + 3,
		      util::sformat("PongPlayer(%zu).lives", id),
		      std::bind(&PongPlayer::child_changes, this, _1))),
	state(std::make_shared<curve::Discrete<PongEvent>>(
		      mgr,
		      (id << 4) + 4,
		      util::sformat("PongPlayer(%zu).state", id),
		      std::bind(&PongPlayer::child_changes, this, _1))),
	size(std::make_shared<curve::Discrete<float>>(
		     mgr,
		     (id << 4) + 5,
		     util::sformat("PongPlayer(%zu).size", id),
		     std::bind(&PongPlayer::child_changes, this, _1))),
	_id{id},
	paddle_x{0} {}


size_t PongPlayer::id() const {
	return _id;
}

std::string PongPlayer::idstr() const {
	std::stringstream ss;
	ss << "PongPlayer(" << this->id() << ")";
	return ss.str();
}


void PongPlayer::child_changes(const curve::time_t &time) {
	this->changes(time);
}


PongBall::PongBall(const std::shared_ptr<event::Loop> &mgr, size_t id)
	:
	EventTarget{mgr},
	speed(std::make_shared<curve::Discrete<util::Vector2d>>(
		      mgr,
		      (id << 2) + 1,
		      util::sformat("PongBall(%zu).speed", id),
		      std::bind(&PongBall::child_changes, this, _1))),
	position(std::make_shared<curve::Continuous<util::Vector2d>>(
		         mgr,
		         (id << 2) + 2,
		         util::sformat("PongBall(%zu).position", id),
		         std::bind(&PongBall::child_changes, this, _1))),
	_id{id} {}


size_t PongBall::id() const {
	return _id;
}


std::string PongBall::idstr() const {
	std::stringstream ss;
	ss << "PongBall(" << this->id() << ")";
	return ss.str();
}


void PongBall::child_changes(const curve::time_t &time) {
	this->changes(time);
}


PongState::PongState(const std::shared_ptr<event::Loop> &mgr,
                     const std::shared_ptr<Gui> &gui)
	:
	State{mgr},
	p1(std::make_shared<PongPlayer>(mgr, 0)),
	p2(std::make_shared<PongPlayer>(mgr, 1)),
	ball(std::make_shared<PongBall>(mgr, 2)),
	gui{gui} {}

} // openage::main::tests::pong
