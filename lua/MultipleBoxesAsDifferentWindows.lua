if not CreateDisplay then require"glwrapper" end
if not sleep then require"socket" sleep = socket.sleep end
Width = 25
Height = 25
Hh = Height/2
Wh = Width/2
MinFrameTime = 1000/75
ScreenW,ScreenH = 1920,1080
flags = SDL_WINDOW_OPENGL
flags = BitOR(flags,SDL_WINDOW_ALWAYS_ON_TOP)
flags = BitOR(flags,SDL_WINDOW_RESIZABLE)
flags = BitOR(flags,SDL_WINDOW_SKIP_TASKBAR)
flags = BitOR(flags,SDL_WINDOW_BORDERLESS)
BoxCount = 9
function Box(x,y,vx,vy,sx,sy,r,g,b) return {x=x,y=y,vx=vx,vy=vy,sx=sx,sy=sy,r=r,g=g,b=b} end
Boxes = {
	Box(math.random(1,ScreenW),math.random(1,ScreenH),math.random(-350,350),math.random(-350,350),25,25,math.random(0,255)/255,math.random(0,255)/255,math.random(0,255)/255),
	Box(math.random(1,ScreenW),math.random(1,ScreenH),math.random(-350,350),math.random(-350,350),25,25,math.random(0,255)/255,math.random(0,255)/255,math.random(0,255)/255),
	Box(math.random(1,ScreenW),math.random(1,ScreenH),math.random(-350,350),math.random(-350,350),25,25,math.random(0,255)/255,math.random(0,255)/255,math.random(0,255)/255),
	Box(math.random(1,ScreenW),math.random(1,ScreenH),math.random(-350,350),math.random(-350,350),25,25,math.random(0,255)/255,math.random(0,255)/255,math.random(0,255)/255),
	Box(math.random(1,ScreenW),math.random(1,ScreenH),math.random(-350,350),math.random(-350,350),25,25,math.random(0,255)/255,math.random(0,255)/255,math.random(0,255)/255),
	Box(math.random(1,ScreenW),math.random(1,ScreenH),math.random(-350,350),math.random(-350,350),25,25,math.random(0,255)/255,math.random(0,255)/255,math.random(0,255)/255),
	Box(math.random(1,ScreenW),math.random(1,ScreenH),math.random(-350,350),math.random(-350,350),25,25,math.random(0,255)/255,math.random(0,255)/255,math.random(0,255)/255),
	Box(math.random(1,ScreenW),math.random(1,ScreenH),math.random(-350,350),math.random(-350,350),25,25,math.random(0,255)/255,math.random(0,255)/255,math.random(0,255)/255),
	Box(math.random(1,ScreenW),math.random(1,ScreenH),math.random(-350,350),math.random(-350,350),25,25,math.random(0,255)/255,math.random(0,255)/255,math.random(0,255)/255)
}

function DrawLine(x,y,x2,y2)
	x = (x-Wh) / Wh 
	x2 = (x2-Wh) / Wh 
	y = (y-Hh) / Hh 
	y2 = (y2-Hh) / Hh 
	GlDrawLine(x,y,x2,y2)
end

for i = 1,BoxCount do
	SetWindowPointer(i)
	CreateDisplay(Width,Height,"Nuts "..tostring(i),flags)
end

LastRun = os.clock()
while not DisplayIsClosed() do
	TimeLapsed = os.clock()-LastRun
	LastRun = os.clock()
	for k = 1,BoxCount do
		SetWindowPointer(k)
		Boxes[k].x = Boxes[k].x + Boxes[k].vx * TimeLapsed
		Boxes[k].y = Boxes[k].y + Boxes[k].vy * TimeLapsed
		if Boxes[k].x < 0 then Boxes[k].x = 0 Boxes[k].vx = Boxes[k].vx * -1 end -- left
		if Boxes[k].y < 0 then Boxes[k].y = 0 Boxes[k].vy = Boxes[k].vy * -1 end -- bottom
		if Boxes[k].x+Boxes[k].sx > 1920 then Boxes[k].x = 1920-Boxes[k].sx Boxes[k].vx = Boxes[k].vx * -1 end --right
		if Boxes[k].y+Boxes[k].sy > 1080 then Boxes[k].y = 1080-Boxes[k].sy Boxes[k].vy = Boxes[k].vy * -1 end --top
		MoveWindow(Boxes[k].x,Boxes[k].y)
		DisplayClear()
		GlBeginLines()
		GlSetColor(Boxes[k].r,Boxes[k].g,Boxes[k].b)

		for x = 0,Width do 
			for y = 0,Height do 
				DrawLine(x,y,x+1,y+1)
			end
		end
		GlEnd()
		DisplayUpdate()
	end
	
	if MinFrameTime-TimeLapsed > 0 then
		sleep((MinFrameTime-TimeLapsed)/1000)
	end
end

DestroyDisplay()