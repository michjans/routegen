function Controller()
{
}

Controller.prototype.IntroductionPageCallback = function()
{

    var widget = gui.currentPageWidget();
    if (widget != null) {
        widget.title = "Setup"; 
        widget.MessageLabel.setText("This tool will install Route Generator on your computer.\n It is recommended to first uninstall any existing verions of Route Generator!"); 
    }
}