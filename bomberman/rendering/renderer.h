#pragma once

#include "../core/entity.h"
#include "../misc/utils.h"
#include "template/opengl.h"
#include "template/tmpl8math.h"


#ifndef MAP_SIZE
#define MAP_SIZE 1
#define MAP_WIDTH 27
#define MAP_HEIGHT 13
#endif

namespace
{
	void translate(mat4& out, float2 vec) {
		// translate the x dimension
		float4 m1 = { out[0] * vec[0], out[1] * vec[0], out[1] * vec[0], out[3] * vec[0] };
		// translate the y dimension
		float4 m2 = { out[4] * vec[0], out[5] * vec[1], out[6] * vec[1], out[7] * vec[1] };
		//Keep the Z dimension the same since its a 2d game.
		float4 m3 = { out[12], out[13], out[14], out[15] };

		//Calc the result
		float4 result = m1 + m2 + m3;

		//Apply the result to our internal data.
		out[12] = result[0];
		out[13] = result[1];
		out[14] = result[2];
		out[15] = result[3];
	}

	// Scales the Matrix according to the Vector2
	void scale(mat4& out, float2 vec) {

		//Scale the X dimension
		float4 m1 = { out[0] * vec[0], out[1] * vec[0], out[1] * vec[0], out[3] * vec[0] };
		//Scale the Y dimension
		float4 m2 = { out[4] * vec[0], out[5] * vec[1], out[6] * vec[1], out[7] * vec[1] };

		//Apply our results to the internal buffer.
		out[0] = m1[0];
		out[1] = m1[1];
		out[2] = m1[2];
		out[3] = m1[3];

		out[4] = m2[0];
		out[5] = m2[1];
		out[6] = m2[2];
		out[7] = m2[3];

	}

	float dist(const float2& a, const float2& b)
	{
		return sqrt(pow(a.x - b.x, 2.0f) + pow(a.y - b.y, 2.0f));
	}

	void ortho(mat4& out, float left, float right, float bottom, float top, float zNear, float zFar)
	{
		out[0] = 2 / (right - left);
		out[5] = 2 / (top - bottom);
		out[10] = -2 / (zFar - zNear);

		out[12] = -(right + left) / (right - left);
		out[13] = -(top + bottom) / (top - bottom);
		out[14] = (zFar + zNear) / (zFar - zNear);
	}
}

namespace bomberman {

	struct DepthBuffer
	{
		unsigned int fbo = 0;
		unsigned int tex = 0;
	};
	 // So for the renderer we need to write it to a finall buffer.
	 // But the question is how?
	 //
	 
	// I like to use structs to store the arguments for a function/constructor that has a ton of arguments.
	// Inspired by C libs like Steamaudio and Vulkan
	struct RendererInfo {
		//the res of our output image.
		int2 screenSize = { 1280, 720 };
	};

	struct RenderInfo
	{
		GLTexture* texture{nullptr};
		Shader* shader{ nullptr };

	};

	struct RawTile
	{
		mat4 model{};
		float2 index{};
	};

	//Very dirty and bad way to do it.
	// But i am only human after all ;)
	inline struct
	{
		int4 bound{ 0, 0, 0, 0 };
		Entity* entityA{ nullptr };
		Entity* entityB{ nullptr };
		int w{0};
		int h{0};
		// i, j, k, l
	} collisionInfo;

	class Camera
	{
	public:
		Camera() = default;
		~Camera() = default;

		void Update();
		 mat4& GetMatrix() { return this->mat;  };

		void SetPosition(const float2& pos) { position = pos; }
		void SetScreenSize(const float2& size) { screenSize = size; }

		float2 GetPosition() const { return position; }
		float2 GetScreenSize() const { return screenSize; }


	protected:
		mat4 mat{};
		float2 position{0, 0};
		float2 screenSize{240, 480};
	};

	
	// Should this be a class or an namespace?
	class Renderer {
	public:
		Renderer(const RendererInfo& info);
		~Renderer();

		void BindCamera(Camera* cam) { camera = cam; };
		Camera* GetActiveCamera() const { return camera; };


		void AddTileToBatch(Tile* entity);
		void clearBatch();

		void PrepareBatch();

		void RenderBatch();

		void RenderEntity(Entity* entity); // Just a normal draw call

		//Pixel perfect collision detection phase
		void SetupPixelPerfectCollision(Entity& entityA, Entity& entityB, const float4& a, const float4& b);

	private:
		Shader depthShader{ "assets/shaders/animated.vert", "assets/shaders/depth.frag", false };
		Camera* camera{nullptr};
		utils::vector<Shader*> shaders{};
		utils::vector<GLTexture*> textures{};

		//utils::vector <Tile*> tileBatch{};
		//utils::vector<RawTile> finalBatch{};

		Tile** tileBatch = nullptr;
		int tileBatchSize = 0;
		int maxTileBatchSize = MAP_WIDTH * MAP_HEIGHT;

		RawTile* finalBatch{ nullptr };
		int finalBatchSize = 0;

		uint matricesBuffer = 0;
	};
}