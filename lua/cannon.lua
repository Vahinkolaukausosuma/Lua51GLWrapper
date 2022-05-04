if not CreateDisplay then require"glwrapper" end
local Keys = {}
local LastRun = os.clock()
local DOWN = true
local UP = false
local FPS = 85
local MinFrameTime = 1/FPS
local ScreenW,ScreenH = 1920,1080
local flags = SDL_WINDOW_OPENGL
local flags = BitOR(flags,SDL_WINDOW_ALWAYS_ON_TOP)
-- local flags = BitOR(flags,SDL_WINDOW_RESIZABLE)
local flags = BitOR(flags,SDL_WINDOW_SKIP_TASKBAR)
local flags = BitOR(flags,SDL_WINDOW_BORDERLESS)
local BoxCount = 1
local cx,cy = 1419,1021
local Health = 60
local mx,my,lastmx,lastmy = 0,0,0,0
local left = 1
local right = 2
local top = 3
local bottom = 4


function table.Count(tbl)
	local cnt = 0
	for k,v in pairs(tbl) do 
		cnt = cnt + 1
	end
	return cnt
end

function Image(name)
	local w,h,channelCount,pixels = LoadTexture(name)
	local pixelsAlpha = {}
	for k,v in pairs(pixels) do
		pixelsAlpha[k] = v.a
	end
	if w then
		return {w=w,h=h,channelCount=channelCount,pixels=pixels,pixelsAlpha=pixelsAlpha,pixelCount = table.Count(pixels)}
	end
end
MoveWindow_ = MoveWindow
function MoveWindow(x,y)
	if x == x and y == y then
		-- print(x,y)
		MoveWindow_(x,y)
	end
end

local Boxes = {}

local function DrawPixel(y,x,w,h)
	local ww = w/2
	local hh = h/2
	x = w-x
	y = y + 1
	x = (x-ww) / ww
	y = (y-hh) / hh
	
	GlDraw2f(x,y)
end

local CannonSelf = Image("tex/CannonSelf.png")
local CannonWheel = Image("tex/cannonWheel.png")
local CannonBall = Image("tex/chungo24.png")
CannonBall.Active = true
CannonBall.x = 0
CannonBall.y = 0
CannonBall.vx = 0
CannonBall.vy = 0
CannonBall.Shot = false

local Explosion = Image("tex/empty512.png")

for x = 0,CannonBall.w do 
	for y = 0,CannonBall.h do 
		local pxc = y * CannonBall.w + x
		-- local px = Explosion.pixelsAlpha[pxc]
		if CannonBall.pixelsAlpha[pxc] and CannonBall.pixelsAlpha[pxc] ~= 0 then
			-- px = Explosion.pixels[pxc]
			local ind = (y+Explosion.w/2-CannonBall.w/2) * Explosion.w + (x+Explosion.h/2-CannonBall.h/2)
			Explosion.pixels[ind] = CannonBall.pixels[pxc]
			Explosion.pixels[ind].vx = CannonBall.pixels[pxc].vx
			Explosion.pixels[ind].vy = CannonBall.pixels[pxc].vy
			-- DrawPixel(x,y,CannonBall.w,CannonBall.h)
		end
	end
end





SetWindowPointer(1)
CreateDisplay(CannonWheel.w,CannonWheel.h,"Cannon",flags) 
MakeWindowSeethrough()


SetWindowPointer(2)
CreateDisplay(CannonBall.w,CannonBall.h,"Ball",flags) 
MakeWindowSeethrough()

SetWindowPointer(3)
CreateDisplay(Explosion.w,Explosion.h,"Explosion",flags) 
MakeWindowSeethrough()


