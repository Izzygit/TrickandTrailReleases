import QtQuick 2.15

Item {
    property string pkgName: "Trick and Trail"
    property string pkgDescriptionMd: "README-pkg.md"
    property string pkgLisp: "tnt.lisp"
    property string pkgQml: "ui.qml"
    property bool pkgQmlIsFullscreen: false
    property string pkgOutput: "tnt.vescpkg"

    // This function should return true when this package is compatible
    // with the connected vesc-based device
    function isCompatible (fwRxParams) {
        var hwName = fwRxParams.hw.toLowerCase();
        var fwName = fwRxParams.fwName.toLowerCase();

        // vesc, vesc bms or custom module
        // Note that VBMS32 is a custom module
        var hwType = fwRxParams.hwTypeStr().toLowerCase();		

        //console.log("HW Name: " + hwName)
        //console.log("FW Name: " + fwName)
        //console.log("HW Type: " + hwType)

        // The classic VESC BMS does not support packages at all
        if (hwType == "vesc bms") {
            return false
        }
        
        if (hwType != "vesc") {
            return false
        }

        return true
    }
}
