function MenuLayerFunc(message)
	GUI:getLayer("MenuLayer"):getWidget("MenuWidget"):AcceptMessage(message)
	WriteMessage(message:getPublisher().." - "..message:getData())
end

function MainLayerFunc(message)
	WriteMessage(message:getPublisher().." - "..message:getData())
end