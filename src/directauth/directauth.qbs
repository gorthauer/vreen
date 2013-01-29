import qbs.base 1.0
import qbs.fileinfo 1.0 as FileInfo

Product {
    condition: false

    property string clientId
    property string clientSecret
    property string clientName

    type: ["staticlibrary", "installed_content"]
    name: "vreendirectauth"
    destination: "lib"

    cpp.defines: [
        "VREEN_DIRECTAUTH_CLIENT_ID=\"" + clientId + "\"",
        "VREEN_DIRECTAUTH_CLIENT_SECRET=\"" + clientSecret + "\"",
        "VREEN_DIRECTAUTH_CLIENT_NAME=\"" + clientName + "\""
    ]
    //cpp.warningLevel: "all"

    Depends { name: "cpp" }
    Depends { name: "qt"; submodules: ['core', 'network', 'webkit'] }
    Depends { name: "vreen"; }

    files: [
        "*.cpp",
        "*.h"
    ]

//    Group {
//        qbs.installDir: "include/vreen/auth"
//        overrideTags: false
//        fileTags: ["install"]
//        files: ["*.h"]
//    }

    Group {
        files: "*.h"
        fileTags: ["hpp", "devheader"]
        overrideTags: false
    }

    Rule {
        inputs: [ "devheader" ]
        Artifact {
            fileTags: [ "hpp" ]
            fileName: "GeneratedFiles/vreen/include/vreen/auth/" + input.fileName
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

