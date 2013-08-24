/**!
* This method is meant to be used in conjunciton with the Signature Pad jQuery
* plugin (http://thomasjbradley.ca/lab/signature-pad/). It takes in the data in
* the JSON string and converts it into an image using ImageMagick's convert
* command.
*
* Prerequisites
*     - ImageMagick must be installed with the "convert" utility
*     - Currently only works on OS X and Linux
*
* Required Parameters
*    json
*       This is the JSON string created by the Signature Pad jQuery plugin.
*
*    filePath
*       The path and file name of the image to be created. This is a normal Lasso
*       file path, so use two slashes to specify starting from the root level.
*       The type of image created will be determined by the extension.
*
* Optional Parameters
*    -imageSize
*       Takes a pair of integers to specify the width and height of the image to
*       create.
*
*       Default is pair(198, 55)
*
*    -bgColor
*       Allows you to specify the background color with an RGB hex string in the
*       form of "#FFF" or "#FFFFFF". Also accepts the value "transparent" to
*       create an image without a background color for those image types that
*       support it (such as PNG).
*
*       Default is "#FFFFFF"
*
*    -penWidth
*       Allows you to specify the thickness of the stroke of the signature.
*
*       Default is 2
*
*    -penColor
*       Allows you to specify the color of the signature with an RGB hex string
*       in the form of "#000" or "#000000". (If passed "transparent" then a black
*       pen color will be used.)
*
*       Default is "#145394"
*
*    -drawMult
*       To help smooth out the signature, the image data is drawn enlarged by
*       this factor and then scaled back down to the specified size.
*
*       Default is 12
*
*    -convertPath
*       The path to the "convert" ImageMagick executable. This value is specified
*       as a system path, so there is no need to start it with two slashes. It
*       should be an absolute path from the root level.
*
*       Default is /usr/bin/convert
*
*
* Copyright 2013 Bradley Lindsay
* 
* Licensed under the Apache License, Version 2.0 (the "License"); you may not
* use this file except in compliance with the License. You may obtain a copy of
* the License at
* 
*     http://www.apache.org/licenses/LICENSE-2.0
* 
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
* WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
* License for the specific language governing permissions and limitations under
* the License.
*/

define sig_jsonToImage(
    json::string,
    filePath::string,
    -imageSize::pair     = pair(198, 55),
    -bgColor::string     = "#FFFFFF",
    -penWidth::integer   = 2,
    -penColor::string    = "#145394",
    -drawMult::integer   = 12,
    -convertPath::string = `/usr/bin/convert`
) => {
    // Verify passed in color values are RGB Hex or "transparent"
    local(valid_color) = regexp(`(?i)^(transparent|#[0-9A-F]{3}|#[0-9A-F]{6})$`)
    
    #valid_color->input = #bgColor
    fail_if(not #valid_color->matches,
        error_code_invalidParameter, 
        error_msg_invalidParameter + ': -bgColor should be RGB Hex string in form of #FFF or #FFFFFF or "transparent"'
    )

    #valid_color->input = #penColor
    fail_if(not #valid_color->matches,
        error_code_invalidParameter, 
        error_msg_invalidParameter + ': -penColor should be RGB Hex string in form of #000 or #000000 or "transparent"'
    )

    local(img_size)   = #imageSize->first + 'x' + #imageSize->second
    local(img_sizeLg) = (#imageSize->first * #drawMult) + 'x' + (#imageSize->second * #drawMult)

    local(instructions) = (
        with item in json_deserialize(#json)
        select "line " + (#item('mx') * #drawMult) + "," + (#item('my') * #drawMult) + " " + (#item('lx') * #drawMult) + "," + (#item('ly') * #drawMult)
    )->join(' ')

    local(proc) = sys_process(#convertPath, (:
        "-size"         , #img_sizeLG,
        "xc:" + #bgColor,
        "-stroke"       , #penColor,
        "-strokewidth"  , (#penWidth * #drawMult) / 2,
        "-draw"         , "@-",
        "-resize"       , #img_size,
        file_forceRoot(#filePath)
    ))
    handle => { #proc->close }

    #proc->write(#instructions)&closeWrite
    local(exit) = #proc->wait
    local(err)  = #proc->readError

    #exit != 0 or #err != void
        ? fail(`Error generating signature: ` + #err)
}