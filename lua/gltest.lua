if not CreateDisplay then require"glwrapper" end
Width = 800
Height = 600
Wh = Width / 2
Hh = Height / 2
LastRun = os.clock()
--LastTime = os.time()
function vec2(x,y) return {x=x,y=y} end
boxPos = vec2(400,300)
boxSize = vec2(70,50)
math.randomseed(os.time())
boxVel = vec2(math.random(-160,160),math.random(-160,160))
CreateDisplay(Width,Height)
EnableVSync(0)


function DrawLine(x,y,x2,y2)
	x = (x-Wh) / Wh 
	x2 = (x2-Wh) / Wh 
	y = (y-Hh) / Hh 
	y2 = (y2-Hh) / Hh 
	GlDrawLine(x,y,x2,y2)
end
function DrawRect(x,y,w,h)
	
	x = (x-Wh) / Wh
	y = (y-Hh) / Hh
	w = w / Wh
	h = h / Hh
	GlSetColor(1.0, 1.0, 1.0)
	GlDrawLine(x,y,x+w,y    ) --bottom
	GlSetColor(0.0, 0.0, 1.0)
	GlDrawLine(x,y,x,y+h    ) -- left
	GlSetColor(1.0, 0.0, 0.0)
	GlDrawLine(x,y+h,x+w,y+h) --top
	GlSetColor(0.0, 1.0, 0.0)
	GlDrawLine(x+w,y,x+w,y+h) -- right
end

while not DisplayIsClosed() do
	TimeLapsed = os.clock()-LastRun
	LastRun = os.clock()
	--if LastTime ~= os.time() then
		--print(1/TimeLapsed)
		--LastTime = os.time()
	--end
	DisplayClear()
	GlBeginLines()
	
	boxPos.x = boxPos.x + boxVel.x * TimeLapsed
	boxPos.y = boxPos.y + boxVel.y * TimeLapsed
	if boxPos.x < 0 then boxPos.x = 0 boxVel.x = boxVel.x * -1 end -- left
	if boxPos.y < 0 then boxPos.y = 0 boxVel.y = boxVel.y * -1 end -- bottom
	if boxPos.x+boxSize.x > Width then boxPos.x = Width-boxSize.x boxVel.x = boxVel.x * -1 end --right
	if boxPos.y+boxSize.y> Height then boxPos.y = Height-boxSize.y boxVel.y = boxVel.y * -1 end --top

	DrawRect(boxPos.x,boxPos.y,boxSize.x,boxSize.y)
	GlEnd()
	DisplayUpdate()
end

DestroyDisplay()