require"glwrapper.core"

function CreateDisplay(w,h)
	local w = w or 800
	local h = h or 600
	
	CreateDisplayUnsafe(w,h)	
end

function EnableVSync(bool)
	local bool = bool or 1
	
	EnableVSyncUnsafe(bool)
end

function GlSetColor(r,g,b)
	local r = r or 1.0
	local g = g or 1.0
	local b = b or 1.0
	
	GlSetColorUnsafe(r,g,b)	
end

function GlDrawLine(x,y,x2,y2)
	if x and y and x2 and y2 then
		GlDrawLineUnsafe(x,y,x2,y2)	
	end
end