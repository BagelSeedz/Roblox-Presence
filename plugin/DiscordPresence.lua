-- Author: Dave (@Bagel_Seedz)

-- [[ DEPENDENCIES ]] --

local HTTP = game:GetService("HttpService")
local SES = game:GetService("ScriptEditorService")
local AS = game:GetService("AssetService")
local RS = game:GetService("RunService")

-- [[ CONSTANTS ]] --

local LOCAL_SERVER = "http://localhost:3000/updateRobloxPresence"
local SMALL_IMAGE = "roblox-studio-icon-filled-256"
local SMALL_TEXT = "Roblox Studio"
local LARGE_IMAGES = {
	["Script"] = "script_light_icon",
	["LocalScript"] = "localscript_light_icon",
	["ModuleScript"] = "modulescript_light_icon",
	["Idling"] = "roblox-presence-icon"
}
local IDLE_BODY = {
	["state"] = "Place: Place1",
	["details"] = "Idling",
	["largeImage"] = LARGE_IMAGES.Idling,
	["largeText"] = "Idling",
	["smallImage"] = SMALL_IMAGE,
	["smallText"] = SMALL_TEXT,
	["start"] = tostring(os.time())
}

-- [[ VARIABLES ]] --

local active = false -- Is the presence active?
local timeActivated = 0 -- The tick value when the presence was last activated.
local connections = {} -- Connections made to be closed upon deactivation.

-- The POST request body to be sent to the presence app.
-- Updates periodically.
local body = table.clone(IDLE_BODY)

-- [[ FUNCTIONS ]] --

local function getPlaceName()
	--[[
		Returns the name of the current place being edited.
		If the place is not published, returns "Place1".
	]]
	local placeId = game.PlaceId
	
	local placePages = AS:GetGamePlacesAsync()
	
	while true do
		for _, place in placePages:GetCurrentPage() do
			if place.PlaceId == game.PlaceId then
				return place.Name
			end
		end
		if placePages.IsFinished then
			break
		end
		placePages:AdvanceToNextPageAsync()
	end
	
	return "Place1"
end

local function getNextDoc()
	--[[
		Returns the next ScriptDocument in the list of
		open ScriptDocuments. Nil of none found. 
		Intended to be used upon closing a document.
	]]
	for _, doc in SES:GetScriptDocuments() do
		if not doc:IsCommandBar() then
			return doc
		end
	end
end

local function getLargeFromDoc(doc: ScriptDocument)
	--[[
		Returns the corresponding largeImage and largeText
		for a given ScriptDocument based on its ClassName.
	]]
	local docScript = doc:GetScript()
	return LARGE_IMAGES[docScript.ClassName], "Editing a " .. docScript.ClassName
end

local function updateBody(doc: ScriptDocument)
	--[[
		Updates the global body table based on the passed
		ScriptDocument. Does not send the request.
		If nil is passed, IDLE_BODY is used by default.
	]]
	table.clear(body)
	
	if doc then
		local largeImage, largeText = getLargeFromDoc(doc)
		
		body = {
			["state"] = "Place: " .. getPlaceName(),
			["details"] = "Editing " .. doc:GetScript().Name,
			["largeImage"] = largeImage,
			["largeText"] = largeText,
			['smallImage'] = SMALL_IMAGE,
			['smallText'] = SMALL_TEXT,
			['start'] = tostring(timeActivated)
		}
	else
		body = table.clone(IDLE_BODY)
		body.state = "Place: " .. getPlaceName()
		body.start = tostring(timeActivated)
	end
end

local function activate()
	-- Set up document connections
	table.insert(connections, SES.TextDocumentDidOpen:Connect(updateBody))
	table.insert(connections, SES.TextDocumentDidChange:Connect(updateBody))
	table.insert(connections, SES.TextDocumentDidClose:Connect(function()
		local doc = getNextDoc() -- Can be nil
		updateBody(doc)
	end))
	
	-- POST every second
	local deltaTime = 0
	table.insert(connections, RS.Heartbeat:Connect(function(delta)
		deltaTime += delta
		if deltaTime < 1 then return end
		deltaTime = 0
		
		local data = HTTP:JSONEncode(body)
		
		local success, err = pcall(function()
			if body and body.state and body.details then
				HTTP:PostAsync(LOCAL_SERVER, data, Enum.HttpContentType.ApplicationJson)
			end
		end)
	end))
end

local function deactivate()
	-- Disconnect all active connections
	for _, connection: RBXScriptConnection in connections do
		connection:Disconnect()
	end
end

local function onButtonClicked()
	-- Toggle the presence on and off
	active = not active
	if active then 
		timeActivated = os.time()
		activate()
		print("Discord Presence Activated!")
	else 
		deactivate()
		print("Discord Presence Deactivated.")
	end
end

-- Connect the plugin to the toolbar
local toolbar = plugin:CreateToolbar("Discord Presence")
local button = toolbar:CreateButton("Activate Presence", "Activates Roblox-to-Discord Presence", "rbxassetid://91060618801142")
button.Click:Connect(onButtonClicked)