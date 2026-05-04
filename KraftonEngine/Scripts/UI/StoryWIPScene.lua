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
local auto_advance_timer = nil
local pending_page_index = nil
local dialogue_background_state = {
    current_one_key = nil,
}
local OVERLAY_COMPONENTS = {
    control_room_baek_0 = "StoryBackgroundBaek0",
    control_room_baek_1 = "StoryBackgroundBaek1",
    control_room_lim_0 = "StoryBackgroundLim0",
    control_room_lim_1 = "StoryBackgroundLim1",
    control_room_pod = "StoryBackgroundPod",
}
local dialogue_transition = {
    active = false,
    elapsed = 0.0,
    duration = 0.0,
    mode = nil,
    zero_key = nil,
    zero_path = nil,
    one_key = nil,
    one_path = nil,
}
local pending_dialogue_ui = nil

local MAX_LINE_WIDTH_UNITS = 28
local SPEAKER_NAME_COLOR = { 134.0 / 255.0, 251.0 / 255.0, 255.0 / 255.0, 233.0 / 255.0 }
local DIALOGUE_TEXT_COLOR = { 235.0 / 255.0, 247.0 / 255.0, 255.0 / 255.0, 209.0 / 255.0 }
local INPUT_COOLDOWN_SECONDS = 0.18
local apply_page = nil
local resolve_neutral_background = nil

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

local function set_default_background()
    local _, neutral_path = resolve_neutral_background()
    if neutral_path then
        set_texture("StoryBackground", neutral_path)
    end
    set_tint("StoryBackground", 1.0, 1.0, 1.0, 1.0)
end

local function set_overlay_alpha(background_key, alpha)
    local component_name = OVERLAY_COMPONENTS[background_key]
    if component_name then
        set_visible(component_name, true)
        set_tint(component_name, 1.0, 1.0, 1.0, alpha or 0.0)
    end
end

local function hide_dialogue_background_layers()
    for background_key, component_name in pairs(OVERLAY_COMPONENTS) do
        set_visible(component_name, true)
        set_tint(component_name, 1.0, 1.0, 1.0, 0.0)
    end
end

local function reset_dialogue_transition()
    dialogue_transition.active = false
    dialogue_transition.elapsed = 0.0
    dialogue_transition.mode = nil
    dialogue_transition.zero_key = nil
    dialogue_transition.zero_path = nil
    dialogue_transition.one_key = nil
    dialogue_transition.one_path = nil
end

local function clear_pending_dialogue_ui()
    pending_dialogue_ui = nil
end

local function hide_dialogue_ui()
    set_text("SpeakerName", "")
    set_text("DialogueText", "")
    set_text("PageHint", "")
    set_visible("SpeakerName", false)
    set_visible("DialogueText", false)
    set_visible("PageHint", false)
end

