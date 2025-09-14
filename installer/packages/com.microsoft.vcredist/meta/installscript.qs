function Component()
{
    // required
}

Component.prototype.createOperations = function()
{
    // Always call default
    component.createOperations();

    // Check if VC++ redistributable is installed
    if (!isVCRedistInstalled()) {
        console.log("VC++ Redistributable not found. Installing...");
        component.addOperation("Execute",
                               "@TargetDir@/vcredist_x64.exe",
                               "/install", "/quiet", "/norestart");
    } else {
        console.log("VC++ Redistributable already installed. Skipping.");
    }
}

// Helper: checks registry for vcredist x64
function isVCRedistInstalled()
{
    try {
        var regQuery = installer.execute("reg", ["query",
            "HKLM\\SOFTWARE\\Microsoft\\VisualStudio\\14.0\\VC\\Runtimes\\x64",
            "/v", "Installed"]);

        if (regQuery[0] === 0) { // exit code 0 = success
            var output = regQuery[1]; // stdout
            if (output.indexOf("0x1") !== -1 || output.indexOf("0x00000001") !== -1) {
                return true;
            }
        }
    } catch (e) {
        console.log("Registry check failed: " + e);
    }
    return false;
}