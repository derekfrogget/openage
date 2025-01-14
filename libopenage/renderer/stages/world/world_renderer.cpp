// Copyright 2022-2023 the openage authors. See copying.md for legal info.

#include "world_renderer.h"

#include "event/clock.h"
#include "renderer/opengl/context.h"
#include "renderer/resources/assets/asset_manager.h"
#include "renderer/resources/shader_source.h"
#include "renderer/resources/texture_info.h"
#include "renderer/shader_program.h"
#include "renderer/stages/world/world_object.h"
#include "renderer/texture.h"
#include "renderer/window.h"

namespace openage::renderer::world {

WorldRenderer::WorldRenderer(const std::shared_ptr<Window> &window,
                             const std::shared_ptr<renderer::Renderer> &renderer,
                             const std::shared_ptr<renderer::camera::Camera> &camera,
                             const util::Path &shaderdir,
                             const std::shared_ptr<renderer::resources::AssetManager> &asset_manager,
                             const std::shared_ptr<event::Clock> clock) :
	renderer{renderer},
	camera{camera},
	asset_manager{asset_manager},
	render_objects{},
	clock{clock},
	default_geometry{this->renderer->add_mesh_geometry(WorldObject::get_mesh())} {
	renderer::opengl::GlContext::check_error();

	auto size = window->get_size();
	this->initialize_render_pass(size[0], size[1], shaderdir);

	window->add_resize_callback([this](size_t width, size_t height) {
		this->resize(width, height);
	});
}

std::shared_ptr<renderer::RenderPass> WorldRenderer::get_render_pass() {
	return this->render_pass;
}

void WorldRenderer::add_render_entity(const std::shared_ptr<WorldRenderEntity> entity) {
	std::unique_lock lock{this->mutex};

	auto world_object = std::make_shared<WorldObject>(this->asset_manager);
	world_object->set_render_entity(entity);
	world_object->set_camera(this->camera);
	this->render_objects.push_back(world_object);
}

void WorldRenderer::update() {
	auto current_time = this->clock->get_real_time();
	for (auto obj : this->render_objects) {
		obj->update(current_time);

		if (obj->is_changed()) {
			if (obj->requires_renderable()) {
				// TODO: Use zoom level from camera for view matrix
				Eigen::Matrix4f view_m = Eigen::Matrix4f::Identity();
				Eigen::Matrix4f proj_m = Eigen::Matrix4f::Identity();

				// TODO: Update existing renderable instead of recreating it
				auto transform_unifs = this->display_shader->new_uniform_input(
					"view",
					view_m,
					"proj",
					proj_m,
					"tex",
					obj->get_texture(),
					"u_id",
					obj->get_id());

				Renderable display_obj{
					transform_unifs,
					this->default_geometry,
					true,
					true,
				};

				this->render_pass->add_renderables(display_obj);
				obj->clear_requires_renderable();

				// update remaining uniforms for the object
				obj->set_uniforms(transform_unifs);
				obj->update_uniforms(current_time);
			}
		}
	}
}

void WorldRenderer::resize(size_t width, size_t height) {
	this->output_texture = renderer->add_texture(resources::Texture2dInfo(width, height, resources::pixel_format::rgba8));
	this->id_texture = renderer->add_texture(resources::Texture2dInfo(width, height, resources::pixel_format::r32ui));

	auto fbo = this->renderer->create_texture_target({this->output_texture, this->id_texture});
	this->render_pass->set_target(fbo);
}

void WorldRenderer::initialize_render_pass(size_t width,
                                           size_t height,
                                           const util::Path &shaderdir) {
	auto vert_shader_file = (shaderdir / "world.vert.glsl").open();
	auto vert_shader_src = renderer::resources::ShaderSource(
		resources::shader_lang_t::glsl,
		resources::shader_stage_t::vertex,
		vert_shader_file.read());
	vert_shader_file.close();

	auto frag_shader_file = (shaderdir / "world.frag.glsl").open();
	auto frag_shader_src = renderer::resources::ShaderSource(
		resources::shader_lang_t::glsl,
		resources::shader_stage_t::fragment,
		frag_shader_file.read());
	frag_shader_file.close();

	this->output_texture = renderer->add_texture(resources::Texture2dInfo(width, height, resources::pixel_format::rgba8));
	this->id_texture = renderer->add_texture(resources::Texture2dInfo(width, height, resources::pixel_format::r32ui));

	this->display_shader = this->renderer->add_shader({vert_shader_src, frag_shader_src});

	auto fbo = this->renderer->create_texture_target({this->output_texture, this->id_texture});
	this->render_pass = this->renderer->add_render_pass({}, fbo);
}

} // namespace openage::renderer::world
