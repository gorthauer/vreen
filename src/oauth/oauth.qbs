import qbs.base 1.0
import qbs.fileinfo as FileInfo

Product {
    type: ["staticlibrary"]
    name: "vreenoauth"
    destination: "lib"

    //cpp.warningLevel: "all"

    Depends { name: "cpp" }
    Depends { name: "Qt"; submodules: ['core', 'network', 'widgets', 'webkit', 'webkitwidgets'] }
    Depends { name: "vreen" }
    Depends { name: "vreen.core" }

    files: [
        "*.cpp"
    ]

    Properties {
        condition: Qt.core.versionMajor === 5
        cpp.defines: base.concat("QT_DISABLE_DEPRECATED_BEFORE=0")
    }

    ProductModule {
        Depends { name: "cpp" }
        Depends { name: "Qt.webkit"}
        Depends { name: "Qt.webkitwidgets"}
        
        cpp.defines: base.concat("VREEN_WITH_OAUTH")
    }

    Group {
        files: "*.h"
        fileTags: ["hpp", "devheader"]
        overrideTags: false
        qbs.installDir: "include/vreen/auth"
        qbs.install: true
    }
}

