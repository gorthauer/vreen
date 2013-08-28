import qbs.base 1.0

Product {
    name: "vreenplugin"
    type: ["dynamiclibrary"]

    property string uri: "com.vk.api"
    property bool system: false

    destination: vreen.core.qmlDestination + "/" + uri.replace(/\./g, "/");

    cpp.includePaths: [
        "src"
    ]
    cpp.rpaths: ["$ORIGIN/../../../../lib"]

    files: [
        "src/audiomodel.cpp",       "src/buddymodel.h",
        "src/clientimpl.cpp",       "src/commentsmodel.h",
        "src/newsfeedmodel.cpp",    "src/wallmodel.cpp",
        "src/audiomodel.h",         "src/chatmodel.cpp",
        "src/clientimpl.h",         "src/dialogsmodel.cpp",
        "src/newsfeedmodel.h",      "src/wallmodel.h",
        "src/buddymodel.cpp",       "src/chatmodel.h",
        "src/commentsmodel.cpp",    "src/dialogsmodel.h",
        "src/vkitqmlplugin.h",      
    ]

    Depends { name: "cpp" }    
    Depends { name: "Qt.core" }    
    Depends { name: "Qt.network" }        
    Depends { name: "Qt.declarative"; condition: Qt.core.versionMajor === 4 }
    Depends { name: "Qt.quick"; condition: Qt.core.versionMajor === 5 }
    Depends { name: "vreen"}
    Depends { name: "vreenoauth" }
    Depends { name: "vreen.core" }

    Properties {
        condition: Qt.core.versionMajor === 5
        cpp.defines: base.concat("QT_DISABLE_DEPRECATED_BEFORE=0")
    }

    Group {
        qbs.installDir: destination
        qbs.install: true
        vreen.core.qmlDestination: destination
        fileTags: ["qml"]
        files: [
            "qmldir/PhotoModel.qml",    "qmldir/qmldir"
        ]
    }
    Group {
        condition: Qt.core.versionMajor === 4
        files:  "vreenplugin_qt4.cpp"
    }
    Group {
        condition: Qt.core.versionMajor === 5
        files: "vreenplugin_qt5.cpp"
    }
    Group {
        fileTagsFilter: product.type
        qbs.install: true
        qbs.installDir: product.destination
    }
}
