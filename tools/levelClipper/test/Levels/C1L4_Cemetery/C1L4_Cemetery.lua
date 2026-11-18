o.PlayerLightTemplate = "PlayerLight_Cemetery.CLight"

o._MagicCard = MagicCards.timeCards[6]
o._CardTask = Languages.Texts[396]

--[[o._ColParams = {
	particlesOnCollision = "butk",
	particlesOnCollisionSize = 0.12,
	collisionMinimumStrength = 2.4,
	amountReportingCollisions = 1,
	miminalMassReportingCollision = 45,
	collisionMinimumFrequency = 1,
	maximalMassReportingCollision = 5000,
}

--]]
o._ExplodeMeshSounds = {}
o._ExplodeMeshSounds[24] = SoundsDefs.catacombs_stones						-- ?


function C1L4_Cemetery:OnPlay(first)
    if first then
        Player.EnabledWeapons = {"PainKiller",nil,nil,nil}
        Player.Ammo = 
        {
            Shotgun  = 0,
            MiniGun  = 0,
            Grenades = 0,
            Stakes   = 0,
            IceBullets = 0,
            Shurikens    = 0,
            Electro      = 0, 
			Rifle		 = 0,
			FlameThrower = 0,
            Bolt         = 0,
            HeaterBomb   = 0,
        }
        Player.Health = 100
        
        --[[local colParams = self._ColParams
        for i=20,24 do
			local count = ENTITY.EnableCollisionsToAll(true, colParams.collisionMinimumFrequency, colParams.collisionMinimumStrength,
					 colParams.miminalMassReportingCollision, colParams.maximalMassReportingCollision, colParams.amountReportingCollisions * 100, i)
			--Game:Print(i.." ecol to "..count)
		end
		PHYSICS.ActiveMeshGroupSetActivationParams(24, true, colParams.collisionMinimumFrequency, colParams.collisionMinimumStrength,
			colParams.miminalMassReportingCollision,	colParams.maximalMassReportingCollision, colParams.amountReportingCollisions * 100,
			colParams.timeToLive,colParams.timeToLiveRandomize)
		--]]
		
		self._lastTimeQuake = 0
        self._r_FlashLight:On()
    
    end
end
--============================================================================
-- Endurance (Finish the level)
function C1L4_Cemetery:GetCardStatus()
	local card = self._MagicCard

	if Game.CardsAvailable[card.index] then return 1 end

	return -1
end

--[[
function o:OnCollision(x,y,z,e,h_me)
	local cg = PHYSICS.GetHavokBodyActiveGroup(h_me)
	local sound = self._ColParamsSounds[cg]
	if sound then
        local colParams = self._ColParams
		AddPFX(colParams.particlesOnCollision, colParams.particlesOnCollisionSize*FRand(0.7,1.0), Vector:New(x,y,z))
		local snd = sound.samples[math.random(1,table.getn(sound.samples))]
		PlaySound3D(snd,x,y,z,sound.dist1,sound.dist2)
	else
		if debugMarek then Game:Print("oncol = "..cg.." nod def") end
	end
end
--]]

function o:OnExplodeMesh(actgrp,x,y,z)
	local sound = self._ExplodeMeshSounds[actgrp]
	if sound then
        --local colParams = self.ColParams
		--AddPFX(colParams.particlesOnCollision, colParams.particlesOnCollisionSize*FRand(0.7,1.0), Vector:New(x,y,z))

		if self._lastTimeQuake + 1.0 < Game.currentTime then	-- zeby nie za czesto
			Game._EarthQuakeProc:Add(x, y, z, 10, 15, 0.15, 0.15, 1.0)
			self._lastTimeQuake = Game.currentTime
		end
	
		local snd = sound.samples[math.random(1,table.getn(sound.samples))]
		PlaySound3D(snd,x,y,z,sound.dist1,sound.dist2)
	end
end
