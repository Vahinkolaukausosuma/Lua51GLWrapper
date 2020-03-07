if not CreateDisplay then require"glwrapper" end
Width = 800
Height = 600
Wh = Width / 2
Hh = Height / 2
LastRun = os.clock()
--LastTime = os.time()
function Box(x,y,vx,vy,sx,sy) return {x=x,y=y,vx=vx,vy=vy,sx=sx,sy=sy} end
box = Box(400,300,math.random(-160,160),math.random(-160,160),70,50)
math.randomseed(os.time())
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
	
	box.x = box.x + box.vx * TimeLapsed
	box.y = box.y + box.vy * TimeLapsed
	if box.x < 0 then box.x = 0 box.vx = box.vx * -1 end -- left
	if box.y < 0 then box.y = 0 box.vy = box.vy * -1 end -- bottom
	if box.x+box.sx > Width then box.x = Width-box.sx box.vx = box.vx * -1 end --right
	if box.y+box.sy > Height then box.y = Height-box.sy box.vy = box.vy * -1 end --top

	DrawRect(box.x,box.y,box.sx,box.sy)
	GlEnd()
	DisplayUpdate()
end

DestroyDisplay()