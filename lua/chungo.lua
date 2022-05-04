if not CreateDisplay then require"glwrapper" end
local Width = 30
local Height = 30
local Hw = Width/2
local Hh = Height/2
local Hh = Height/2
local Wh = Width/2
local yrat = (Height/Width)
local xrat = (Width/Height)
local Keys = {}
local DOWN = true
local UP = false
local FPS = 144
local MinFrameTime = 1000/FPS
local ScreenW,ScreenH = 1920,1080
local flags = SDL_WINDOW_OPENGL
local flags = BitOR(flags,SDL_WINDOW_ALWAYS_ON_TOP)
-- local flags = BitOR(flags,SDL_WINDOW_RESIZABLE)
local flags = BitOR(flags,SDL_WINDOW_SKIP_TASKBAR)
local flags = BitOR(flags,SDL_WINDOW_BORDERLESS)
local BoxCount = 9
local Health = 60
local mx,my,lastmx,lastmy = 0,0,0,0
function Box(x,y,vx,vy,w,h,r,g,b) return {x=x,y=y,vx=vx,vy=vy,w=w,h=h,r=r,g=g,b=b,mass=math.random(1,90000)/4000,radius=30} end
function Image(name)
	local w,h,channelCount,pixels = LoadTexture(name)
	if w then
		return {w=w,h=h,channelCount=channelCount,pixels=pixels}
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
-- CaptureScreen()
local function DrawLine(x,y,x2,y2)
	x = (x-Wh) / Wh 
	x2 = (x2-Wh) / Wh 
	y = (y-Hh) / Hh 
	y2 = (y2-Hh) / Hh 
	GlDrawLine(x,y,x2,y2)
end
local function DrawPixel(y,x)
	-- x = x / 2
	-- y = y /2
	-- y = y*yrat 
	x = Width-x
	
	x = (x-Hw) / Hw 
	y = (y-Hh) / Hh 
	
	GlDraw2f(x,y)
end
for i = 1,BoxCount do
	SetWindowPointer(i)
	CreateDisplay(Width,Height,"Chungo "..tostring(i),flags)
	-- local rand = openssl.random(1):byte() .. openssl.random(1):byte() .. openssl.random(1):byte()
	-- math.randomseed(rand)
	
	Boxes[i] = 	Box(
		math.random(1,ScreenH), 
		math.random(1,ScreenW), 
		math.random(-150,150), 
		math.random(-150,150), 
		30,
		30,
		math.random(0,255)/255,
		math.random(0,255)/255,
		math.random(0,255)/255
	)
end

local Ina = Image("tex/inat.png")

LastRun = os.clock()
drawthing = {}
for k = 1,BoxCount do 
	drawthing[k] = 0
end

for i = 1,255 do
	Keys[i] = UP
end

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

