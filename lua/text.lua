if not CreateDisplay then require"glwrapper" end
local Keys = {}
local LastRun = os.clock()
local DOWN = true
local UP = false
local FPS = 240
local MinFrameTime = (1/FPS)*1000
local ScreenW,ScreenH = 1920,1080
local flags = SDL_WINDOW_OPENGL
local flags = BitOR(flags,SDL_WINDOW_ALWAYS_ON_TOP)
-- local flags = BitOR(flags,SDL_WINDOW_RESIZABLE)
local flags = BitOR(flags,SDL_WINDOW_SKIP_TASKBAR)
-- local flags = BitOR(flags,SDL_WINDOW_BORDERLESS)

local mx,my,lastmx,lastmy = 0,0,0,0
local Width = 1600
local Height = 600
local Hw = Width/2
local Hh = Height/2
local yrat = (Height/Width)
local xrat = (Width/Height)


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

function Font(name)
	local w,h,channelCount,pixels = LoadTexture(name)
	local pixelsTemp = {}
	for k,v in pairs(pixels) do
		if v.a == 255 then
			pixelsTemp[k] = {}
			-- pixelsTemp[k].x = math.floor(k%w)
			-- pixelsTemp[k].y = math.floor(k/h)
		end
	end
	pixels = pixelsTemp
	if w then
		return {w=w,h=h,channelCount=channelCount,pixels=pixels}
	end
end
MoveWindow_ = MoveWindow
function MoveWindow(x,y)
	if x == x and y == y then
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

local function DrawPixel(y,x)
	-- x = x / 2
	-- y = y /2
	y = y*yrat 
	x = Width-x*xrat - 1
	
	x = (x-Hw) / Hw 
	y = (y-Hh) / Hh 
	
	GlDraw2f(x,y)
end


local font = Font("tex/minecraftfont.png")


SetWindowPointer(1)
CreateDisplay(Width,Height,"nutsack",flags) 
EnableVSyncUnsafe(0)
-- CreateDisplay(CannonWheel.w,CannonWheel.h,"Cannon",flags) 


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
function DrawCharacter(num,xoff,yoff)
	local glyphY = math.floor(num/16)
	local glyphX = num%16
	for y = 1,16 do 
		for x = 1,16 do 
		local pxc = (((y-1)+(glyphY*16)) * font.w + ((x-1)+(glyphX*16)))
			if font.pixels[pxc] then
				DrawPixel(x+xoff,y+yoff,Width,Height)
			end
		end
	end 
end
function DrawText(str,x,y)
	str = tostring(str)
	for i = 1,#str do
		DrawCharacter(str:sub(i,i):byte(),x+i*17,y)
	end
end
fuck = 0
LastFrame = 0
while not DisplayIsClosed() do
	fuck = fuck + 1
	local Start = os.clock()
	local mx,my = GetMousePosition()
	
	DisplayClear()
	GlBeginPoints()
	local ptr = 10
	SetColor(255,0,0)
	DrawText(fuck,10,ptr) ptr = ptr + 17
	DrawText("wojox dthioadjwo awpjaw jippadwawoawoid jawoidawjdwa oi",10,ptr) ptr = ptr + 17
	DrawText("How could one man get through your forces fingers time and time again",10,ptr) ptr = ptr + 17
	DrawText("Liar — Today at 22:52 Damn bro :trollhappy:",10,ptr) ptr = ptr + 17
	DrawText(os.clock(),10,ptr) ptr = ptr + 17
	DrawText(os.time() ,10,ptr) ptr = ptr + 17
	DrawText(os.date() ,10,ptr) ptr = ptr + 17
	DrawText("function DrawCharacter(num,xoff,yoff)"	,10,ptr) ptr = ptr + 17
	DrawText("    local glyphY = math.floor(num/16)"	,10,ptr) ptr = ptr + 17
	DrawText("    local glyphX = num%16"	,10,ptr) ptr = ptr + 17
	DrawText("    for y = 1,16 do "	,10,ptr) ptr = ptr + 17
	DrawText("        for x = 1,16 do "	,10,ptr) ptr = ptr + 17
	DrawText("        local pxc = (((y-1)+(glyphY*16)) * font.w + ((x-1)+(glyphX*16)))"	,10,ptr) ptr = ptr + 17
	DrawText("        -- print(glyphY,glyphX)"	,10,ptr) ptr = ptr + 17
	DrawText("            -- local pxc = (y-1) * font.w + ((x-1))"	,10,ptr) ptr = ptr + 17
	DrawText("            if font.pixels[pxc] then"	,10,ptr) ptr = ptr + 17
	DrawText("                DrawPixel(x+xoff,y+yoff,Width,Height)"	,10,ptr) ptr = ptr + 17
	DrawText("            end"	,10,ptr) ptr = ptr + 17
	DrawText("        end"	,10,ptr) ptr = ptr + 17
	DrawText("    end "	,10,ptr) ptr = ptr + 17
	DrawText("end"	,10,ptr) ptr = ptr + 17
	DrawText("function DrawText(str,x,y)"	,10,ptr) ptr = ptr + 17
	DrawText("    for i = 1,#str do"	,10,ptr) ptr = ptr + 17
	DrawText("        DrawCharacter(str:sub(i,i):byte(),x+i*17,y)"	,10,ptr) ptr = ptr + 17
	DrawText("    end"	,10,ptr) ptr = ptr + 17
	DrawText("end"	,10,ptr) ptr = ptr + 17
	DrawText("FPS: " .. math.floor(1/LastFrame),10,ptr) ptr = ptr + 17

	GlEnd()
	DisplayUpdate()	
	
	-- lastmx = mx
	-- lastmy = my
	-- Sleep(1)
	-- Sleep(os.clock()-Start+MinFrameTime)
	LastFrame = os.clock()-Start
	-- print(LastFrame)
end

DestroyDisplay()