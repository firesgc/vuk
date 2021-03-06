#include "Pool.hpp"
#include "Context.hpp"

namespace vuk {
	// pools
	template<>
	std::span<vk::Semaphore> PooledType<vk::Semaphore>::acquire(PerThreadContext& ptc, size_t count) {
		if (values.size() < (needle + count)) {
			auto remaining = values.size() - needle;
			for (auto i = 0; i < (count - remaining); i++) {
				auto nalloc = ptc.ctx.device.createSemaphore({});
				values.push_back(nalloc);
			}
		}
		std::span<vk::Semaphore> ret{ &*values.begin() + needle, count };
		needle += count;
		return ret;
	}

	template<>
	std::span<vk::Fence> PooledType<vk::Fence>::acquire(PerThreadContext& ptc, size_t count) {
		if (values.size() < (needle + count)) {
			auto remaining = values.size() - needle;
			for (auto i = 0; i < (count - remaining); i++) {
				auto nalloc = ptc.ctx.device.createFence({});
				values.push_back(nalloc);
			}
		}
		std::span<vk::Fence> ret{ &*values.begin() + needle, count };
		needle += count;
		return ret;
	}

	template<class T>
	void PooledType<T>::free(Context& ctx) {
		for (auto& v : values) {
			ctx.device.destroy(v);
		}
	}

	template struct PooledType<vk::Semaphore>;
	template struct PooledType<vk::Fence>;

	template<>
	void PooledType<vk::Fence>::reset(Context& ctx) {
		if (needle > 0) {
			ctx.device.waitForFences((uint32_t)needle, values.data(), true, UINT64_MAX);
			ctx.device.resetFences((uint32_t)needle, values.data());
		}
		needle = 0;
	}

	// vk::CommandBuffer pool
	PooledType<vk::CommandBuffer>::PooledType(Context& ctx) {
		pool = ctx.device.createCommandPoolUnique({});
	}

	std::span<vk::CommandBuffer> PooledType<vk::CommandBuffer>::acquire(PerThreadContext& ptc, vk::CommandBufferLevel level, size_t count) {
        auto& values = level == vk::CommandBufferLevel::ePrimary ? p_values : s_values;
        auto& needle = level == vk::CommandBufferLevel::ePrimary ? p_needle : s_needle;
		if (values.size() < (needle + count)) {
			auto remaining = values.size() - needle;
			vk::CommandBufferAllocateInfo cbai;
			cbai.commandBufferCount = (unsigned)(count - remaining);
			cbai.commandPool = *pool;
			cbai.level = level;
			auto nalloc = ptc.ctx.device.allocateCommandBuffers(cbai);
			values.insert(values.end(), nalloc.begin(), nalloc.end());
		}
		std::span<vk::CommandBuffer> ret{ &*values.begin() + needle, count };
		needle += count;
		return ret;
	}
	void PooledType<vk::CommandBuffer>::reset(Context& ctx) {
		vk::CommandPoolResetFlags flags = {};
		ctx.device.resetCommandPool(*pool, flags);
		p_needle = s_needle = 0;
	}

	void PooledType<vk::CommandBuffer>::free(Context& ctx) {
		ctx.device.freeCommandBuffers(*pool, s_values);
		ctx.device.freeCommandBuffers(*pool, p_values);
		pool.reset();
	}

}
