#include "precomp.h"
#include "physics.h"

#include "../rendering/renderer.h"


bomberman::PhysicsObject::PhysicsObject(const float2& position, const float2& init_vel,
                                        float mass) : _position(position), _velocity(init_vel), _mass(mass) {
}

bomberman::PhysicsObject::PhysicsObject(const float4& AABB, ObjectType type) {

    _position = { AABB.x, AABB.y };

    _aabb = AABB;

    _type = type;
}

void bomberman::PhysicsObject::Step(float , float2 gravity) {
    _velocity *= {0.75, 0.75};
    // Calc the force from the gravity; F = m * G;
    _force += gravity * _mass;

    // calc our new velocity, V+ = F/m
    _velocity += (_force / _mass);

    //Reset our force;
    _force = { 0, 0 };


    // Update our position according to our velocity
    _position += _velocity;
    

    // Add a correction (if we have one)
    _position -= _correction;
    // Reset the correction;
    _correction = { 0, 0 };
}


float2 bomberman::PhysicsObject::GetPosition() const {
    return _position;
}

float2 bomberman::PhysicsObject::GetVelocity() const {
    return _velocity;
}

float2 bomberman::PhysicsObject::GetForce() const {
    return _force;
}

float bomberman::PhysicsObject::GetMass() const {
    return _mass;
}

float4 bomberman::PhysicsObject::GetAABB() const {
    return _aabb;
}

void bomberman::PhysicsObject::SetPosition(const float2& position) {
    _position = position;
}

void bomberman::PhysicsObject::SetVelocity(const float2& velocity) {
    _velocity = velocity;
}

void bomberman::PhysicsObject::SetForce(const float2& force) {
    _velocity = force;
}

void bomberman::PhysicsObject::SetMass(float mass) {
    _mass = mass;
}

void bomberman::PhysicsObject::SetAABB(const float4& collider) {
    _aabb = collider;
}

void bomberman::PhysicsObject::SetRestitution(float restitution) {
    _restitution = restitution;
}

float bomberman::PhysicsObject::GetRestitution() const {
    return _restitution;
}

bomberman::PhysicsID bomberman::PhysicsObject::GetID() const {
    return _id;
}

void bomberman::PhysicsObject::SetID(bomberman::PhysicsID id) {
    _id = id;
}

bomberman::ObjectType bomberman::PhysicsObject::GetObjectType() const {
    return _type;
}

void bomberman::PhysicsObject::SetObjectType(bomberman::ObjectType type) {
    _type = type;
}

void bomberman::PhysicsObject::AddForce(const float2& vel) {
    _force -= vel;
}

void bomberman::PhysicsObject::SetCorrection(const float2& cor) {
    _correction = cor;
}

bomberman::CollisionInfo bomberman::PhysicsObject::GetCollisionInfo() {
    CollisionInfo info{ _col_info.collision_normal, _col_info.is_colliding };

    // Clear our internal collision info so that it isnt outdated.
    _col_info = { {0, 0}, false};

    return info;
}

void bomberman::PhysicsObject::SetCollisionInfo(const CollisionInfo& new_info) {
    _col_info = new_info;
}


//----------------------- PHYSICS HANLDER


bool bomberman::PhysicsHandler::Collision(const bomberman::AABB& a, const bomberman::AABB& b) {

    // .x = min.x, .y = min.y
    // .z = max.x, .w = max.y

    // Collision detection using the seperate axis theory.
    // We check if any axis are seperated which automatically means no collision, so we return false

    //Too expensive
    //if (dist({ a.x, a.y }, { b.x, b.y }) > 150)
    //    return false;

	if (a.z < b.x || a.x > b.z) return false;
    if (a.w < b.y || a.y > b.w) return false;

    // If both axes arent seperated it means we have a collision
    return true;
}


float2 bomberman::PhysicsHandler::GetCorrection(const float2& aCenter, const float2& bCenter, const AABB& A, const AABB& B) {

    float2 correction{ 0, 0 };
     
    // First check what axis we should correct (the one most far away from each other)
    
    if (bCenter.x <= A.x || bCenter.x >= A.z){
        // Check if A is left or right from B
        if (aCenter.x <= bCenter.x) {
            correction.x = (B.x - A.z);
        }
        else {
            correction.x = (B.z - A.x);
        }
    } if (bCenter.y <= A.y || bCenter.y >= A.w) {
        // Check if A is above or below B
        if (aCenter.y <= bCenter.y) {
            correction.y = (B.y - A.w);
        }
        else {
            correction.y = (B.w - A.y);
        }
        if (abs(correction.x) >= abs(correction.y))
        {
            correction.x = 0;
        } else if (abs(correction.x) != 0)
        {
            correction.y = 0;
        }
    }




    return correction;
}

