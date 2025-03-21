local ecs = require "ecs"

local function rand_flt(from, to)
	local maxNumber = 32767
    return from + (math.random(maxNumber) / maxNumber) * (to - from)
end

local function move(it)
    for pos, vel, ent in ecs.each(it) do
        pos.x = pos.x + vel.x * it.delta_time
        pos.y = pos.y + vel.y * it.delta_time
		pos.z = pos.z + vel.z * it.delta_time
    end
end

local function gravity(it)
    for pos, vel, grav, plane, ent in ecs.each(it) do
        local planeEpsilon = 0.1
		
		if plane.x * pos.x + plane.y * pos.y + plane.z * pos.z < plane.w + planeEpsilon then
			do return end
		end
		
		vel.x = vel.x + grav.x * it.delta_time
        vel.y = vel.y + grav.y * it.delta_time
		vel.z = vel.z + grav.z * it.delta_time
    end
end

local function FrictionSystem(it)
    for vel, friction, ent in ecs.each(it) do
        vel.x = vel.x - vel.x * friction.value * it.delta_time
		vel.y = vel.y - vel.y * friction.value * it.delta_time
		vel.z = vel.z - vel.z * friction.value * it.delta_time
    end
end

local function ShiverSystem(it)
    for pos, shiver, ent in ecs.each(it) do
        pos.x = pos.x + rand_flt(-shiver.value, shiver.value)
		pos.y = pos.y + rand_flt(-shiver.value, shiver.value)
		pos.z = pos.z + rand_flt(-shiver.value, shiver.value)
    end
end

local function BounceSystem(it)
    for pos, vel, plane, bounciness, ent in ecs.each(it) do
        local dotPos = plane.x * pos.x + plane.y * pos.y + plane.z * pos.z
		local dotVel = plane.x * vel.x + plane.y * vel.y + plane.z * vel.z
		
		if dotPos < plane.w then
			pos.x = pos.x - (dotPos - plane.w) * plane.x
			pos.y = pos.y - (dotPos - plane.w) * plane.y
			pos.z = pos.z - (dotPos - plane.w) * plane.z

			vel.x = vel.x - (1.0 + bounciness.value) * plane.x * dotVel
			vel.y = vel.y - (1.0 + bounciness.value) * plane.y * dotVel
			vel.z = vel.z - (1.0 + bounciness.value) * plane.z * dotVel
		end
    end
end

local function TTLSystem(it)
    for ttl, ent in ecs.each(it) do
        ttl.ttl = ttl.ttl-1
        if ttl.ttl<=0 then
            ecs.delete(ent)
        end
    end
end

local positions = {}
local velocities = {}
local entities = {}
local hits = {}

--save bullets
local function CollisionSave(it)
    for pos, vel, tag, ent in ecs.each(it) do
        table.insert(positions, pos)
        table.insert(velocities, vel)
        table.insert(entities, ent)
        table.insert(hits, false)
    end
end

--find collisions
local function CollisionEval(it)
    for pos, vel, tag, ent in ecs.each(it) do
        for i = 1, #positions do
            local sq_dist = (positions[i].x - pos.x)*(positions[i].x - pos.x)+(positions[i].y - pos.y)*(positions[i].y - pos.y)+(positions[i].z - pos.z)*(positions[i].z - pos.z)
            if sq_dist<5 then
                tag.switchColor = true
                vel.x=vel.x + rand_flt(-4, 4)
                vel.y=vel.y + rand_flt(-4, 4)
                vel.z=vel.z + rand_flt(-4, 4)
                hits[i] = true
            end
        end
    end
end

--clear saved bullets
local function CollisionClear(it)
    for i = 1, #positions do
        if hits[i] then
            ecs.delete(entities[i])
        end
    end
    positions = {}
    velocities = {}
    entities = {}
    hits = {}
end


ecs.system(move, "Move", ecs.OnUpdate, "Position, Velocity")
ecs.system(gravity, "grav", ecs.OnUpdate, "Position, Velocity, Gravity, BouncePlane")
ecs.system(FrictionSystem, "FrictionSystem", ecs.OnUpdate, "Velocity, FrictionAmount")
ecs.system(ShiverSystem, "ShiverSystem", ecs.OnUpdate, "Position, ShiverAmount")
ecs.system(BounceSystem, "BounceSystem", ecs.OnUpdate, "Position, Velocity, BouncePlane, Bounciness")

ecs.system(TTLSystem, "TTLSystem", ecs.OnUpdate, "BulletDestructor")
ecs.system(CollisionSave, "CollisionSave", ecs.OnUpdate, "Position, Velocity, BulletDestructor")
ecs.system(CollisionEval, "CollisionEval", ecs.OnUpdate, "Position, Velocity, SceneObject")
ecs.system(CollisionClear, "CollisionClear", ecs.OnUpdate, "Player")