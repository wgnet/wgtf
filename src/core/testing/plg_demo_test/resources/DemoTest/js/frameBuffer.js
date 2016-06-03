
Qt.include("gl-matrix.js")

//
// Draws a cube that has the Qt logo as decal texture on each face in to a texture.
// That texture is used as the texture for drawing another cube on the screen.
//

var gl;

var rttFramebuffer;
var rttTexture;

var cubeTexture = 0;

var vertexPositionAttribute;
var textureCoordAttribute;
var vertexNormalAttribute;
var vertexColorAttribute;

var pMatrixUniform;
var mvMatrixUniform;
var nUniform;
var uClicked;
var uTracer;
var mouseClickPoint = [];
var mouseDown = false;
var selectObjectIndex = -1;
var canvas3d;
var isLogEnabled = false;

function log(message) {
    if (isLogEnabled)
        console.log(message)
}

function initializeGL(canvas, textureLoader) {
    canvas3d = canvas
    try {
        // Get the OpenGL context object that represents the API we call
        gl = canvas.getContext("canvas3d", {depth:true, antialias:true});

        // Setup the OpenGL state
        gl.enable(gl.DEPTH_TEST);
        gl.enable(gl.CULL_FACE);
        gl.cullFace(gl.BACK);
        gl.pixelStorei(gl.UNPACK_FLIP_Y_WEBGL, true);
        gl.clearColor(0.05, 0.05, 0.05, 1.0);
        // Initialize the shader program
        initShaders();

        // Initialize vertex and color buffers
        initBuffers();

        // Load the Qt logo as texture
        var qtLogoImage = TextureImageFactory.newTexImage();
        qtLogoImage.imageLoaded.connect(function() {
            cubeTexture = gl.createTexture();
            cubeTexture.name = "CubeTexture";
            gl.bindTexture(gl.TEXTURE_2D, cubeTexture);
            gl.texImage2D(gl.TEXTURE_2D,    // target
                          0,                // level
                          gl.RGBA,          // internalformat
                          gl.RGBA,          // format
                          gl.UNSIGNED_BYTE, // type
                          qtLogoImage);    // pixels

            gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.LINEAR);
            gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.LINEAR_MIPMAP_NEAREST);
            gl.generateMipmap(gl.TEXTURE_2D);
        });
        qtLogoImage.imageLoadingFailed.connect(function() {
            console.log("Texture load FAILED, "+qtLogoImage.errorString);
        });
        qtLogoImage.src = "../splash.png";

    } catch(e) {
        console.log("initializeGL FAILURE!");
        console.log(""+e);
        console.log(""+e.message);
    }
}

function degToRad(degrees) {
    return degrees * Math.PI / 180;
}


function paintCube(canvas, position) {

    var mvMatrix = mat4.create();
    var pMatrix  = mat4.create();
    var nMatrix  = mat4.create();

    // Calculate and set matrix uniforms
    mat4.perspective(pMatrix, degToRad(90), canvas.width / canvas.height, 0.1, 100.0);
    gl.uniformMatrix4fv(pMatrixUniform, false, pMatrix);

    mat4.identity(mvMatrix);

    mat4.translate(mvMatrix, mvMatrix, [position[0],
                                        position[1],
                                        position[2]]);

    log(mat4.str(mvMatrix));
    //mat4.rotate(mvMatrix, mvMatrix, degToRad(canvas.xRotAnim), [0, 1, 0]);

    gl.uniformMatrix4fv(mvMatrixUniform, false, mvMatrix);

    mat4.invert(nMatrix, mvMatrix);
    mat4.transpose(nMatrix, nMatrix);

    gl.uniformMatrix4fv(nUniform, false, nMatrix);

    // Draw the on-screen cube
    gl.drawElements(gl.TRIANGLES, 36, gl.UNSIGNED_SHORT, 0);
}

function paintGL(canvas, positions) {

    // Bind default framebuffer and setup viewport accordingly

    gl.viewport(0, 0,
                canvas.width * canvas.devicePixelRatio,
                canvas.height * canvas.devicePixelRatio);

    if (mouseDown) {
      select(canvas, positions);
    }
    gl.clearColor(0.05, 0.05, 0.05, 1.0);
    draw(canvas, positions);
}

