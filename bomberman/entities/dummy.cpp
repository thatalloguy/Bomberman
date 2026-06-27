#include "precomp.h"
#include "dummy.h"

void bomberman::Dummy::Initialize()
{
	AnimatedEntity::Initialize();
	position = {400, 400};
	scale = {125, 125};

	filterColor = { 0, 1, 0, 1.0};
	frameSize = { 0.1425f,0.2f };
	currentFrame = {0, 0};
}

void bomberman::Dummy::PrepareRender()
{
	AnimatedEntity::PrepareRender();

	if (animationTimer.elapsed() > 0.3)
	{
		animationTimer.reset();
		currentFrame.x += 1;
		if (currentFrame.x > 2)
		{
			currentFrame.x = 0;
			currentFrame.y += 1;
		}
		if (currentFrame.y > 3)
		{
			currentFrame.y = 0;
		}
	}

}

void bomberman::Dummy::Destroy()
{
}