local function show_dialogue_ui(page)
    local speaker_name = ScenarioLoader.resolve_speaker_name(scenario, page.speaker)
    local message = format_terminal_message(tostring(page.message or ""))

    set_visible("SpeakerName", true)
    set_visible("DialogueText", true)
    set_visible("PageHint", true)
    set_text("SpeakerName", speaker_name)
    set_tint("SpeakerName", SPEAKER_NAME_COLOR[1], SPEAKER_NAME_COLOR[2], SPEAKER_NAME_COLOR[3], SPEAKER_NAME_COLOR[4])
    set_text("DialogueText", message)
    set_tint("DialogueText", DIALOGUE_TEXT_COLOR[1], DIALOGUE_TEXT_COLOR[2], DIALOGUE_TEXT_COLOR[3], DIALOGUE_TEXT_COLOR[4])
    set_text("PageHint", "[SPACE / CLICK] NEXT  " .. tostring(current_index) .. "/" .. tostring(#pages))
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

resolve_neutral_background = function()
    if not scenario then
        return nil, nil
    end

    local neutral_key = "control_room_on"
    local neutral_path = ScenarioLoader.resolve_asset(scenario, "images", neutral_key)
    return neutral_key, neutral_path
end

local function resolve_fade_source_background(page)
    if not scenario or type(page) ~= "table" or type(page.background) ~= "string" then
        return nil, nil
    end

    local background_key = page.background
    if background_key:sub(-2) ~= "_1" then
        return nil, nil
    end

    local base_key = background_key:sub(1, -3) .. "_0"
    local base_path = ScenarioLoader.resolve_asset(scenario, "images", base_key)
    if not base_path then
        return nil, nil
    end

    return base_key, base_path
end

local function complete_dialogue_transition()
    local completed_mode = dialogue_transition.mode
    if completed_mode == "enter" then
        set_overlay_alpha(dialogue_transition.zero_key, 1.0)
        set_overlay_alpha(dialogue_transition.one_key, 1.0)
        dialogue_background_state.current_one_key = dialogue_transition.one_key
        show_dialogue_ui(pending_dialogue_ui)
        clear_pending_dialogue_ui()
    elseif completed_mode == "exit" then
        hide_dialogue_background_layers()
        dialogue_background_state.current_one_key = nil
    end

    reset_dialogue_transition()

    if completed_mode == "exit" and pending_page_index then
        current_index = pending_page_index
        pending_page_index = nil
        apply_page(pages[current_index])
    end
end

local function set_background_immediately(background_key, background_path)
    if not background_path then
        return
    end

    reset_dialogue_transition()
    clear_pending_dialogue_ui()
    set_default_background()
    hide_dialogue_background_layers()
    local zero_key = background_key
    local zero_path = background_path
    local one_key = nil
    local one_path = nil

    if type(background_key) == "string" and background_key:sub(-2) == "_1" then
        zero_key, zero_path = resolve_fade_source_background({ background = background_key })
        one_key = background_key
        one_path = background_path
    end

    if zero_key then
        set_overlay_alpha(zero_key, 1.0)
    end

    if one_key then
        set_overlay_alpha(one_key, 1.0)
    end

    dialogue_background_state.current_one_key = background_key
end

local function set_non_dialogue_background(background_path)
    reset_dialogue_transition()
    clear_pending_dialogue_ui()
    set_default_background()
    hide_dialogue_background_layers()
    if type(background_path) == "string" then
        for background_key, component_name in pairs(OVERLAY_COMPONENTS) do
            local resolved = ScenarioLoader.resolve_asset(scenario, "images", background_key)
            if resolved == background_path then
                set_overlay_alpha(background_key, 1.0)
                break
            end
        end
    end
    set_visible("StoryBackground", true)
    set_tint("StoryBackground", 1.0, 1.0, 1.0, 1.0)
    dialogue_background_state.current_one_key = nil
end

local function begin_dialogue_transition(mode, zero_key, zero_path, one_key, one_path)
    if not zero_path or not one_path then
        if mode == "enter" then
            set_background_immediately(one_key, one_path)
        else
            set_non_dialogue_background(select(2, resolve_neutral_background()))
        end
        return
    end

    set_default_background()
    hide_dialogue_background_layers()
    if zero_key then
        set_overlay_alpha(zero_key, 1.0)
    end

    if mode == "enter" then
        if one_key then
            set_overlay_alpha(one_key, 1.0)
        end
        dialogue_background_state.current_one_key = one_key
        show_dialogue_ui(pending_dialogue_ui)
        clear_pending_dialogue_ui()
    else
        if one_key then
            set_overlay_alpha(one_key, 0.0)
        end
        dialogue_background_state.current_one_key = nil
        if pending_page_index then
            current_index = pending_page_index
            pending_page_index = nil
            apply_page(pages[current_index])
        end
    end
    reset_dialogue_transition()
end

local function is_transition_dialogue_page(page)
    return type(page) == "table"
        and page.type == "dialogue"
        and type(page.background) == "string"
        and page.background:sub(-2) == "_1"
end

local function start_dialogue_intro_transition(page, target_path)
    local base_key, base_path = resolve_fade_source_background(page)
    if not base_path or not target_path then
        set_background_immediately(page.background, target_path)
        return
    end

    begin_dialogue_transition("enter", base_key, base_path, page.background, target_path)
end

local function start_dialogue_exit_transition(page)
    local base_key, base_path = resolve_fade_source_background(page)
    local one_path = resolve_page_background(page)
    if not one_path or not base_path then
        set_non_dialogue_background(select(2, resolve_neutral_background()))
        if pending_page_index then
            current_index = pending_page_index
            pending_page_index = nil
            apply_page(pages[current_index])
        end
        return
    end

    begin_dialogue_transition("exit", base_key, base_path, page.background, one_path)
end

local function is_auto_page(page)
    return type(page) == "table"
        and (page.type == "image" or page.type == "splash")
        and type(page.duration) == "number"
        and page.duration > 0.0
end

apply_page = function(page)
    if type(page) ~= "table" then
        return
    end

    local background = resolve_page_background(page)
    local auto_page = is_auto_page(page)
    local should_delay_dialogue_ui = is_transition_dialogue_page(page)

    if background then
        if should_delay_dialogue_ui and dialogue_background_state.current_one_key ~= page.background then
            pending_dialogue_ui = page
            hide_dialogue_ui()
            start_dialogue_intro_transition(page, background)
            return
        elseif should_delay_dialogue_ui then
            set_background_immediately(page.background, background)
        else
            set_non_dialogue_background(background)
        end
    end

    if auto_page then
        clear_pending_dialogue_ui()
        auto_advance_timer = page.duration
        set_text("SpeakerName", "")
        set_text("DialogueText", "")
        set_text("PageHint", "")
        set_visible("SpeakerName", false)
        set_visible("DialogueText", false)
        set_visible("PageHint", false)
        return
    end

    auto_advance_timer = nil

    clear_pending_dialogue_ui()
    show_dialogue_ui(page)
end

local function advance_page()
    if #pages == 0 then
        return
    end

    local next_index = current_index + 1
    if next_index > #pages then
        next_index = 1
    end

    local current_page = pages[current_index]
    if is_transition_dialogue_page(current_page) then
        pending_page_index = next_index
        hide_dialogue_ui()
        start_dialogue_exit_transition(current_page)
        return
    end

    current_index = next_index
    apply_page(pages[current_index])
end

local function try_advance_page()
    if input_cooldown > 0.0 or dialogue_transition.active or pending_page_index then
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
    cache_component("StoryBackgroundBaek0")
    cache_component("StoryBackgroundBaek1")
    cache_component("StoryBackgroundLim0")
    cache_component("StoryBackgroundLim1")
    cache_component("StoryBackgroundPod")
    cache_component("SpeakerName")
    cache_component("DialogueText")
    cache_component("PageHint")
    cache_component("NextPageButton")

    set_visible("StoryBackground", true)
    set_default_background()
    hide_dialogue_background_layers()
    set_visible("SpeakerName", true)
    set_visible("DialogueText", true)
    set_visible("PageHint", true)

    load_story()
end

function Tick(dt)
    if dialogue_transition.active then
        dialogue_transition.elapsed = dialogue_transition.elapsed + (dt or 0.0)
        local alpha = 1.0
        if dialogue_transition.duration > 0.0 then
            alpha = math.min(dialogue_transition.elapsed / dialogue_transition.duration, 1.0)
        end

        if dialogue_transition.mode == "enter" then
            set_overlay_alpha(dialogue_transition.one_key, alpha)
        else
            set_overlay_alpha(dialogue_transition.one_key, 1.0 - alpha)
        end

        if alpha >= 1.0 then
            complete_dialogue_transition()
        end
    end

    if auto_advance_timer then
        auto_advance_timer = auto_advance_timer - (dt or 0.0)
        if auto_advance_timer <= 0.0 then
            auto_advance_timer = nil
            advance_page()
            return
        end
    end

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
