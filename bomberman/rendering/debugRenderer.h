#pragma once
#include "../misc/utils.h"

//------------------------
// Credits to MYSELF: https://github.com/thatalloguy/SchoolGameApplication/blob/main/Engine/src/renderer/DebugRenderer.h
/// Basicaly a copy of the debug renderer i wrote for my intake project.


namespace bomberman {


    class DrawCommand {
    public:
        virtual ~DrawCommand() = default;
        virtual void Render(Shader& shader, mat4& camera) = 0;
    };


    class LineDrawCommand : public DrawCommand {

    public:
        LineDrawCommand(const float2& begin, const float2& end);

        void Render(Shader& shader, mat4& camera) override;
    private:
        float2 _begin;
        float2 _end;
    };

    class PointDrawCommand : public DrawCommand {
    public:
        PointDrawCommand(const float2& position);

        void Render(Shader& shader, mat4& camera) override;
    private:
        mat4 _transform;
    };


    class DebugRenderer {

    public:
        DebugRenderer();
        ~DebugRenderer();


        void RenderLine(const float2& begin, const float2& end);
        void RenderPoint(const float2& position);
        void RenderBox(const float4& AABB);

        void Render(mat4& camera);

        // NEED to call clearQueue even if not calling render
        // Otherwise you will be leaking memory!
        void ClearQueue();

    private:

        utils::vector<DrawCommand*> _queue{};
        Shader _debug_shader;

    };


}