function Component()
{
    // default constructor
}

Component.prototype.createOperations = function()
{
    // call default implementation
    component.createOperations();

    if (systemInfo.productType === "windows") {
        component.addOperation("CreateShortcut", "@TargetDir@/routegen.exe", "@StartMenuDir@/Route Generator.lnk",
            "workingDirectory=@TargetDir@", "iconPath=@TargetDir@/routegen.ico",
            "description=Start Route Generator");
    }
}