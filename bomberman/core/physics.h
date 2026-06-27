#pragma once
#include "../rendering/debugRenderer.h"
#include "../misc/utils.h"

//----
//CREDITS TO MYSELF: https://github.com/thatalloguy/SchoolGameApplication/blob/main/Engine/src/physics/PhysicsObject.h
// Also https://github.com/thatalloguy/SchoolGameApplication/blob/main/Engine/src/physics/PhysicsHandler.h
// Wrote this physics engine for my intake and now i can reuse it :D

namespace bomberman {


    typedef float4 AABB;
    typedef unsigned int PhysicsID;

    enum class ObjectType {
        Static = 0,
        Rigid = 1,
        //unused
        Ghost = 2,
    };

    struct CollisionInfo {
        float2 collision_normal{0.0f, 0.0f};
        bool is_colliding = false;
    };

    // A physics object represents a physical object.
    class PhysicsObject {

    public:
        PhysicsObject() = default;
        PhysicsObject(const float2& position, const float2& init_vel, float mass = 1.0f); //init_vel = Initial Velocity
        PhysicsObject(const float4& AABB, ObjectType type);

        ~PhysicsObject() = default;

        // Applies gravity to our velocity and updates our position.
        void Step(float delta_time, float2 gravity);

        // Getters and setters
        float2 GetPosition() const;
        float2 GetVelocity() const;
        float2 GetForce() const;
        float4 GetAABB() const;
        float GetMass() const;
        float GetRestitution() const;
        PhysicsID GetID() const;
        ObjectType GetObjectType() const;

        void SetPosition(const float2& position);
        void SetVelocity(const float2& velocity);
        void SetForce(const float2& force);
        void SetMass(float mass);
        void SetRestitution(float restitution);
        void SetAABB(const float4& collider);
        void SetID(PhysicsID id);
        void SetObjectType(ObjectType type);

        void AddForce(const float2& force);
        void SetCorrection(const float2& cor);

        CollisionInfo GetCollisionInfo();
        void SetCollisionInfo(const CollisionInfo& new_info);
    private:

        PhysicsID  _id = 0;
        ObjectType _type = ObjectType::Static;
        CollisionInfo _col_info{};

        float2 _position{ 0, 0 };
        float2 _correction{ 0, 0 };
        float2 _velocity{ 0, 0 };
        float2 _force{ 0, 0 };

        float _mass = 1.0f;
        float _restitution = 0.1f;

        float4 _aabb{ 0.0f, 0.0f, 0.0f, 0.0f };
        float2 _size{ 0.0f, 0.0f };
    };


    namespace PhysicsHandler {

        // Returns wether or not the boxes are colliding
        bool Collision(const AABB& a, const  AABB& b);

        // Calculates / adjustes the velocities of the objects (and correction positions if needed)
        void ResolveCollision(PhysicsObject& a, PhysicsObject& b);

        // Returns the correct correction position
        // aCenter = the Center of box A
        // bCenter = the Center of box B
        float2 GetCorrection(const float2& aCenter, const float2& bCenter, const AABB& A, const AABB& B);


    };

    // The world is a class the manages and updates PhysicsBodies
    class World {
    public:
        // The default gravity of earth is 9.7N
        World(bool handle_memory = false, float2 gravity = { 0, 9.7f });
        ~World();

        PhysicsID AddRigidBody(PhysicsObject* object);
        PhysicsID AddStaticBody(PhysicsObject* object);
        PhysicsID AddGhostBody(PhysicsObject* object);

        void RemoveBody(PhysicsID id);
        void ClearAllStaticBodies();
        void ClearBodies();

        // Updates all of the bodies in the world
        void Step(float delta_time);

        void RenderBodies(DebugRenderer& debug_renderer);

        // getters and setters
        float2 GetGravity();
        void SetGravity(const float2& vec);
        PhysicsObject* GetFirstBody() { return _bodies[0]; };

    private:
        float2 _gravity{ 0, 9.7f };
        utils::vector<PhysicsObject*> _bodies;

        unsigned int _id_count = 0;

        // If enabled any bodies added to the world will get automatically deleted when removed (or world gets destroyed).
        bool _should_handle_memory = false;


    };
}