function select(canvas, positions) {
    // Bind the loaded texture
    gl.bindTexture(gl.TEXTURE_2D, cubeTexture);
    gl.generateMipmap(gl.TEXTURE_2D);

    for (var i = 0; i < positions.length; i++) {
        var picked = false;
          picked = check(canvas, mouseClickPoint[0], mouseClickPoint[1], positions[i]);
          if (picked) {
              selectObjectIndex = i;
          }
    }
}

function draw(canvas, positions) {
    // Bind the loaded texture
    gl.bindTexture(gl.TEXTURE_2D, cubeTexture);
    gl.generateMipmap(gl.TEXTURE_2D);
    gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);
    for (var i = 0; i < positions.length; i++) {
        gl.uniform1i(uTracer, 0);
        if (selectObjectIndex == i) {
          //console.log("===selected object: " + i + " =====");
          gl.uniform1i(uClicked, 1);
        }
        else {
          gl.uniform1i(uClicked, 0);
        }
        paintCube(canvas, positions[i])
    }
}

function onMouseDown(mouseX, mouseY, positions) {

    mouseDown = true;
    var x = mouseX;
    var y = canvas3d.height - mouseY;
    mouseClickPoint[0] = x;
    mouseClickPoint[1] = y;
    selectObjectIndex = -1;
}

function resizeGL(canvas)
{
    var pixelRatio = canvas.devicePixelRatio;
    canvas.pixelSize = Qt.size(canvas.width * pixelRatio,
                               canvas.height * pixelRatio);
}

