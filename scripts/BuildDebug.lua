require("scripts.SetupDebug")

if not (SETUP_OUT == nil)
then
    print("[BuildDebug.lua]")
    BUILD_OUT = os.execute("cmake --build ./build")
end
