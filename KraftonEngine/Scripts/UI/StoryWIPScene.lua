DeclareProperties({
    ScenarioPath = { type = "string", default = "Asset/Content/Data/Scenarios/story_wip.scenario.json" },
})

local ScenarioLoader = require("UI.ScenarioLoader")

local SCENARIO_PATH = property("ScenarioPath", "Asset/Content/Data/Scenarios/story_wip.scenario.json")

local ui = {}
local scenario = nil
local pages = {}
local current_index = 1

local SPEAKER_COLORS = {
    BAEK_COMMANDER = { 0.62, 0.86, 1.0, 1.0 },
    LIM_COMMANDER = { 1.0, 0.77, 0.35, 1.0 },
}

local function get_component(name)
    local component = obj:GetComponent(name)
    if component and component:IsValid() then
        return component
    end

    warn("StoryWIPScene component missing:", name)
    return nil
end

local function cache_component(name)
    ui[name] = get_component(name)
end

local function set_visible(name, visible)
    local component = ui[name]
    if component then
        component:SetVisible(visible)
    end
end

local function set_text(name, text)
    local component = ui[name]
    if component then
        component:SetText(text)
    end
end

local function set_tint(name, r, g, b, a)
    local component = ui[name]
    if component then
        component:SetTint(r, g, b, a)
    end
end

local function set_texture(name, texture_path)
    local component = ui[name]
    if component and texture_path and texture_path ~= "" then
        component:SetTexture(texture_path)
    end
end

local function resolve_page_background(page)
    if not scenario or type(page) ~= "table" then
        return nil
    end

    if type(page.background) == "string" then
        local resolved = ScenarioLoader.resolve_asset(scenario, "images", page.background)
        if resolved then
            return resolved
        end
        if page.background:sub(1, 6) == "Asset/" then
            return page.background
        end
    end

    return nil
end

local function apply_page(page)
    if type(page) ~= "table" then
        return
    end

    local speaker_name = ScenarioLoader.resolve_speaker_name(scenario, page.speaker)
    local message = tostring(page.message or "")
    local background = resolve_page_background(page)
    local color = SPEAKER_COLORS[page.speaker] or { 1.0, 1.0, 1.0, 1.0 }

    if background then
        set_texture("StoryBackground", background)
    end

    set_text("SpeakerName", speaker_name)
    set_tint("SpeakerName", color[1], color[2], color[3], color[4])

    set_text("DialogueText", message)
    set_tint("DialogueText", 0.92, 0.97, 1.0, 1.0)

    set_text("PageHint", "[SPACE / CLICK] NEXT  " .. tostring(current_index) .. "/" .. tostring(#pages))
end

local function advance_page()
    if #pages == 0 then
        return
    end

    current_index = current_index + 1
    if current_index > #pages then
        current_index = 1
    end

    apply_page(pages[current_index])
end

local function load_story()
    scenario = ScenarioLoader.load(SCENARIO_PATH, load_json_file)
    if not scenario then
        warn("Failed to load story scenario:", SCENARIO_PATH)
        set_text("SpeakerName", "SYSTEM")
        set_text("DialogueText", "Scenario load failed.")
        set_text("PageHint", SCENARIO_PATH)
        return
    end

    pages = scenario.sequence or {}
    current_index = 1

    if #pages == 0 then
        set_text("SpeakerName", "SYSTEM")
        set_text("DialogueText", "No pages in scenario.")
        set_text("PageHint", SCENARIO_PATH)
        return
    end

    apply_page(pages[current_index])
end

function BeginPlay()
    cache_component("StoryBackground")
    cache_component("SpeakerName")
    cache_component("DialogueText")
    cache_component("PageHint")
    cache_component("NextPageButton")

    set_visible("StoryBackground", true)
    set_visible("SpeakerName", true)
    set_visible("DialogueText", true)
    set_visible("PageHint", true)

    load_story()
end

function Tick(dt)
    if GetKeyDown("SPACE") then
        advance_page()
        return
    end

    if ui["NextPageButton"] and ui["NextPageButton"]:WasClicked() then
        advance_page()
    end
end