function initBuffers()
{
    log("        cubeVertexPositionBuffer");
    var cubeVertexPositionBuffer = gl.createBuffer();
    cubeVertexPositionBuffer.name = "cubeVertexPositionBuffer";
    gl.bindBuffer(gl.ARRAY_BUFFER, cubeVertexPositionBuffer);
    gl.bufferData(
                gl.ARRAY_BUFFER,
                new Float32Array([// Front face
                                        -1.0, -1.0,  1.0,
                                        1.0, -1.0,  1.0,
                                        1.0,  1.0,  1.0,
                                        -1.0,  1.0,  1.0,

                                        // Back face
                                        -1.0, -1.0, -1.0,
                                        -1.0,  1.0, -1.0,
                                        1.0,  1.0, -1.0,
                                        1.0, -1.0, -1.0,

                                        // Top face
                                        -1.0,  1.0, -1.0,
                                        -1.0,  1.0,  1.0,
                                        1.0,  1.0,  1.0,
                                        1.0,  1.0, -1.0,

                                        // Bottom face
                                        -1.0, -1.0, -1.0,
                                        1.0, -1.0, -1.0,
                                        1.0, -1.0,  1.0,
                                        -1.0, -1.0,  1.0,

                                        // Right face
                                        1.0, -1.0, -1.0,
                                        1.0,  1.0, -1.0,
                                        1.0,  1.0,  1.0,
                                        1.0, -1.0,  1.0,

                                        // Left face
                                        -1.0, -1.0, -1.0,
                                        -1.0, -1.0,  1.0,
                                        -1.0,  1.0,  1.0,
                                        -1.0,  1.0, -1.0
                                       ]),
                gl.STATIC_DRAW);

    gl.enableVertexAttribArray(vertexPositionAttribute);
    gl.vertexAttribPointer(vertexPositionAttribute, 3, gl.FLOAT, false, 0, 0);

    log("        cubeVertexIndexBuffer");
    var cubeVertexIndexBuffer = gl.createBuffer();
    cubeVertexIndexBuffer.name = "cubeVertexIndexBuffer";
    gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, cubeVertexIndexBuffer);
    gl.bufferData(gl.ELEMENT_ARRAY_BUFFER,
                  new Uint16Array([
                                            0,  1,  2,      0,  2,  3,    // front
                                            4,  5,  6,      4,  6,  7,    // back
                                            8,  9,  10,     8,  10, 11,   // top
                                            12, 13, 14,     12, 14, 15,   // bottom
                                            16, 17, 18,     16, 18, 19,   // right
                                            20, 21, 22,     20, 22, 23    // left
                                        ]),
                  gl.STATIC_DRAW);
    gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, cubeVertexIndexBuffer);

    log("        cubeVerticesTextureCoordBuffer");
    var cubeVerticesTextureCoordBuffer = gl.createBuffer();
    cubeVerticesTextureCoordBuffer.name = "cubeVerticesTextureCoordBuffer";
    gl.bindBuffer(gl.ARRAY_BUFFER, cubeVerticesTextureCoordBuffer);
    var textureCoordinates = [
                // Front
                1.0,  0.0,
                0.0,  0.0,
                0.0,  1.0,
                1.0,  1.0,
                // Back
                1.0,  0.0,
                0.0,  0.0,
                0.0,  1.0,
                1.0,  1.0,
                // Top
                1.0,  0.0,
                0.0,  0.0,
                0.0,  1.0,
                1.0,  1.0,
                // Bottom
                1.0,  0.0,
                0.0,  0.0,
                0.0,  1.0,
                1.0,  1.0,
                // Right
                1.0,  0.0,
                0.0,  0.0,
                0.0,  1.0,
                1.0,  1.0,
                // Left
                1.0,  0.0,
                0.0,  0.0,
                0.0,  1.0,
                1.0,  1.0
            ];
    gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(textureCoordinates),
                  gl.STATIC_DRAW);
    gl.enableVertexAttribArray(textureCoordAttribute);
    gl.vertexAttribPointer(textureCoordAttribute, 2, gl.FLOAT, false, 0, 0);

    var cubeVerticesNormalBuffer = gl.createBuffer();
    cubeVerticesNormalBuffer.name = "cubeVerticesNormalBuffer";
    gl.bindBuffer(gl.ARRAY_BUFFER, cubeVerticesNormalBuffer);
    gl.bufferData(gl.ARRAY_BUFFER, new Float32Array([
                                                              // Front
                                                              0.0,  0.0,  1.0,
                                                              0.0,  0.0,  1.0,
                                                              0.0,  0.0,  1.0,
                                                              0.0,  0.0,  1.0,

                                                              // Back
                                                              0.0,  0.0, -1.0,
                                                              0.0,  0.0, -1.0,
                                                              0.0,  0.0, -1.0,
                                                              0.0,  0.0, -1.0,

                                                              // Top
                                                              0.0,  1.0,  0.0,
                                                              0.0,  1.0,  0.0,
                                                              0.0,  1.0,  0.0,
                                                              0.0,  1.0,  0.0,

                                                              // Bottom
                                                              0.0, -1.0,  0.0,
                                                              0.0, -1.0,  0.0,
                                                              0.0, -1.0,  0.0,
                                                              0.0, -1.0,  0.0,

                                                              // Right
                                                              1.0,  0.0,  0.0,
                                                              1.0,  0.0,  0.0,
                                                              1.0,  0.0,  0.0,
                                                              1.0,  0.0,  0.0,

                                                              // Left
                                                              -1.0,  0.0,  0.0,
                                                              -1.0,  0.0,  0.0,
                                                              -1.0,  0.0,  0.0,
                                                              -1.0,  0.0,  0.0
                                                          ]), gl.STATIC_DRAW);
    gl.bindBuffer(gl.ARRAY_BUFFER, cubeVerticesNormalBuffer);
    gl.vertexAttribPointer(vertexNormalAttribute, 3, gl.FLOAT, false, 0, 0);
}

