DeclareProperties({
    ScenarioPath = { type = "string", default = "Asset/Content/Data/Scenarios/story_wip.scenario.json" },
})

local ScenarioLoader = require("UI.ScenarioLoader")

local SCENARIO_PATH = property("ScenarioPath", "Asset/Content/Data/Scenarios/story_wip.scenario.json")

local ui = {}
local scenario = nil
local pages = {}
local current_index = 1
local input_cooldown = 0.0

local MAX_LINE_WIDTH_UNITS = 28
local SPEAKER_NAME_COLOR = { 134.0 / 255.0, 251.0 / 255.0, 255.0 / 255.0, 233.0 / 255.0 }
local DIALOGUE_TEXT_COLOR = { 235.0 / 255.0, 247.0 / 255.0, 255.0 / 255.0, 209.0 / 255.0 }
local INPUT_COOLDOWN_SECONDS = 0.18

local function format_terminal_message(text)
    local source = type(text) == "string" and text or ""
    if source == "" then
        return ">_"
    end

    local function split_utf8_chars(value)
        local chars = {}
        local index = 1
        local length = #value

        while index <= length do
            local byte = string.byte(value, index)
            local char_length = 1

            if byte >= 0xF0 then
                char_length = 4
            elseif byte >= 0xE0 then
                char_length = 3
            elseif byte >= 0xC0 then
                char_length = 2
            end

            chars[#chars + 1] = value:sub(index, index + char_length - 1)
            index = index + char_length
        end

        return chars
    end

    local function normalize_newlines(value)
        local result = {}
        local index = 1
        local length = #value

        while index <= length do
            local char = value:sub(index, index)
            if char == "\r" then
                result[#result + 1] = "\n"
                if index < length and value:sub(index + 1, index + 1) == "\n" then
                    index = index + 1
                end
            else
                result[#result + 1] = char
            end
            index = index + 1
        end

        return table.concat(result)
    end

    local function is_ascii_space(char)
        return char == " " or char == "\t"
    end

    local function char_width_units(char)
        local byte = string.byte(char, 1)
        if not byte then
            return 0
        end

        if byte < 0x80 then
            if is_ascii_space(char) then
                return 0.45
            end
            return 0.6
        end

        return 1.0
    end

    local function trim_spaces(value)
        local chars = split_utf8_chars(value)
        local start_index = 1
        local end_index = #chars

        while start_index <= end_index and is_ascii_space(chars[start_index]) do
            start_index = start_index + 1
        end

        while end_index >= start_index and is_ascii_space(chars[end_index]) do
            end_index = end_index - 1
        end

        if start_index > end_index then
            return ""
        end

        return table.concat(chars, "", start_index, end_index)
    end

    local function wrap_line(line)
        local source = trim_spaces(line)
        if source == "" then
            return { "" }
        end

        local chars = split_utf8_chars(source)
        local lines = {}
        local current = {}
        local last_space_index = nil
        local current_width = 0.0

        local function current_text(count)
            return table.concat(current, "", 1, count or #current)
        end

        local function push_current(text)
            lines[#lines + 1] = trim_spaces(text)
            current = {}
            last_space_index = nil
            current_width = 0.0
        end

        for _, char in ipairs(chars) do
            current[#current + 1] = char
            current_width = current_width + char_width_units(char)
            if char == " " then
                last_space_index = #current
            end

            if current_width >= MAX_LINE_WIDTH_UNITS then
                if last_space_index and last_space_index > 1 then
                    local split_index = last_space_index
                    local snapshot = current
                    push_current(table.concat(snapshot, "", 1, split_index - 1))

                    local remaining = {}
                    for index = split_index + 1, #snapshot do
                        remaining[#remaining + 1] = snapshot[index]
                    end
                    current = remaining
                    current_width = 0.0
                    last_space_index = nil

                    for index, remaining_char in ipairs(current) do
                        current_width = current_width + char_width_units(remaining_char)
                        if remaining_char == " " then
                            last_space_index = index
                        end
                    end
                else
                    push_current(current_text())
                end
            end
        end

        if #current > 0 then
            push_current(current_text())
        end

        return lines
    end

    local normalized = normalize_newlines(source)
    local wrapped_lines = {}
    local current_line = {}

    local function flush_line()
        local raw_line = table.concat(current_line)
        current_line = {}

        if raw_line == "" and #wrapped_lines > 0 then
            wrapped_lines[#wrapped_lines + 1] = ""
        elseif raw_line ~= "" then
            local line_parts = wrap_line(raw_line)
            for _, line in ipairs(line_parts) do
                wrapped_lines[#wrapped_lines + 1] = line
            end
        end
    end

    local index = 1
    while index <= #normalized do
        local char = normalized:sub(index, index)
        if char == "\n" then
            flush_line()
        else
            current_line[#current_line + 1] = char
        end
        index = index + 1
    end

    if #current_line > 0 then
        flush_line()
    end

    if #wrapped_lines == 0 then
        return ">_"
    end

    local message = table.concat(wrapped_lines, "\n")
    if message == "" then
        return ">_"
    end

    local continuation_indent = "   "
    local indented_message = message:gsub("\n", "\n" .. continuation_indent)
    return ">_ " .. indented_message
end

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
    local message = format_terminal_message(tostring(page.message or ""))
    local background = resolve_page_background(page)

    if background then
        set_texture("StoryBackground", background)
    end

    set_text("SpeakerName", speaker_name)
    set_tint("SpeakerName", SPEAKER_NAME_COLOR[1], SPEAKER_NAME_COLOR[2], SPEAKER_NAME_COLOR[3], SPEAKER_NAME_COLOR[4])

    set_text("DialogueText", message)
    set_tint("DialogueText", DIALOGUE_TEXT_COLOR[1], DIALOGUE_TEXT_COLOR[2], DIALOGUE_TEXT_COLOR[3], DIALOGUE_TEXT_COLOR[4])

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

local function try_advance_page()
    if input_cooldown > 0.0 then
        return false
    end

    input_cooldown = INPUT_COOLDOWN_SECONDS
    advance_page()
    return true
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
    if input_cooldown > 0.0 then
        input_cooldown = math.max(0.0, input_cooldown - (dt or 0.0))
    end

    if GetKeyDown("SPACE") then
        try_advance_page()
        return
    end

    if ui["NextPageButton"] and ui["NextPageButton"]:WasClicked() then
        try_advance_page()
    end
end