local t = 0
while not DisplayIsClosed() do
	TimeLapsed = os.clock()-LastRun
	LastRun = os.clock()
	mx,my = GetMousePosition()
	mx = mx + 1
	my = my + 1
	-- CheckKeys()
	if IsKeyDown(1) then
		for i = 1,BoxCount do
			Health = 60
			Boxes[i].vx = (mx-lastmx) * 49
			Boxes[i].vy = (my-lastmy) * 49
			Boxes[i].x = mx
			Boxes[i].y = my
		end
	end

	for k = 1,BoxCount do
		-- Boxes[k].vy = Boxes[k].vy + 7
		-- print(Boxes[k].y+Boxes[k].h)
		if Boxes[k].y+Boxes[k].h == 1080 then Boxes[k].vx = Boxes[k].vx * 0.965 end
		drawthing[k] = drawthing[k] + 1
		SetWindowPointer(k)
		Boxes[k].x = Boxes[k].x + Boxes[k].vx * TimeLapsed
		Boxes[k].y = Boxes[k].y + Boxes[k].vy * TimeLapsed
		
		if Boxes[k].x < 0 then Boxes[k].x = 0 Boxes[k].vx = Boxes[k].vx * -0.5 if k == 1 then Health = Health - math.random(10,25) end end -- left
		if Boxes[k].y < 0 then Boxes[k].y = 0 Boxes[k].vy = Boxes[k].vy * -0.5 if k == 1 then Health = Health - math.random(10,25) end end -- bottom
		if Boxes[k].x+Boxes[k].w > ScreenW then Boxes[k].x = ScreenW-Boxes[k].w Boxes[k].vx = Boxes[k].vx * -0.5 if k == 1 then Health = Health - math.random(10,25) end end --right
		if Boxes[k].y+Boxes[k].h > ScreenH then Boxes[k].y = ScreenH-Boxes[k].h Boxes[k].vy = Boxes[k].vy * -0.5 if k == 1 then Health = Health - math.random(10,25) end end --top
		
		for u = 1,BoxCount do
			if k ~= u then
				
				
				local fDistance = math.sqrt((Boxes[k].x - Boxes[u].x)*(Boxes[k].x - Boxes[u].x) + (Boxes[k].y - Boxes[u].y)*(Boxes[k].y - Boxes[u].y))
				if fDistance ~= fDistance or fDistance == 0 then fDistance = math.random(5,35) end
				if fDistance <= Boxes[k].radius then
					
					local fOverlap = 0.05 * (fDistance - Boxes[k].radius - Boxes[u].radius)
					if fOverlap ~= fOverlap then fOverlap = math.random(4,30) end
					
					-- if Boxes[u].y < Boxes[k].y then
					if Health <= 0 then
						Boxes[u].x = Boxes[u].x + fOverlap * (Boxes[k].x - Boxes[u].x) / fDistance
						Boxes[u].y = Boxes[u].y + fOverlap * (Boxes[k].y - Boxes[u].y) / fDistance
						Boxes[k].x = Boxes[k].x - fOverlap * (Boxes[k].x - Boxes[u].x) / fDistance
						Boxes[k].y = Boxes[k].y - fOverlap * (Boxes[k].y - Boxes[u].y) / fDistance
					end
					-- end
					fDistance = math.sqrt((Boxes[k].x - Boxes[u].x)*(Boxes[k].x - Boxes[u].x) + (Boxes[k].y - Boxes[u].y)*(Boxes[k].y - Boxes[u].y))
					if fDistance ~= fDistance then fDistance = 30 end

					if Health <= 0 then
						local nx = (Boxes[u].x - Boxes[k].x) / fDistance
						if nx ~= nx then nx = 0 end
						local ny = (Boxes[u].y - Boxes[k].y) / fDistance
						if ny ~= ny then ny = 0 end
						local kx = (Boxes[k].vx - Boxes[u].vx)
						if kx ~= kx then kx = 0 end
						local ky = (Boxes[k].vy - Boxes[u].vy)
						if ky ~= ky then ky = 0 end
						local p = 2.0 * (nx * kx + ny * ky) / (Boxes[k].mass + Boxes[u].mass)
						if p ~= p then p = 0 end 
						-- print(nx,ny,kx,ky,p)
						Boxes[k].vx = Boxes[k].vx - p * Boxes[u].mass * nx
						Boxes[k].vy = Boxes[k].vy - p * Boxes[u].mass * ny
						Boxes[u].vx = Boxes[u].vx + p * Boxes[k].mass * nx
						Boxes[u].vy = Boxes[u].vy + p * Boxes[k].mass * ny
					end
				end
			end
		end

		MoveWindow(math.floor(Boxes[k].x),math.floor(Boxes[k].y))
		
		
		if os.clock() - drawthing[k] < 0.2 then
			drawthing[k] = os.clock()
			-- CaptureScreen()
			-- end
			
			-- drawthing[k] = 0
			-- print("drawing")
			
			if k == 1 then
			t = os.clock()
		end
			DisplayClear()
			GlBeginPoints()
			
			
			-- local t = os.clock()
			-- for i = 1,100 do
			for x = 0,Width do 
				for y = 0,Height do 
					local px = Ina.pixels[y * Ina.w + x]
					
					if px then
						local r,g,b,a = px.r,px.g,px.b,px.a
						SetColor(r,g,b)
						DrawPixel(x,y+1)
					end
				end
			end
			GlEnd()
			DisplayUpdate()
		end
		
	end
	-- if not drawn then drawn = true end
	lastmx = mx
	lastmy = my
	if MinFrameTime-TimeLapsed > 0 then
		Sleep((MinFrameTime-TimeLapsed)/1000)
	end
end

DestroyDisplay()