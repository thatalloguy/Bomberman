#include "precomp.h"
#include "menu.h"

#include "game.h"

void bomberman::Arrow::Initialize()
{
	position = {-0.4f,-0.3f};
	scale = {0.03f, 0.03f};
	texture = Assets::GetTexture(TextureType::ARROW);
	shader = Assets::GetShader(ShaderType::DEFAULT);
}

void bomberman::Arrow::PrepareRender()
{
}

void bomberman::Arrow::Tick(float)
{
}

void bomberman::Arrow::Destroy()
{
}

void bomberman::Text::Initialize()
{
	frameSize = {8, 48};
	replaceColor = {0, 0, 0, 1};
	texture = Assets::GetTexture(TextureType::TEXT);
	shader = Assets::GetShader(ShaderType::ANIMATED);

	position = { 0, 0 };
	scale = { 0.1f, -0.15f };
	// A
	currentFrame = {0, 0};

}

void bomberman::Text::PrepareRender()
{
	AnimatedEntity::PrepareRender();
}

void bomberman::Text::Tick(float deltaTime)
{
	AnimatedEntity::Tick(deltaTime);
}

void bomberman::Text::Destroy()
{
	AnimatedEntity::Destroy();
}

void bomberman::Menu::Initialize()
{
	texture = Assets::GetTexture(TextureType::MENU);
	shader =Assets::GetShader(ShaderType::DEFAULT);

	position = {0, 0};
	scale = {0.75f, 1};
}

void bomberman::Menu::PrepareRender()
{
}

void bomberman::Menu::Tick(float)
{

}

void bomberman::Menu::Destroy()
{
}
