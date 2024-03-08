print("[SetupMSVC.lua]")

local cmake =
{
    "cmake",
    "-G \"Visual Studio 17 2022\"",
    "-S ./",
    "-B ./build",
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
