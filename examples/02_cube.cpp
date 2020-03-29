#include "example_runner.hpp"
#include <glm/mat4x4.hpp>
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

namespace {
	float angle = 0.f;
	auto box = util::generate_cube();

	vuk::Example x{
		.name = "02_cube",
		.setup = [](vuk::ExampleRunner& runner, vuk::InflightContext& ifc) {
			vuk::PipelineCreateInfo pci;
			pci.shaders.push_back("../../examples/ubo_test.vert");
			pci.shaders.push_back("../../examples/triangle_depthshaded.frag");
			runner.context->create_named_pipeline("cube", pci);
		},
		.render = [](vuk::ExampleRunner& runner, vuk::InflightContext& ifc) {
			auto ptc = ifc.begin();
		
			auto [bverts, stub1] = ptc.create_scratch_buffer(vuk::MemoryUsage::eGPUonly, vk::BufferUsageFlagBits::eVertexBuffer, gsl::span(&box.first[0], box.first.size()));
			auto verts = std::move(bverts);
			auto [binds, stub2] = ptc.create_scratch_buffer(vuk::MemoryUsage::eGPUonly, vk::BufferUsageFlagBits::eIndexBuffer, gsl::span(&box.second[0], box.second.size()));
			auto inds = std::move(binds);
			struct VP {
				glm::mat4 view;
				glm::mat4 proj;
			} vp;
			vp.view = glm::lookAt(glm::vec3(0, 1.5, 3.5), glm::vec3(0), glm::vec3(0, 1, 0));
			vp.proj = glm::perspective(glm::degrees(70.f), 1.f, 1.f, 10.f);

			auto [buboVP, stub3] = ptc.create_scratch_buffer(vuk::MemoryUsage::eCPUtoGPU, vk::BufferUsageFlagBits::eUniformBuffer, gsl::span(&vp, 1));
			auto uboVP = buboVP;
			ptc.wait_all_transfers();

			vuk::RenderGraph rg;
			rg.add_pass({
				.resources = {"02_cube_final"_image(vuk::eColorWrite)},
				.execute = [verts, uboVP, inds](vuk::CommandBuffer& command_buffer) {
					command_buffer
					  .set_viewport(0, vuk::Area::Framebuffer{})
					  .set_scissor(0, vuk::Area::Framebuffer{})
					  .bind_vertex_buffer(0, verts, 0, vuk::Packed{vk::Format::eR32G32B32Sfloat, vuk::Ignore{sizeof(util::Vertex) - sizeof(util::Vertex::position)}})
					  .bind_index_buffer(inds, vk::IndexType::eUint32)
					  .bind_pipeline("cube")
					  .bind_uniform_buffer(0, 0, uboVP);
					glm::mat4* model = command_buffer.map_scratch_uniform_binding<glm::mat4>(0, 1);
					*model = static_cast<glm::mat4>(glm::angleAxis(glm::radians(angle), glm::vec3(0.f, 1.f, 0.f)));
					command_buffer
					  .draw_indexed(box.second.size(), 1, 0, 0, 0);
					}
				}
			);

			angle += 360.f * ImGui::GetIO().DeltaTime;

			return rg;
		}
	};

	REGISTER_EXAMPLE(x);
}