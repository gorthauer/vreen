import qbs.base 1.0
import qbs.fileinfo 1.0 as FileInfo

Product {
    name: "vreen"

    property string versionMajor:  0
    property string versionMinor: 9
    property string versionRelease: 5
    property string version: versionMajor + '.' + versionMinor + '.' + versionRelease

    destination: vreen.core.libDestination
    type: ["dynamiclibrary", "installed_content"]

    //cpp.warningLevel: "all"
    cpp.includePaths: [
        "3rdparty",
        "api"
    ]
    cpp.defines: [
        "VK_LIBRARY",
        "K8JSON_INCLUDE_GENERATOR",
        "K8JSON_INCLUDE_COMPLEX_GENERATOR"
    ]
    cpp.visibility: "hidden"

    files: [
        "api/*.cpp",
    ]

    Properties {
        condition: qt.core.versionMajor === 5
        cpp.defines: outer.concat("QT_DISABLE_DEPRECATED_BEFORE=0")
    }

    Depends { name: "cpp" }
    Depends { name: "Qt"; submodules: ["core", "network", "gui"] }
    Depends { name: "k8json" }
    Depends { name: "vreen.core" }

//    Group {
//        qbs.installDir: "include/vreen/" + version + "/vreen/private"
//        fileTags: ["install"]
//        overrideTags: false
//        files: [
//            "api/*_p.h",
//            "api/draft/*_p.h",
//            "api/draft/*.h"
//        ]
//    }
//    Group {
//        qbs.installDir: "include/vreen"
//        fileTags: ["install"]
//        overrideTags: false
//        files: [
//            "api/*[^_][a-z].h",
//        ]
//    }

    ProductModule {
        Depends { name: "cpp" }
        Depends { name: "Qt"; submodules: ["core", "network", "gui"] }
        Depends { name: "vreen.core" }

        cpp.includePaths: [
            product.buildDirectory + "/GeneratedFiles/vreen/include",
            product.buildDirectory + "/GeneratedFiles/vreen/include/vreen",
            product.buildDirectory + "/GeneratedFiles/vreen/include/vreen/" +  "0.9.5" //hack
        ]
        cpp.rpaths: product.buildDirectory + "/" + vreen.core.libDestination
    }

    Group {
        files: [
            "api/*_p.h",
            "api/draft/*_p.h",
            "api/draft/*.h"
        ]
        fileTags: ["hpp", "privdevheader"]
        overrideTags: false
    }

    Group {
        files: "api/*.h"
        excludeFiles: "api/*_p.h"
        fileTags: ["hpp", "devheader"]
        overrideTags: false
    }

    Rule {
        inputs: [ "devheader" ]
        Artifact {
            fileTags: [ "hpp" ]
            fileName: "GeneratedFiles/vreen/include/vreen/" + input.fileName
        }

        prepare: {
            var cmd = new JavaScriptCommand();
            cmd.sourceCode = function() {
                var inputFile = new TextFile(input.fileName, TextFile.ReadOnly);
                var file = new TextFile(output.fileName, TextFile.WriteOnly);
                file.truncate();
                file.write("#include \"" + input.fileName + "\"\n");
                file.close();
            }
            cmd.description = "generating " + FileInfo.fileName(output.fileName);
            cmd.highlight = "filegen";
            return cmd;
        }
    }

    Rule {
        inputs: [ "privdevheader" ]
        Artifact {
            fileTags: [ "hpp" ]
            fileName: "GeneratedFiles/vreen/include/vreen/private/" + input.fileName
        }

        prepare: {
            var cmd = new JavaScriptCommand();
            cmd.sourceCode = function() {
                var inputFile = new TextFile(input.fileName, TextFile.ReadOnly);
                var file = new TextFile(output.fileName, TextFile.WriteOnly);
                file.truncate();
                file.write("#include \"" + input.fileName + "\"\n");
                file.close();
            }
            cmd.description = "generating " + FileInfo.fileName(output.fileName);
            cmd.highlight = "filegen";
            return cmd;
        }
    }
}
