print("[SetupDebug.lua]")

local cmake =
{
    "cmake",
    "-G \"Ninja\"",
    "-S ./",
    "-B ./build",
    "-DCMAKE_BUILD_TYPE=Debug"
}

local function merge_lines(string_array)
    string = ""

    ---@diagnostic disable-next-line: unused-local
    for i, value in ipairs(string_array) do
        string = string .. value .. " "
    end

    return string;
end

SETUP_OUT = os.execute(merge_lines(cmake))
