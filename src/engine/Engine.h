#pragma once

namespace engine
{
	enum class modes
	{
		debug,
		depth,
		cullFront,
		cullBack,
		cullFrontBack,
	};

	struct engine_context
	{
		double deltaTime = 0.0;
	};

	static engine_context g_engine;


	bool init();
	void terminate();

	void updateFrame();

	void enableDebugMode();
}