void bomberman::PhysicsHandler::ResolveCollision(bomberman::PhysicsObject& a, bomberman::PhysicsObject& b) {

    if (a.GetObjectType() == ObjectType::Static &&
        b.GetObjectType() == ObjectType::Static ||
        a.GetObjectType() == ObjectType::Ghost ||
        b.GetObjectType() == ObjectType::Ghost ||
        (a.GetObjectType() == ObjectType::Rigid && b.GetObjectType() == ObjectType::Rigid))
        return;

    float2 normal{0, 0};

    // n = normalize(bVel - aVel)
    normal.x = (b.GetVelocity().x - a.GetVelocity().x);
    normal.y = (a.GetVelocity().y - b.GetVelocity().y);

    float nA = sqrt(( normal.x * normal.x) + (normal.y * normal.y));
    normal = {normal.x / nA, normal.y / nA };

    if (normal.x > 1 || normal.x < -1 ||
        normal.y > 1 || normal.y < -1) {
        //printf("[Warning] Invalid collision normal\n");
        return;
    }

    //apply velocities only to rigidbodies
    if (a.GetObjectType() == ObjectType::Rigid) {
        a.SetVelocity({ 0, 0 });
        //a.SetVelocity(a.GetVelocity() + (impulse * (1 / a.GetMass())));
    }

    if (b.GetObjectType() == ObjectType::Rigid) {
        //b.SetVelocity(b.GetVelocity() - (impulse * (1 / b.GetMass())));
        b.SetVelocity({ 0, 0 });
    }

    // Calculate and apply corrections
    float4 aBox = a.GetAABB();
    float4 bBox = b.GetAABB();

    float2 aCenter = { aBox.x + ((aBox.z - aBox.x) / 2),aBox.y + ((aBox.w - aBox.y) / 2) };
    float2 bCenter = { bBox.x + ((bBox.z - bBox.x) / 2),bBox.y + ((bBox.w - bBox.y) / 2) };


    float2 correction = GetCorrection(aCenter, bCenter, aBox, bBox);

    if (a.GetObjectType() == ObjectType::Rigid)
        a.SetCorrection(-correction);
    if (b.GetObjectType() == ObjectType::Rigid)
        b.SetCorrection(correction);

    a.SetCollisionInfo({  normal, true});
    b.SetCollisionInfo({  normal, true});

}

bomberman::World::World(bool handle_memory, float2 gravity) : _gravity(gravity), _should_handle_memory(handle_memory) {}

bomberman::World::~World() {
    if (_should_handle_memory) {
        for (PhysicsObject* body : _bodies) {
            delete body;
        }
    }
}

bomberman::PhysicsID bomberman::World::AddRigidBody(bomberman::PhysicsObject* object) {
    // Increase the id count
    _id_count++;

    // Update the object's information
    object->SetID(_id_count);
    object->SetObjectType(ObjectType::Rigid);

    _bodies.push_back(object);

    return _id_count;
}

bomberman::PhysicsID bomberman::World::AddStaticBody(bomberman::PhysicsObject* object) {
    // Increase the id count
    _id_count++;

    // Update the object's information
    object->SetID(_id_count);
    object->SetObjectType(ObjectType::Static);

    _bodies.push_back(object);

    return _id_count;
}


bomberman::PhysicsID bomberman::World::AddGhostBody(bomberman::PhysicsObject* object) {
    // Increase the id count
    _id_count++;

    // Update the object's information
    object->SetID(_id_count);
    object->SetObjectType(ObjectType::Ghost);

    _bodies.push_back(object);

    return _id_count;
}


void bomberman::World::RemoveBody(bomberman::PhysicsID id) {
    for (int i = 0; i < _bodies.length(); i++) {
        if (_bodies[i]->GetID() == id) {
            if (_should_handle_memory) {
                delete _bodies[i];
            }
            _bodies.remove(i);
        }
    }
}



void bomberman::World::Step(float delta_time) {
    //First  Update all of our rigidBodies;
    for (PhysicsObject* body : _bodies) {
        if (body->GetObjectType() == ObjectType::Rigid) {
            body->Step(delta_time, _gravity);
        }
    }

    // Then check for collision and resolve them if needed
    for (int i = 0; i < _bodies.length(); i++) {
        PhysicsObject* a = _bodies[i];
        if (a->GetObjectType() == ObjectType::Ghost)
            break;

        // Loop through all of the other objects, since we have to check against every other body
        // Isnt optimale but we wont have so much bodies in our vector so it wont be a issue
        for (PhysicsObject* b : _bodies) {
            if (b->GetID() == a->GetID())
                break;

            if (PhysicsHandler::Collision(a->GetAABB(), b->GetAABB())) {
                PhysicsHandler::ResolveCollision(*a, *b);
            }
        }
    }
}

void bomberman::World::RenderBodies(bomberman::DebugRenderer& debug_renderer) {
    int i = 0;
    for (PhysicsObject* body : _bodies) {
        if (body == nullptr) {
            _bodies.remove(i);
            break;
        }

        debug_renderer.RenderBox(body->GetAABB());

        i++;
    }
}

float2 bomberman::World::GetGravity() {
    return _gravity;
}

void bomberman::World::SetGravity(const float2& vec) {
    _gravity = vec;
}

void bomberman::World::ClearAllStaticBodies() {

    for (PhysicsObject* body : _bodies) {
        if (_should_handle_memory && body->GetObjectType() == ObjectType::Static) {
            delete body;
        }
    }

    _bodies.clear();
    _id_count = 0;
}

void bomberman::World::ClearBodies()
{
    for (PhysicsObject* body : _bodies) {
        if (_should_handle_memory) {
            delete body;
        }
    }

    _bodies.clear();
    _id_count = 0;
}
