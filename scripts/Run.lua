require("scripts.Build")

if not (BUILD_OUT == nil)
then
    print("[Run.lua]")
    os.execute("./build/VIS")
end
