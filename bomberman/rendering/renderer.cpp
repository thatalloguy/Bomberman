#include "precomp.h"
#include "renderer.h"
#include "../core/assetManager.h"
#include <stb_image.h>


//----- Camera 

void bomberman::Camera::Update()
{
	ortho(mat, 0, screenSize.x, screenSize.y, 0, -1.0f, 1.0f);
	translate(mat, position);
}






//------ Renderer
bomberman::Renderer::~Renderer()
{
    printf("deleted\n");
    delete[] finalBatch;
    finalBatchSize = 0;
}

void bomberman::Renderer::AddTileToBatch(Tile* entity)
{
    if (!tileBatch)
    {
        tileBatch = new Tile*[maxTileBatchSize];
    }

    tileBatch[tileBatchSize] = entity;
    tileBatchSize++;
}

void bomberman::Renderer::clearBatch()
{
    delete[] tileBatch;
    tileBatch = nullptr;
    tileBatchSize = 0;

    delete[] finalBatch;
    finalBatch = nullptr;
    finalBatchSize = 0;
}

void bomberman::Renderer::PrepareBatch()
{
    glDeleteBuffers(1, &matricesBuffer);

    delete[] finalBatch;
    finalBatch = new  RawTile[tileBatchSize];
    finalBatchSize = tileBatchSize;

    for (int i = 0; i < tileBatchSize; i++) {
       
        if (Tile* tile = tileBatch[i])
        {
            mat4 mat{};


            translate(mat, tile->GetPosition());
            scale(mat, tile->GetScale());

            float2 index = tile->GetIndex();

            if (tile->GetType() == BREAKABLE)
            {
	           index = {6, 0}; 
            }

            finalBatch[i] = RawTile{ mat, index};
	        
        }
    	
    }

    // Instance buffer for the matrices
    
    glGenBuffers(1, &matricesBuffer);

    DrawQuad(true);

    glBindBuffer(GL_ARRAY_BUFFER, matricesBuffer);
    glBufferData(GL_ARRAY_BUFFER, finalBatchSize * sizeof(RawTile), finalBatch, GL_STREAM_DRAW);

    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(RawTile), (void*)0);
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(RawTile), (void*)(sizeof(float4)));
    glEnableVertexAttribArray(5);
    glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(RawTile), (void*)(2 * sizeof(float4)));
    glEnableVertexAttribArray(6);
    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(RawTile), (void*)(3 * sizeof(float4)));
    glEnableVertexAttribArray(7);
    glVertexAttribPointer(7, 2, GL_FLOAT, GL_FALSE, sizeof(RawTile), (void*)(4 * sizeof(float4)));

    glVertexAttribDivisor(3, 1);
    glVertexAttribDivisor(4, 1);
    glVertexAttribDivisor(5, 1);
    glVertexAttribDivisor(6, 1);
    glVertexAttribDivisor(7, 1);


    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void bomberman::Renderer::RenderBatch()
{
    //We are dependent on the first entity for its shader
	if (tileBatch == nullptr)
    {
        return;
    } if (tileBatch[tileBatchSize-1]->GetShader() == nullptr) return;
    Shader* shader = tileBatch[tileBatchSize-1]->GetShader();

    shader->Bind();
    shader->SetInputTexture(1, "tex", tileBatch[tileBatchSize-1]->GetTexture());
    shader->SetFloat2("grid", { 16, 384 });
    shader->SetInputMatrix("proj", camera->GetMatrix());


    DrawQuad(true);

    glDrawArraysInstanced(GL_TRIANGLES, 0, 6, finalBatchSize);
    glBindVertexArray(0);

}


bomberman::Renderer::Renderer(const RendererInfo&)
{
}


void bomberman::Renderer::RenderEntity(Entity* entity) 
{
	if (entity == nullptr)
	{
		printf("[Warning][Renderer] Trying to render an invalid entity\n");
		return;
	} else if (entity->GetShader() == nullptr || entity->GetTexture() == nullptr || camera == nullptr)
	{
        printf("[Warning][Renderer] Trying to render an invalid shader/texture\n");
		return;
	}

	mat4 matrix{};


	translate(matrix, entity->GetPosition());
	scale(matrix, entity->GetScale());

	Shader* shader = entity->GetShader();

    if (entity->GetShader()->GetID() > 50)
    {
        return;
    }

	shader->Bind();

	entity->PrepareRender();


	shader->SetInputTexture(1, "tex", entity->GetTexture());
	shader->SetInputMatrix("proj", camera->GetMatrix());


	shader->SetInputMatrix("matrix", matrix);
	DrawQuad();


	//shader->Unbind();
}

void bomberman::Renderer::SetupPixelPerfectCollision(Entity& entityA, Entity& entityB, const float4& a, const float4& b)
{
    int i = static_cast<int>(min(a.x, b.x));
    int j = static_cast<int>(min(a.y, b.y));
    int k = static_cast<int>(max(a.z, b.z));
    int l = static_cast<int>(max(a.w, b.w));

    int w = k - i;
    int h = l - j;

    bomberman::collisionInfo.entityA = &entityA;
    collisionInfo.entityB = &entityB;
    collisionInfo.w = w;
    collisionInfo.h = h;

    collisionInfo.bound.x = i;
    collisionInfo.bound.y = j;
    collisionInfo.bound.z = k;
    collisionInfo.bound.w = l;


    CheckGL();

}

