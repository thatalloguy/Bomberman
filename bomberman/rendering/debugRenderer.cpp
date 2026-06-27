#include "precomp.h"
#include "debugRenderer.h"
#include "renderer.h"

//Global constants
namespace {
    struct GPUMesh {
        //VAO = vertex array buffer
        unsigned int VAO = 0;
        //VBO = vertex buffer object
        unsigned int VBO = 0;
    };


    static float quadVertices[] = {
        // positions   // texCoords
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
        1.0f, -1.0f,  1.0f, 0.0f,

        -1.0f,  1.0f,  0.0f, 1.0f,
        1.0f, -1.0f,  1.0f, 0.0f,
        1.0f,  1.0f,  1.0f, 1.0f
    };

    GPUMesh _quad_mesh;
    GPUMesh _line_mesh;

}

bomberman::LineDrawCommand::LineDrawCommand(const float2& begin, const float2& end) {
    _begin = begin;
    _end = end;
}

void bomberman::LineDrawCommand::Render(Shader& shader, mat4& mat) {
    shader.Bind();

    mat4 transform{};

    shader.SetInputMatrix("projection", mat);
    shader.SetInputMatrix("model", transform);


    float line_verts[] = {
        _begin.x, _begin.y,  0.0f, 1.0f,
        _end.x, _end.y,  0.0f, 0.0f
    };

    // update the mesh for the line!
    glBindBuffer(GL_ARRAY_BUFFER, _line_mesh.VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(line_verts), &line_verts, GL_DYNAMIC_DRAW);

    glBindVertexArray(_line_mesh.VAO);
    glDrawArrays(GL_LINES, 0, 2);
    shader.Unbind();

}

bomberman::PointDrawCommand::PointDrawCommand(const float2& position) {
    translate(_transform, position);
    scale(_transform, { 2, 2 });
}

void bomberman::PointDrawCommand::Render(Shader& shader, mat4& mat) {
    shader.Bind();


    shader.SetInputMatrix("projection", mat);
    shader.SetInputMatrix("model", _transform);

    DrawQuad();
    shader.Unbind();
}

bomberman::DebugRenderer::DebugRenderer() : _debug_shader("assets/shaders/debug.vert", "assets/shaders/debug.frag", false){



    //Setup a reusable quad mesh;
    glGenVertexArrays(1, &_quad_mesh.VAO);
    glGenBuffers(1, &_quad_mesh.VBO);

    glBindVertexArray(_quad_mesh.VAO);
    glBindBuffer(GL_ARRAY_BUFFER, _quad_mesh.VBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));


    // Do the same for the Line mesh
    // We dont upload any vertices to it since the line command will do that internally.
    glGenVertexArrays(1, &_line_mesh.VAO);
    glGenBuffers(1, &_line_mesh.VBO);

    glBindVertexArray(_line_mesh.VAO);
    glBindBuffer(GL_ARRAY_BUFFER, _line_mesh.VBO);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

}

bomberman::DebugRenderer::~DebugRenderer() {
    glDeleteVertexArrays(1, &_quad_mesh.VAO);
    glDeleteVertexArrays(1, &_line_mesh.VAO);
}

void bomberman::DebugRenderer::RenderLine(const float2& begin, const float2& end) {
    _queue.push_back(new LineDrawCommand(begin, end));
}

void bomberman::DebugRenderer::RenderPoint(const float2& position) {

    _queue.push_back(new PointDrawCommand(position));
}

void bomberman::DebugRenderer::RenderBox(const float4& AABB) {
    // Its a very hacky way to do this.
    // But it doesnt matter for the scale of this project.



    _queue.push_back(new PointDrawCommand({ AABB.x, AABB.y }));
    _queue.push_back(new PointDrawCommand({ AABB.z, AABB.y }));
    _queue.push_back(new PointDrawCommand({ AABB.x, AABB.w }));
    _queue.push_back(new PointDrawCommand({ AABB.z, AABB.w }));

    _queue.push_back(new LineDrawCommand({ AABB.x, AABB.y }, { AABB.z, AABB.y }));
    _queue.push_back(new LineDrawCommand({ AABB.x, AABB.y }, { AABB.z, AABB.w }));

    _queue.push_back(new LineDrawCommand({ AABB.x, AABB.w }, { AABB.z, AABB.y }));
    _queue.push_back(new LineDrawCommand({ AABB.z, AABB.w }, { AABB.x, AABB.w }));

}

void bomberman::DebugRenderer::Render(mat4& mat) {
    for (DrawCommand* command : _queue) {
        command->Render(_debug_shader, mat);
    }
}

void bomberman::DebugRenderer::ClearQueue() {
    for (DrawCommand* command : _queue) {
    	delete command;
    }

    _queue.clear();
}