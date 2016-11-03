/**
* WebGL Engine
* Limitations include:
*   - Camera control
*   - Loading json meshes
*/
function Engine()
{
    var gl_;
    var canvas_;
    var projectionMat_;
    var viewMat_;
    var viewProjMat_;
    var previousTime_;
    var currentTime_;
    var scene_;
    var width_;
    var height_;
    var cameraNear_;
    var cameraFar_;
    var shaders_;
    var selectedShaderID_;
    var diagnostics_;
    var idRenderTarget_;
    var textureCache_;
    
    /**
    * Data for a render target
    */
    function RenderTarget()
    {
        var size_;
        var frameBuffer_;
        var texture_;
        var renderBuffer_;
    }
    
    /**
    * Data for diagnostics
    */
    function Diagnostics()
    {
        var positions_;
        var indices_;
        var colors_;
        var indexBuffer_;
        var positionBuffer_;
        var colorBuffer_;
    }
    
    /**
    * Data for a mesh face used for diagnostics
    */
    function Face()
    {
        var normal_;
        var p0_;
        var p1_;
        var p2_;
        var center_;
    }
    
    /**
    * Data for a shader
    */
    function Shader()
    {
        var program_;
        var uniforms_;
        var attributes_;
        var samplerSlots_;
    }
    
    /**
    * Data for a shader sampler
    */
    function SamplerSlot()
    {
        var location_;
        var ID_;
    }    
    
    /**
    * Mapping of default shader IDs
    */
    ShaderID = 
    {
        DIAGNOSTIC_FX : "diagnostic-shader",
        ID_FX         : "id-shader"
    }
    
    /**
    * Mapping of buffer strides
    */
    BufferStride = 
    {
        POSITION_STRIDE : 3,
        NORMAL_STRIDE   : 3,
        UV_STRIDE       : 2,
        COLOR_STRIDE    : 4,
        INDEX_STRIDE    : 1
    }
    
    /**
    * Mapping of shader names
    */
    ShaderEnum = 
    {
        WORLD_MAT     : "uWorldMat",
        PROJ_VIEW_MAT : "uViewProjMat",
        LIGHT_DIR     : "uLightDirection",
        MESH_COLOR    : "uColor",
        SAMPLER       : "uSampler",
        VERT_POSITION : "aVertexPosition",
        VERT_COLOR    : "aVertexColor",
        VERT_UV       : "aVertexUV",
        VERT_NORMAL   : "aVertexNormal"
    }
    
    /**
    * Mapping of texture filters
    */
    TextureFilter = 
    {
        LINEAR_FILTER : 1,
        POINT_FILTER  : 2
    }

    /**
    * Initialises WebGL
    */
    this.initialiseWebGL = function( w, h )
    {
        try 
        {
            canvas_ = document.getElementById( "canvas" );
            if( canvas_ == null )
            {
                console.error( "Could not find canvas" );
                return false;
            }
                        
            cameraNear_ = 0.1;
            cameraFar_ = 100.0;
            selectedShaderID_ = -1;
            
            gl_ = canvas_.getContext( "experimental-webgl" );
            gl_.enable( gl_.DEPTH_TEST );
            
            viewMat_ = mat4.create();
            viewProjMat_ = mat4.create();
            projectionMat_ = mat4.create();
            
            mat4.identity( viewMat_ );
            mat4.identity( viewProjMat_ );
            mat4.identity( projectionMat_ );

            textureCache_ = {};
        } 
        catch (e) 
        {
            console.error( e.toString() );
            return false;
        }
        
        if ( gl_ == null)
        {
            console.error( "Could not initialise WebGL" );
            return false;
        }
        return true;
    }
    
    /**
    * Sets the size of the viewport
    */
    this.setViewportSize = function( w, h )
    {
        width_ = Math.max( w, idRenderTarget_.size_ );
        height_ = Math.max( h, idRenderTarget_.size_ );
        canvas_.width = width_;
        canvas_.height = height_;
        gl_.viewportWidth = width_;
        gl_.viewportHeight = height_;

        var fov = 45.0;
        var aspectRatio = gl_.viewportWidth / gl_.viewportHeight;
        mat4.perspective( fov, aspectRatio, cameraNear_, cameraFar_, projectionMat_ ); 
        
        if( selectedShaderID_ != -1 )
        {
            this.setViewProjectionMatrix();
        }
        
        return true;
    }
    
    /**
    * Initialises the diagnostics
    */
    this.initialiseDiagnostics = function()
    {
        diagnostics_ = new Diagnostics();
        diagnostics_.positions_ = [];
        diagnostics_.indices_ = [];
        diagnostics_.colors_ = [];
        diagnostics_.indexBuffer_ = this.createIndexBuffer( [], true );
        diagnostics_.positionBuffer_ = this.createVertexBufferPosition( [], true );
        diagnostics_.colorBuffer_ = this.createVertexBufferColor( [], true );
        
        return true;
    }
    
    /**
    * Creates the ID from buffer
    */
    this.initialiseIDBuffer = function()
    {
        var rt = new RenderTarget();

        rt.size_ = 128;
        rt.frameBuffer_ = gl_.createFramebuffer();
        gl_.bindFramebuffer( gl_.FRAMEBUFFER, rt.frameBuffer_ );
        rt.frameBuffer_.width = rt.size_;
        rt.frameBuffer_.height = rt.size_;
        
        rt.texture_ = gl_.createTexture();
        gl_.bindTexture( gl_.TEXTURE_2D, rt.texture_ );
        gl_.texParameteri( gl_.TEXTURE_2D, gl_.TEXTURE_MAG_FILTER, gl_.LINEAR );
        gl_.texParameteri( gl_.TEXTURE_2D, gl_.TEXTURE_MIN_FILTER, gl_.LINEAR );
        gl_.texImage2D( gl_.TEXTURE_2D, 0, gl_.RGBA, rt.size_, 
            rt.size_, 0, gl_.RGBA, gl_.UNSIGNED_BYTE, null );
        
        rt.renderBuffer_ = gl_.createRenderbuffer();
        gl_.bindRenderbuffer( gl_.RENDERBUFFER, rt.renderBuffer_ );
        
        gl_.renderbufferStorage( gl_.RENDERBUFFER, 
            gl_.DEPTH_COMPONENT16, rt.size_, rt.size_ );
        
        gl_.framebufferTexture2D( gl_.FRAMEBUFFER, 
            gl_.COLOR_ATTACHMENT0, gl_.TEXTURE_2D, rt.texture_, 0 );
            
        gl_.framebufferRenderbuffer( gl_.FRAMEBUFFER, 
            gl_.DEPTH_ATTACHMENT, gl_.RENDERBUFFER, rt.renderBuffer_ );
        
        gl_.bindTexture( gl_.TEXTURE_2D, null );
        gl_.bindRenderbuffer( gl_.RENDERBUFFER, null );
        gl_.bindFramebuffer( gl_.FRAMEBUFFER, null );        
        
        idRenderTarget_ = rt;
        
        return true;
    }

    /**
    * Creates a shader
    */
    this.initialiseShader = function( doc, id )
    {
        var shaderScript = doc.scripts.namedItem( id );
        if ( shaderScript == null )
        {
            console.error( "Could not find shader " + id );
            return null;
        }    
        
        if( shaderScript.text == "" )
        {
            console.error( "Could not get shader text for " + id );
            return null;
        }
        
        var str = "";
        var shader;
        if ( shaderScript.type == "x-shader/x-fragment" ) 
        {
            shader = gl_.createShader( gl_.FRAGMENT_SHADER );
            str = "precision mediump float; " + shaderScript.text;
        } 
        else if ( shaderScript.type == "x-shader/x-vertex" ) 
        {
            shader = gl_.createShader( gl_.VERTEX_SHADER );
            str = shaderScript.text;
        } 
        else 
        {
            console.error( "Unsupported shader type " + shaderScript.type );
            return null;
        }

        gl_.shaderSource( shader, str );
        gl_.compileShader( shader );

        if ( !gl_.getShaderParameter( shader, gl_.COMPILE_STATUS ) ) 
        {
            console.error( gl_.getShaderInfoLog( shader ) );
            return null;
        }

        return shader;
    }
    
    /**
    * Enables a new shader
    */
    this.enableShader = function( shaderID )
    {
        if( !( shaderID in shaders_ ) )
        {
            console.error( "Shader " + shaderID + " was not initialised" );
            return false;
        }
        
        // Disable any enabled arrays
        if(selectedShaderID_ in shaders_ )
        {
            var previousShader = this.selectedShader();
            for( var index in previousShader.attributes_ )
            {
                if( previousShader.attributes_[index] != null )
                {
                    gl_.disableVertexAttribArray( previousShader.attributes_[index] );
                }
            }
        }
        
        // Disable any enabled textures
        var slot = 0;
        while( this.getTextureID( slot ) != -1 )
        {
            gl_.activeTexture( this.getTextureID( slot ) );
            gl_.bindTexture( gl_.TEXTURE_2D, null );            
            ++slot;
        }        
        
        selectedShaderID_ = shaderID;
        var shader = this.selectedShader();
       
        // Enable the new shader
        gl_.useProgram( shader.program_ );
        for( var index in shader.attributes_ )
        {
            gl_.enableVertexAttribArray( shader.attributes_[index] );
        }
       
        this.setViewProjectionMatrix();
       
        return true;
    }
    
    /**
    * @return the selected shader
    */
    this.selectedShader = function()
    {
        return shaders_[selectedShaderID_];
    }
    
    /**
    * Initialises the shader attribute
    */
    this.initialiseShaderAttribute = function( shader, attr )
    {
        shader.attributes_[attr] = null;
        
        var attrLocation = gl_.getAttribLocation( shader.program_, attr );
        if( attrLocation != -1 )
        {
            shader.attributes_[attr] = attrLocation;
        }
    }
    
    /**
    * Initialises the shader uniform
    */
    this.initialiseShaderUniform = function( shader, uniform )
    {
        shader.uniforms_[uniform] = gl_.getUniformLocation( shader.program_, uniform );
    }    
    
    /**
    * Initialises the shader sampler
    */
    this.initialiseShaderSampler = function( shader, slot )
    {
        shader.samplerSlots_[slot] = null;
        
        var samplerLocation = gl_.getUniformLocation(
            shader.program_, ShaderEnum.SAMPLER + slot );
            
        if( samplerLocation != -1 )
        {
            shader.samplerSlots_[slot] = new SamplerSlot();
            shader.samplerSlots_[slot].location_ = samplerLocation;
            shader.samplerSlots_[slot].ID_ = this.getTextureID( slot );
        }
    }

    /**
    * @return the shader uniform
    */
    this.getShaderUniform = function( uniform )
    {
        return this.selectedShader().uniforms_[uniform];
    }
    
    /**
    * @return the shader attribute
    */
    this.getShaderAttribute = function( attr )
    {
        return this.selectedShader().attributes_[attr];
    }    
    
    /**
    * Initialises all shaders
    */
    this.initialiseShaders = function()
    {
        shaders_ = [];
        
        var shaderDoc = document.querySelector( 'link[rel="import"]' ).import;
        
        var shaderIDs = scene_.requiredShaders();
        shaderIDs.push( ShaderID.DIAGNOSTIC_FX );
        shaderIDs.push( ShaderID.ID_FX );
        
        for( var index in shaderIDs )
        {
            var shaderID = shaderIDs[index];
            if( shaderID in shaders_ )
            {
                continue; // Shader already created
            }
            
            var vertexShader = this.initialiseShader( shaderDoc, shaderID + "-vs" );
            if( vertexShader == null )
            {
                console.error( "Vertex shader initialization failed" );
                return false;
            }
                 
            var fragmentShader = this.initialiseShader( shaderDoc, shaderID + "-fs" );
            if( fragmentShader == null )
            {
                console.error( "Vertex shader initialization failed" );
                return false;
            }
            
            shaders_[shaderID] = new Shader();
            var shader = shaders_[shaderID];
            shader.attributes_ = [];
            shader.uniforms_ = [];
            shader.samplerSlots_ = [];

            shader.program_ = gl_.createProgram();
            gl_.attachShader( shader.program_, vertexShader );
            gl_.attachShader( shader.program_, fragmentShader );
            gl_.linkProgram( shader.program_ );

            if ( !gl_.getProgramParameter( shader.program_, gl_.LINK_STATUS ) ) 
            {
                console.error( "Could not initialise shaders" );
                return false;
            }

            // Initialise all supported shader uniforms and attributes
            this.initialiseShaderAttribute( shader, ShaderEnum.VERT_POSITION );
            this.initialiseShaderAttribute( shader, ShaderEnum.VERT_UV );
            this.initialiseShaderAttribute( shader, ShaderEnum.VERT_NORMAL );
            this.initialiseShaderAttribute( shader, ShaderEnum.VERT_COLOR );
            this.initialiseShaderUniform( shader, ShaderEnum.PROJ_VIEW_MAT );
            this.initialiseShaderUniform( shader, ShaderEnum.WORLD_MAT );
            this.initialiseShaderUniform( shader, ShaderEnum.LIGHT_DIR );
            this.initialiseShaderUniform( shader, ShaderEnum.MESH_COLOR );
        
            // Find all supported texture slots
            var slot = 0;
            while( this.getTextureID( slot ) != -1 )
            {
                this.initialiseShaderSampler( shader, slot );
                ++slot;
            }
        }
        
        this.enableShader( ShaderID.ID_FX );
        return true;
    }
    
    /**
    * @return the texture ID
    * @note currently only supporting binding 8 textures to shader
    */
    this.getTextureID = function( ID )
    {
        switch(ID)
        {
        case 0:
            return gl_.TEXTURE0;
        case 1:
            return gl_.TEXTURE1;
        case 2:
            return gl_.TEXTURE2;
        case 3:
            return gl_.TEXTURE3;
        case 4:
            return gl_.TEXTURE4;
        case 5:
            return gl_.TEXTURE5;
        case 6:
            return gl_.TEXTURE6;
        case 7:
            return gl_.TEXTURE7;
        case 8:
            return gl_.TEXTURE8; 
        }
        return -1;
    }
    
    /**
    * Creates the scene
    */
    this.createScene = function() 
    {
        if ( typeof Scene !== 'function' )
        {
            console.error( "No WebGL Scene found" );
            return false;
        }
        scene_ = new Scene();
        return true;
    }
    
    /**
    * Initialises the scene
    */
    this.initialiseScene = function() 
    {
        currentTime_ = new Date().getTime();
        previousTime_ = new Date().getTime();
        return scene_.initialise( this );
    }
    
    /**
    * Creates a vertex buffer of positions
    * Each position must consist of 3 floats
    */
    this.createVertexBufferPosition = function( vertices, dynamic )
    {
        return this.createVertexBuffer( vertices, 
            BufferStride.POSITION_STRIDE,
            ShaderEnum.VERT_POSITION,
            dynamic || false );
    }
    
    /**
    * Creates a vertex buffer of colors
    * Each color must consist of 4 floats
    */
    this.createVertexBufferColor = function( vertices, dynamic )
    {
        return this.createVertexBuffer( vertices, 
            BufferStride.COLOR_STRIDE, 
            ShaderEnum.VERT_COLOR,
            dynamic || false );
    }

    /**
    * Creates a vertex buffer of uvs
    * Each uv must consist of 2 floats
    */
    this.createVertexBufferUV = function( vertices, dynamic )
    {
        return this.createVertexBuffer( vertices, 
            BufferStride.UV_STRIDE,
            ShaderEnum.VERT_UV,
            dynamic || false );
    }
    
    /**
    * Creates a vertex buffer of normals
    * Each normal must consist of 3 floats
    */
    this.createVertexBufferNormal = function( vertices, dynamic )
    {
        return this.createVertexBuffer( vertices, 
            BufferStride.NORMAL_STRIDE,
            ShaderEnum.VERT_NORMAL,
            dynamic || false );
    }        
    
    /**
    * Creates a Vertex Buffer
    */
    this.createVertexBuffer = function( vertices, stride, type, dynamic )
    {
        var buffer = gl_.createBuffer();
        buffer.stride_ = stride;
        buffer.type_ = type;
        buffer.flag_ = dynamic ? gl_.DYNAMIC_DRAW : gl_.STATIC_DRAW;
        
        if( vertices.length > 0 )
        {
            gl_.bindBuffer( gl_.ARRAY_BUFFER, buffer );
            this.fillVertexBuffer( buffer, vertices );
        }
        
        return buffer;            
    }
    
    /**
    * Fills a vertex buffer
    */
    this.fillVertexBuffer = function( buffer, vertices )
    {
        gl_.bufferData( gl_.ARRAY_BUFFER, new Float32Array( vertices ), buffer.flag_ );        
        buffer.numItems_ = vertices.length / buffer.stride_;
    }
    
    /**
    * Creates an Index Buffer
    */
    this.createIndexBuffer = function( indices, dynamic )
    {
        dynamic = dynamic || false;
        var stride = BufferStride.INDEX_STRIDE;
        var buffer = gl_.createBuffer();
        
        buffer.stride_ = stride;
        buffer.flag_ = dynamic ? gl_.DYNAMIC_DRAW : gl_.STATIC_DRAW;
        
        if( indices.length > 0 )
        {
            gl_.bindBuffer( gl_.ELEMENT_ARRAY_BUFFER, buffer );
            this.fillIndexBuffer( buffer, indices );
        }        
        
        return buffer;
    }
    
    /**
    * Fills an Index Buffer
    */
    this.fillIndexBuffer = function( buffer, indices )
    {
        gl_.bufferData( gl_.ELEMENT_ARRAY_BUFFER, new Uint16Array(indices), buffer.flag_ );
        buffer.numItems_ = indices.length / buffer.stride_;
    }
    
    /**
    * Creates a face buffer used for diagnostics
    */
    this.createFaceBuffer = function( positions, indices )
    {
        if( positions == null )
        {
            console.error( "Face buffer requires a position buffer" );
            return null;
        }

        if( indices == null )
        {
            console.error( "Face buffer requires an index buffer" );
            return null;
        }
        
        var getPositionIndex = function( i )
        {
            return indices[i] * BufferStride.POSITION_STRIDE;
        };
        
        var faceBuffer = [];
        
        // For each triangle in mesh
        for( var i = 0; i < indices.length; i += 3 )
        {            
            var face = new Face();

            // Get position of each vertex
            var p0 = getPositionIndex( i+0 );
            face.p0_ = vec3.createFrom(
                positions[p0],
                positions[p0+1],
                positions[p0+2] );
            
            var p1 = getPositionIndex( i+1 );
            face.p1_ = vec3.createFrom(
                positions[p1],
                positions[p1+1],
                positions[p1+2] );
            
            var p2 = getPositionIndex( i+2 );
            face.p2_ = vec3.createFrom(
                positions[p2],
                positions[p2+1],
                positions[p2+2] );
            
            // Get the center point of the triangle
            face.center_ = vec3.createFrom(
                ( face.p0_[0] + face.p1_[0] + face.p2_[0] ) / 3.0,
                ( face.p0_[1] + face.p1_[1] + face.p2_[1] ) / 3.0,
                ( face.p0_[2] + face.p1_[2] + face.p2_[2] ) / 3.0 );
            
            // Get axis of face
            var u = vec3.createFrom(
                face.p1_[0] - face.p0_[0],
                face.p1_[1] - face.p0_[1],
                face.p1_[2] - face.p0_[2] );
            
            var v = vec3.createFrom(
                face.p2_[0] - face.p0_[0],
                face.p2_[1] - face.p0_[1],
                face.p2_[2] - face.p0_[2] );
            
            // Get normal of face
            face.normal_ = vec3.createFrom( 0.0, 0.0, 0.0 );
            vec3.cross( u, v, face.normal_ );            
            vec3.normalize( face.normal_ );

            faceBuffer.push( face );
        }   

        return faceBuffer;
    }
        
    
    /**
    * Binds a vertex buffer to the shader if the shader supports it
    */
    this.bindVertexBuffer = function( buffer )
    {
        if( buffer == null )
        {
            return;
        }
    
        var attr = this.getShaderAttribute( buffer.type_ );
        if( attr == null )
        {
            return;
        }
    
        gl_.bindBuffer( gl_.ARRAY_BUFFER, buffer );
        gl_.vertexAttribPointer( attr, buffer.stride_, gl_.FLOAT, false, 0, 0 );
    }
    
    /**
    * Binds an index buffer to the shader
    */
    this.bindIndexBuffer = function( buffer )
    {
        gl_.bindBuffer( gl_.ELEMENT_ARRAY_BUFFER, buffer );
    }
    
    /**
    * Unbinds the vertex and index buffers
    */
    this.unbindBuffers = function()
    {
        gl_.bindBuffer( gl_.ARRAY_BUFFER, null );
        gl_.bindBuffer( gl_.ELEMENT_ARRAY_BUFFER, null );        
    }
    
    /**
    * @return the texture filter
    */
    this.getTextureFilter = function( filter )
    {
        switch( filter )
        {
        case TextureFilter.LINEAR_FILTER:
            return gl_.LINEAR;
        case TextureFilter.POINT_FILTER:
            return gl_.POINT;            
        }
        console.error( "Unsupported texture filter " + filter );
        return -1;
    }
    
    /**
    * Creates a texture from file
    */
    this.getTexture = function( file, filter )
    {
        if ( file in textureCache_ )
        {
            return textureCache_[file];
        }
        
        filter = filter || TextureFilter.LINEAR_FILTER;
        glFilter = this.getTextureFilter( filter );
        
        var texture = gl_.createTexture();
        texture.hasLoaded_ = false;
        texture.image = new Image();
        texture.image.onload = function() 
        {
            gl_.bindTexture( gl_.TEXTURE_2D, texture );
            gl_.pixelStorei( gl_.UNPACK_FLIP_Y_WEBGL, true );
            gl_.texImage2D( gl_.TEXTURE_2D, 0, gl_.RGBA, gl_.RGBA, gl_.UNSIGNED_BYTE, texture.image );
            gl_.texParameteri( gl_.TEXTURE_2D, gl_.TEXTURE_MAG_FILTER, glFilter );
            gl_.texParameteri( gl_.TEXTURE_2D, gl_.TEXTURE_MIN_FILTER, glFilter );
            gl_.texParameteri( gl_.TEXTURE_2D, gl_.TEXTURE_WRAP_S, gl_.CLAMP_TO_EDGE);
            gl_.texParameteri( gl_.TEXTURE_2D, gl_.TEXTURE_WRAP_T, gl_.CLAMP_TO_EDGE);            
            gl_.bindTexture( gl_.TEXTURE_2D, null );
            texture.hasLoaded_ = true;
        }

        texture.image.src = file;
        textureCache_[file] = texture;

        return texture;
    }
    
    /**
    * Binds a texture to the shader slot
    */
    this.setTexture = function( texture, slot )
    {
        if( texture == null || !texture.hasLoaded_ )
        {
            return false;
        }
        
        var shader = this.selectedShader();
        
        if( slot >= shader.samplerSlots_.length || shader.samplerSlots_[slot] == null )
        {
            console.error( "Unsupported texture slot " + slot );
            return false;
        }
        
        gl_.activeTexture( shader.samplerSlots_[slot].ID_ );
        gl_.bindTexture( gl_.TEXTURE_2D, texture );
        gl_.uniform1i( shader.samplerSlots_[slot].location_, 0 );
        return true;
    }
    
    /**
    * Sets the mesh colour on the shader if supported
    */
    this.setMeshColour = function( r, g, b )
    {
        var uniform = this.getShaderUniform( ShaderEnum.MESH_COLOR );
        if( uniform != null )
        {
            gl_.uniform4f( uniform, r, g, b, 1.0 );
        }
        else
        {
            console.error( "Mesh colour not supported for current shader" );
        }        
    }    
    
    /**
    * Sets the light direction on the shader if supported
    * @note direction will be normalized before being set
    */
    this.setLightDirection = function( x, y, z )
    {
        var uniform = this.getShaderUniform( ShaderEnum.LIGHT_DIR );
        if( uniform != null )
        {
            var direction = vec3.createFrom( x, y, z );
            vec3.normalize( direction );
            gl_.uniform3f( uniform, direction[0], direction[1], direction[2] );
        }
        else
        {
            console.error( "Lighting not supported for current shader" );
        }        
    }
    
    /**
    * Sets the view projection matrix on the shader
    */
    this.setViewProjectionMatrix = function()
    {
        mat4.multiply( projectionMat_, viewMat_, viewProjMat_ );
        gl_.uniformMatrix4fv( this.getShaderUniform( ShaderEnum.PROJ_VIEW_MAT ), false, viewProjMat_ );        
    }    
    
    /**
    * Sets the world matrix on the shader
    */
    this.setWorldMatrix = function( worldMat )
    {
        gl_.uniformMatrix4fv( this.getShaderUniform( ShaderEnum.WORLD_MAT ), false, worldMat );        
    }
    
    /**
    * Renders a pickable mesh
    * @param ID a unique ID for the mesh from [0,255]
    * @param worldMat The mesh world matrix
    * @param indexBuffer The mesh indices
    * @param vertexBuffers An array of mesh vertex buffers
    */
    this.renderPickableMesh = function( ID, worldMat, indexBuffer, vertexBuffers )
    {
        this.setMeshColour( ID / 255.0, 1.0, 1.0 );
        this.renderMesh( worldMat, indexBuffer, vertexBuffers );
    }    
    
    /**
    * Renders a mesh
    * @param worldMat The mesh world matrix
    * @param indexBuffer The mesh indices
    * @param vertexBuffers An array of mesh vertex buffers
    */
    this.renderMesh = function( worldMat, indexBuffer, vertexBuffers )
    {
        for( var index in vertexBuffers )
        {
            this.bindVertexBuffer( vertexBuffers[index] );
        }

        this.bindIndexBuffer( indexBuffer );
        this.setWorldMatrix( worldMat );
        
        gl_.drawElements( gl_.TRIANGLES, indexBuffer.numItems_, gl_.UNSIGNED_SHORT, 0 );
        
        this.unbindBuffers();
    }
    
    /**
    * Determines which mesh was picked by the mouse or -1 if none selected
    */
    this.mousePickMesh = function(mouseX, mouseY)
    {
        var x = parseInt( mouseX * ( idRenderTarget_.size_ / width_ ) );
        var y = parseInt( mouseY * ( idRenderTarget_.size_ / height_ ) );
        y = idRenderTarget_.size_ - y;
        
        gl_.bindFramebuffer( gl_.FRAMEBUFFER, idRenderTarget_.frameBuffer_ );
        var pixels = new Uint8Array( 4 );
        gl_.readPixels( x, y, 1, 1, gl_.RGBA, gl_.UNSIGNED_BYTE, pixels );
        gl_.bindFramebuffer( gl_.FRAMEBUFFER, null );
          
        return pixels[1] == 0 ? -1 : pixels[0];
    }
    
    /**
    * Adds the face buffer to diagnostics
    */
    this.addDiagnosticMesh = function(worldMat, faceBuffer)
    {
        var worldRotation = mat3.create();
        mat4.toMat3( worldMat, worldRotation );
        
        for( var index in faceBuffer )
        {
            var face = faceBuffer[index];

            var p0 = vec3.createFrom( 0.0, 0.0, 0.0 );
            mat4.multiplyVec3( worldMat, face.p0_, p0 );
            
            var p1 = vec3.createFrom( 0.0, 0.0, 0.0 );
            mat4.multiplyVec3( worldMat, face.p1_, p1 );    

            var p2 = vec3.createFrom( 0.0, 0.0, 0.0 );
            mat4.multiplyVec3( worldMat, face.p2_, p2 );                
            
            this.addDiagnosticLine( p0, p1, 1.0, 0.0, 0.0 );
            this.addDiagnosticLine( p1, p2, 1.0, 0.0, 0.0 );
            this.addDiagnosticLine( p2, p0, 1.0, 0.0, 0.0 );
        }
    }
    
    /**
    * Adds the face buffer to diagnostics
    */
    this.addDiagnosticMeshNormals = function(worldMat, faceBuffer)
    {
        var worldRotation = mat3.create();
        mat4.toMat3( worldMat, worldRotation );
        
        for( var index in faceBuffer )
        {
            var face = faceBuffer[index];
            
            var center = vec3.createFrom( 0.0, 0.0, 0.0 );
            mat4.multiplyVec3( worldMat, face.center_, center );
            
            var normal = vec3.createFrom( 0.0, 0.0, 0.0 );
            mat3.multiplyVec3( worldRotation, face.normal_, normal );

            this.addDiagnosticLineDirection( center, normal, 1.0, 0.0, 0.0 );
        }
    }    
    
    /**
    * Adds a new diagnostic line
    * @note the line will only render for the frame added
    */
    this.addDiagnosticLineDirection = function( start, direction, r, g, b )
    {
        var end = vec3.createFrom(
            start[0] + direction[0],
            start[1] + direction[1],
            start[2] + direction[2] );    
        this.addDiagnosticLine( start, end, r, g, b );
    } 
    
    /**
    * Adds a new diagnostic line
    * @note the line will only render for the frame added
    */
    this.addDiagnosticLine = function( start, end, r, g, b )
    {
        var index = diagnostics_.positions_.length / BufferStride.POSITION_STRIDE;
        
        diagnostics_.positions_.push( start[0] );
        diagnostics_.positions_.push( start[1] );
        diagnostics_.positions_.push( start[2] );
        diagnostics_.colors_.push( r );
        diagnostics_.colors_.push( g );
        diagnostics_.colors_.push( b );
        diagnostics_.colors_.push( 1.0 );        
        
        diagnostics_.positions_.push( end[0] );
        diagnostics_.positions_.push( end[1] );
        diagnostics_.positions_.push( end[2] );
        diagnostics_.colors_.push( r );
        diagnostics_.colors_.push( g );
        diagnostics_.colors_.push( b );
        diagnostics_.colors_.push( 1.0 );      
        
        diagnostics_.indices_.push( index );
        diagnostics_.indices_.push( index + 1);        
    }
    
    /**
    * Renders all diagnostics
    * @note diagnostics is slow and should only be used when testing
    */
    this.renderDiagnostics = function()
    {
        if( diagnostics_.positions_.length == 0 )
        {
            return;
        }
        
        var previousShaderID = selectedShaderID_;
        this.enableShader( ShaderID.DIAGNOSTIC_FX );
        
        this.bindVertexBuffer( diagnostics_.positionBuffer_ );
        this.fillVertexBuffer( diagnostics_.positionBuffer_, diagnostics_.positions_ );
        
        this.bindVertexBuffer( diagnostics_.colorBuffer_ );
        this.fillVertexBuffer( diagnostics_.colorBuffer_, diagnostics_.colors_ );
        
        this.bindIndexBuffer( diagnostics_.indexBuffer_ );
        this.fillIndexBuffer( diagnostics_.indexBuffer_, diagnostics_.indices_ );
        
        gl_.drawArrays( gl_.LINES, 0, diagnostics_.indexBuffer_.numItems_ );
        
        this.unbindBuffers();
        this.enableShader( previousShaderID );
    }
    
    /**
    * Clears all diagnostics
    */
    this.clearDiagnostics = function()
    {
        diagnostics_.positions_ = [];
        diagnostics_.indices_ = [];
        diagnostics_.colors_ = [];
    }
    
    /**
    * Renders the scene
    */
    this.render = function()
    {
        this.clearDiagnostics();
        
        currentTime_ = new Date().getTime();
        var deltatime = currentTime_ - previousTime_;
        previousTime_ = currentTime_;        
        scene_.tick( deltatime );
        
        // Render to the ID buffer
        gl_.bindFramebuffer( gl_.FRAMEBUFFER, idRenderTarget_.frameBuffer_ );
        gl_.clearColor( 0.0, 0.0, 0.0, 1.0 );
        gl_.viewport( 0, 0, idRenderTarget_.size_, idRenderTarget_.size_ );
        gl_.clear( gl_.COLOR_BUFFER_BIT | gl_.DEPTH_BUFFER_BIT );
        
        var previousShader = selectedShaderID_;
        this.enableShader( ShaderID.ID_FX );
        scene_.renderPickable();
        this.enableShader( previousShader );
        
        // Render to the back buffer
        gl_.bindFramebuffer( gl_.FRAMEBUFFER, null );
        gl_.clearColor( 0.1, 0.1, 0.1, 1.0 );
        gl_.viewport( 0, 0, width_, height_ );
        gl_.clear( gl_.COLOR_BUFFER_BIT | gl_.DEPTH_BUFFER_BIT );
        
        scene_.render();
        
        this.renderDiagnostics();
    }
        
    /**
    * @return the scene
    */
    this.getScene = function()
    {
        return scene_;
    } 
}

/**
* WebGL Engine
*/
var engine_ = new Engine();

/**
* @return the WebGL Engine
*/
document.getEngine = function()
{
    return engine_;
}

/**
* Sets the width and height of the viewport
*/
document.setViewportSize = function( w, h )
{
    engine_.setViewportSize( w, h );
}

/**
* Called by Qt to initialise WebGL
*/
document.engineStart = function( w, h )
{   
    if(!engine_.initialiseWebGL() ||
       !engine_.initialiseIDBuffer() ||
       !engine_.setViewportSize( w, h ) ||    
       !engine_.createScene() ||
       !engine_.initialiseShaders() ||
       !engine_.initialiseDiagnostics() ||
       !engine_.initialiseScene())
    {
        return false;
    }
    
    tick();
    return true;
}

/**
* Ticks the engine
*/
function tick()
{
    requestAnimationFrame( tick );
    engine_.render();
}

/**
* Converts degrees to radians
*/
function degToRad(degrees) 
{
    return degrees * Math.PI / 180.0;
}