function CheckKeys()
	for i = 1,255 do
		-- if IsKeyDown(i) then print(i) end
		if IsKeyDown(i) ~= Keys[i] then
			-- print(
			-- Keys[i] = not Keys[i]
			-- OnChangedKey(i,Keys[i])
		end
	end
end

function OnChangedKey(i,state)
	-- print(i,state)	
	if i == 1 and state == DOWN then
		-- print(i)
		-- mx,my = GetMousePosition()
		-- for i = 1,BoxCount do
		-- Boxes[i].x = mx
		-- Boxes[i].y = my
		-- end
	end
end

local lastshot = os.clock()
-- for i = 1,10 do 
while not DisplayIsClosed() do
	TimeLapsed = os.clock()-LastRun
	LastRun = os.clock()
	test = os.clock()
	local mx,my = GetMousePosition()
	local cannonDirX,cannonDirY = cx-mx,cy-my
	local ang = math.atan2(cannonDirX,cannonDirY)-math.pi/2
	cannonDirX,cannonDirY = 1421.5-mx,1022.5-my
	local hitX,hitY = 0,0
	
	-- MoveWindow(mx+extrax,my+extray)
	
	
	if not CannonBall.Shot then
		CannonBall.x = 1421.5-CannonBall.w/2 +math.sin(ang-math.pi/2)*32
		CannonBall.y = 1022.5-CannonBall.h/2 +math.cos(ang-math.pi/2)*32
	end
	if IsKeyDown(70) and os.clock()-lastshot > 0.5 then
		lastshot = os.clock()
		CannonBall.Shot = true
		CannonBall.Active = true
		CannonBall.x = 1421.5-CannonBall.w/2 +math.sin(ang-math.pi/2)*32
		CannonBall.y = 1022.5-CannonBall.h/2 +math.cos(ang-math.pi/2)*32
		CannonBall.vx = math.sin(ang-math.pi/2)*1850
		CannonBall.vy = math.cos(ang-math.pi/2)*1850
	end
	if CannonBall.Shot then 
		CannonBall.vx = CannonBall.vx * 0.999
		CannonBall.vy = CannonBall.vy + 5.5
		CannonBall.x = CannonBall.x + CannonBall.vx * TimeLapsed
		CannonBall.y = CannonBall.y + CannonBall.vy * TimeLapsed
		
		local hitCorner = false
		
		if CannonBall.x < 0 then CannonBall.x = 0 CannonBall.vx = CannonBall.vx   * -0.05 CannonBall.Shot = false CannonBall.Active = false hitCorner = left end -- left
		if CannonBall.y < 0 then CannonBall.y = 0 CannonBall.vy = CannonBall.vy   * -0.05 CannonBall.Shot = false CannonBall.Active = false hitCorner = bottom end -- bottom
		if CannonBall.x+CannonBall.w > ScreenW then CannonBall.vx = CannonBall.vx * -0.05 CannonBall.x = ScreenW-CannonBall.w CannonBall.Shot = false CannonBall.Active = false hitCorner = right end --right
		if CannonBall.y+CannonBall.h > ScreenH then CannonBall.vy = CannonBall.vy * -0.05 CannonBall.y = ScreenH-CannonBall.h CannonBall.Shot = false CannonBall.Active = false hitCorner = top end --top
		hitX,hitY = CannonBall.x,CannonBall.y
		if hitCorner then
			CannonBall.hitTime = os.clock()
		end
		
		if not CannonBall.Active then
			SetWindowPointer(3)
			CannonBall.Shot = false
			
			CannonBall.x = CannonBall.x - Explosion.w/2
			CannonBall.y = CannonBall.y - Explosion.h/2
			local extrax,extray = 0,0
			if CannonBall.x+Explosion.w > ScreenW then -- xd
				extrax = extrax + (ScreenW-CannonBall.x) - Explosion.w
			end
			if CannonBall.y+Explosion.h > ScreenH then
				extray = extray + (ScreenH-CannonBall.y) - Explosion.h
			end
			if CannonBall.y-Explosion.h < -Explosion.h then
				extray = -CannonBall.y
			end
			if CannonBall.x-Explosion.w < -Explosion.w then
				extrax = -CannonBall.x
			end
			
			Explosion.pixels = {}
			
			for k,v in pairs(CannonBall.pixels) do
				local y = math.floor(k/CannonBall.w)
				local x = math.floor(k%CannonBall.w)
				-- print(x,y)
				local pxc = y * CannonBall.w + x
				if CannonBall.pixelsAlpha[pxc] and CannonBall.pixelsAlpha[pxc] ~= 0 then
					local ind = math.floor(Explosion.h-(hitY-CannonBall.y+extray-y)) * Explosion.w + math.floor(Explosion.w-(-x+hitX-CannonBall.x+extrax))
					Explosion.pixels[ind] = CannonBall.pixels[pxc]

					Explosion.pixels[ind].x = math.floor(Explosion.w-(-x+hitX-CannonBall.x+extrax))
					Explosion.pixels[ind].y = math.floor(Explosion.h-(hitY-CannonBall.y+extray-y))

					Explosion.pixels[ind].vx = CannonBall.vx + math.random(-90,90)
					Explosion.pixels[ind].vy = CannonBall.vy + math.random(-90,90)
				end
			end
			MoveWindow(CannonBall.x+extrax,CannonBall.y+extray)
		end
	end
	
	
	SetWindowPointer(1)
	DisplayClear()
	GlBeginPoints()
	for x = 0,CannonSelf.w do 
		for y = 0,CannonSelf.h do 
			local pxc = y * CannonSelf.w + x
			-- local px = Explosion.pixelsAlpha[pxc]
			if CannonSelf.pixelsAlpha[pxc] and CannonSelf.pixelsAlpha[pxc] ~= 0 then
				SetColorAlpha(CannonSelf.pixels[pxc].r,CannonSelf.pixels[pxc].g,CannonSelf.pixels[pxc].b,CannonSelf.pixels[pxc].a)
				local Xx = x-CannonSelf.w/2
				local Yy = y-CannonSelf.h/2
				local xx = Xx * math.cos(ang) + Yy * math.sin(ang)
				local yy = Yy * math.cos(ang) - Xx * math.sin(ang)
				DrawPixel(3+xx+CannonSelf.w/2,23+yy+CannonSelf.h/2,CannonSelf.w,CannonSelf.h)
			end
		end
	end 
	for x = 0,CannonWheel.w do 
		for y = 0,CannonWheel.h do 
			local pxc = y * CannonWheel.w + x
			
			if CannonWheel.pixelsAlpha[pxc] and CannonWheel.pixelsAlpha[pxc] ~= 0 then
				SetColorAlpha(CannonWheel.pixels[pxc].r,CannonWheel.pixels[pxc].g,CannonWheel.pixels[pxc].b,CannonWheel.pixels[pxc].a)
				DrawPixel(x,y,CannonWheel.w,CannonWheel.h)
			end
		end
	end 
	
	GlEnd()
	DisplayUpdate()
	MoveWindow(1379, 960)
	
	
	
	SetWindowPointer(2)
	MoveWindow(CannonBall.x,CannonBall.y)
	DisplayClear()
	GlBeginPoints()
	if CannonBall.Active then
		for x = 0,CannonBall.w do 
			for y = 0,CannonBall.h do 
				local pxc = y * CannonBall.w + x
				if CannonBall.pixelsAlpha[pxc] and CannonBall.pixelsAlpha[pxc] ~= 0 then
					SetColorAlpha(CannonBall.pixels[pxc].r,CannonBall.pixels[pxc].g,CannonBall.pixels[pxc].b,CannonBall.pixels[pxc].a)
					DrawPixel(x,y,CannonBall.w,CannonBall.h)
				end
			end
		end 
	end
	GlEnd()
	DisplayUpdate()	
	
	SetWindowPointer(3)
	DisplayClear()
	GlBeginPoints()


	for k,v in pairs(Explosion.pixels) do
		if Explosion.pixels[k] and Explosion.pixels[k].vy then
			Explosion.pixels[k].x = Explosion.pixels[k].x + Explosion.pixels[k].vx * TimeLapsed
			Explosion.pixels[k].y = Explosion.pixels[k].y + Explosion.pixels[k].vy * TimeLapsed
			Explosion.pixels[k].vx = Explosion.pixels[k].vx * 0.998
			Explosion.pixels[k].vy = Explosion.pixels[k].vy + 0.76
		end
	end

	for k,v in pairs(Explosion.pixels) do
		if v.x and v.r then
			SetColorAlpha(v.r,v.g,v.b,0)
			DrawPixel(v.x,v.y,Explosion.w,Explosion.h)
		end
	end
	GlEnd()
	DisplayUpdate()
	
	
	lastmx = mx
	lastmy = my
	
	if os.clock()-test > MinFrameTime then
		
		Sleep(os.clock()-test)
		-- print(string.format("%.2f ms",(os.clock()-test)*1000))
	end
end

DestroyDisplay()