function initShaders()
{
    var vertexShader = getShader(gl,
                                 "attribute highp vec3 aVertexNormal;   \
                                  attribute highp vec3 aVertexPosition; \
                                  attribute highp vec2 aTextureCoord;   \
                                                                        \
                                  uniform highp mat4 uNormalMatrix;     \
                                  uniform mat4 uMVMatrix;               \
                                  uniform mat4 uPMatrix;                \
                                                                        \
                                  varying mediump vec4 vColor;          \
                                  varying highp vec2 vTextureCoord;     \
                                  varying highp vec3 vLighting;         \
                                                                        \
                                  void main(void) {                     \
                                      gl_Position = uPMatrix * uMVMatrix * vec4(aVertexPosition, 1.0);                      \
                                      vTextureCoord = aTextureCoord;                                                        \
                                      highp vec3 ambientLight = vec3(0.5, 0.5, 0.5);                                        \
                                      highp vec3 directionalLightColor = vec3(0.75, 0.75, 0.75);                             \
                                      highp vec3 directionalVector = vec3(0.85, 0.8, 0.75);                                 \
                                      highp vec4 transformedNormal = uNormalMatrix * vec4(aVertexNormal, 1.0);              \
                                      highp float directional = max(dot(transformedNormal.xyz, directionalVector), 0.0);    \
                                      vLighting = ambientLight + (directionalLightColor * directional);                     \
                                  }", gl.VERTEX_SHADER);
    var fragmentShader = getShader(gl,
                                   "varying highp vec2 vTextureCoord;   \
                                    varying highp vec3 vLighting;       \
                                    uniform bool u_Clicked;             \
                                    uniform bool u_Tracer;             \
                                    uniform sampler2D uSampler;         \
                                                                        \
                                    void main(void) {                   \
                                        mediump vec3 texelColor = texture2D(uSampler, vec2(vTextureCoord.s, vTextureCoord.t)).rgb;  \
                                        mediump vec3 clr = texelColor * vLighting; \
                                        if (u_Tracer) { \
                                            gl_FragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);              \
                                        } \
                                        else { \
                                          if (u_Clicked) { \
                                              gl_FragColor = vec4(0.5f * clr + vec3(0.5f, 0.5f, 0.5f), 1.0f);              \
                                          } \
                                          else { \
                                            gl_FragColor = vec4(clr, 1.0f);\
                                          } \
                                        } \
                                    }", gl.FRAGMENT_SHADER);

    var shaderProgram = gl.createProgram();
    gl.attachShader(shaderProgram, vertexShader);
    gl.attachShader(shaderProgram, fragmentShader);
    gl.linkProgram(shaderProgram);

    if (!gl.getProgramParameter(shaderProgram, gl.LINK_STATUS)) {
        console.log("Could not initialise shaders");
        console.log(gl.getProgramInfoLog(shaderProgram));
    }

    gl.useProgram(shaderProgram);

    // look up where the vertex data needs to go.
    vertexPositionAttribute = gl.getAttribLocation(shaderProgram, "aVertexPosition");
    gl.enableVertexAttribArray(vertexPositionAttribute);
    textureCoordAttribute = gl.getAttribLocation(shaderProgram, "aTextureCoord");
    gl.enableVertexAttribArray(textureCoordAttribute);
    vertexNormalAttribute =gl.getAttribLocation(shaderProgram, "aVertexNormal");
    gl.enableVertexAttribArray(vertexNormalAttribute);

    pMatrixUniform  = gl.getUniformLocation(shaderProgram, "uPMatrix");
    mvMatrixUniform = gl.getUniformLocation(shaderProgram, "uMVMatrix");
    nUniform = gl.getUniformLocation(shaderProgram, "uNormalMatrix");

    uClicked = gl.getUniformLocation(shaderProgram, "u_Clicked");
    gl.uniform1i(uClicked, 0);
    uTracer = gl.getUniformLocation(shaderProgram, "u_Tracer");
    gl.uniform1i(uTracer, 0);
    var textureSamplerUniform = gl.getUniformLocation(shaderProgram, "uSampler")
    gl.activeTexture(gl.TEXTURE0);
    gl.uniform1i(textureSamplerUniform, 0);
}

function getShader(gl, str, type) {
    var shader = gl.createShader(type);
    gl.shaderSource(shader, str);
    gl.compileShader(shader);

    if (!gl.getShaderParameter(shader, gl.COMPILE_STATUS)) {
        console.log("JS:Shader compile failed");
        console.log(gl.getShaderInfoLog(shader));
        return null;
    }

    return shader;
}

function getSelectedObject()
{
    mouseDown= false;
    return selectObjectIndex;
}

function setSelectedObject(idx)
{
    if (mouseDown) {
        return;
    }
    selectObjectIndex = idx;
}

function check(canvas, x, y, position) {
    var picked = false;
    gl.clearColor(0.0, 0.0, 0.0, 1.0);
    gl.uniform1i(uClicked, 0);
    gl.uniform1i(uTracer, 1);
    gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);
    paintCube(canvas, position);
    var pixels = new Uint8Array(4);
    gl.readPixels(x, y, 1, 1, gl.RGBA, gl.UNSIGNED_BYTE, pixels);
    var isWhite = pixels[0] == 255 && pixels[1] == 255 && pixels[2] == 255;
    if (isWhite)
    {
        picked = true;
    }
    return picked;
}
