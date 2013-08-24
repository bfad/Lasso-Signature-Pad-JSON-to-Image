local(path_here) = currentCapture->callsite_file->stripLastComponent
not #path_here->beginsWith('/')? #path_here = io_file_getcwd + '/' + #path_here
not #path_here->endsWith('/') ? #path_here += '/'
sourcefile(file(#path_here + '../sig_jsonToImage.inc'), -autoCollect=false)->invoke

describe(::sig_jsonToImage) => {
    local(json_data) = json_serialize(array(
        map("lx"=20,"ly"=34,"mx"=20,"my"=34),
        map("lx"=21,"ly"=33,"mx"=20,"my"=34),
        map("lx"=22,"ly"=31,"mx"=21,"my"=33),
        map("lx"=23,"ly"=30,"mx"=22,"my"=31)
    ))
    local(file_path) = '//tmp/sig.png'

    afterEach => {
        protect => { file(#file_path)->delete }
    }

    context(`Testing color values passed to -bgColor`) =>  {
        it(`succeeds when the value is "transparent"`) => {
            expect->errorCode(error_code_noerror) => {
                sig_jsonToImage(#json_data, #file_path, -bgColor="transparent")
            }
        }
        it(`succeeds when the value is a 3 digit hex string`) => {
            expect->errorCode(error_code_noerror) => {
                sig_jsonToImage(#json_data, #file_path, -bgColor="#FFF")
            }
        }
        it(`succeeds when the value is a 6 digit hex string`) => {
            expect->errorCode(error_code_noerror) => {
                sig_jsonToImage(#json_data, #file_path, -bgColor="#FFFFFF")
            }
        }

        it(`throws the proper error when hex string doesn't have 3 or 6 digits`) => {
            expect->error(
                error_code_invalidParameter,
                error_msg_invalidParameter + ': -bgColor should be RGB Hex string in form of #FFF or #FFFFFF or "transparent"'
            ) => {
                sig_jsonToImage(#json_data, #file_path, -bgColor="#FF")
            }
            expect->error(
                error_code_invalidParameter,
                error_msg_invalidParameter + ': -bgColor should be RGB Hex string in form of #FFF or #FFFFFF or "transparent"'
            ) => {
                sig_jsonToImage(#json_data, #file_path, -bgColor="#FFFF")
            }
            expect->error(
                error_code_invalidParameter,
                error_msg_invalidParameter + ': -bgColor should be RGB Hex string in form of #FFF or #FFFFFF or "transparent"'
            ) => {
                sig_jsonToImage(#json_data, #file_path, -bgColor="#FFFFFFF")
            }
        }
        it(`throws the proper error when not a hex string or "transparent"`) => {
            expect->error(
                error_code_invalidParameter,
                error_msg_invalidParameter + ': -bgColor should be RGB Hex string in form of #FFF or #FFFFFF or "transparent"'
            ) => {
                sig_jsonToImage(#json_data, #file_path, -bgColor="FFFFFF")
            }
        }
    }


    context(`Testing color values passed to -penColor`) =>  {
        it(`succeeds when the value is "transparent"`) => {
            expect->errorCode(error_code_noerror) => {
                sig_jsonToImage(#json_data, #file_path, -penColor="transparent")
            }
        }
        it(`succeeds when the value is a 3 digit hex string`) => {
            expect->errorCode(error_code_noerror) => {
                sig_jsonToImage(#json_data, #file_path, -penColor="#000")
            }
        }
        it(`succeeds when the value is a 6 digit hex string`) => {
            expect->errorCode(error_code_noerror) => {
                sig_jsonToImage(#json_data, #file_path, -penColor="#000000")
            }
        }

        it(`throws the proper error when hex string doesn't have 3 or 6 digits`) => {
            expect->error(
                error_code_invalidParameter,
                error_msg_invalidParameter + ': -penColor should be RGB Hex string in form of #000 or #000000 or "transparent"'
            ) => {
                sig_jsonToImage(#json_data, #file_path, -penColor="#00")
            }
            expect->error(
                error_code_invalidParameter,
                error_msg_invalidParameter + ': -penColor should be RGB Hex string in form of #000 or #000000 or "transparent"'
            ) => {
                sig_jsonToImage(#json_data, #file_path, -penColor="#0000")
            }
            expect->error(
                error_code_invalidParameter,
                error_msg_invalidParameter + ': -penColor should be RGB Hex string in form of #000 or #000000 or "transparent"'
            ) => {
                sig_jsonToImage(#json_data, #file_path, -penColor="#0000000")
            }
        }
        it(`throws the proper error when not a hex string or "transparent"`) => {
            expect->error(
                error_code_invalidParameter,
                error_msg_invalidParameter + ': -penColor should be RGB Hex string in form of #000 or #000000 or "transparent"'
            ) => {
                sig_jsonToImage(#json_data, #file_path, -penColor="000000")
            }
        }
    }


    it(`creates a file at the given path`) => {
        local(sig_file) = file(#file_path)
        expect(not #sig_file->exists)

        sig_jsonToImage(#json_data, #file_path)

        expect(#sig_file->exists)
    }
}