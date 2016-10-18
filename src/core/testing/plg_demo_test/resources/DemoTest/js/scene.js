/**
* Demo scene
* To interact with webGL the scene must implement:
*   - requiredShaders
*   - initialise
*   - renderPickable
*   - render
*   - tick
*/
function Scene()
{
    function Cube()
    {
        var worldMat_;
        var indexBuffer_;
        var vertexBuffers_;
        var faceBuffer_;
        var texture_;
        var pX_;
        var pY_;
        var pZ_;
        var rX_;
        var rY_;
        var rZ_;
        var sX_;
        var sY_;
        var sZ_;
        var visible_;
        var dirty_;
    }
 
    var engine_; 
    var cubes_;
    var selectedCube_;
    var selectedColor_;
    var selectedRotation_;
    var defaultColor_;
    var modelCount_;
    
    /**
    * @return any shaders required by the scene
    */
    this.requiredShaders = function()
    {
        return ["diffuse-shader"];
    }
    
    /**
    * Creates a new cube mesh
    */
    this.createCube = function()
    {
        var cube = new Cube();
        
        var vertices = [
            -1.0, -1.0,  1.0,
             1.0, -1.0,  1.0,
             1.0,  1.0,  1.0,
            -1.0,  1.0,  1.0,
            -1.0, -1.0, -1.0,
            -1.0,  1.0, -1.0,
             1.0,  1.0, -1.0,
             1.0, -1.0, -1.0,
            -1.0,  1.0, -1.0,
            -1.0,  1.0,  1.0,
             1.0,  1.0,  1.0,
             1.0,  1.0, -1.0,
            -1.0, -1.0, -1.0,
             1.0, -1.0, -1.0,
             1.0, -1.0,  1.0,
            -1.0, -1.0,  1.0,
             1.0, -1.0, -1.0,
             1.0,  1.0, -1.0,
             1.0,  1.0,  1.0,
             1.0, -1.0,  1.0,
            -1.0, -1.0, -1.0,
            -1.0, -1.0,  1.0,
            -1.0,  1.0,  1.0,
            -1.0,  1.0, -1.0
        ];
        
        var uvCoords = [
            0.0, 0.0,
            1.0, 0.0,
            1.0, 1.0,
            0.0, 1.0,
            1.0, 0.0,
            1.0, 1.0,
            0.0, 1.0,
            0.0, 0.0,
            0.0, 1.0,
            0.0, 0.0,
            1.0, 0.0,
            1.0, 1.0,
            1.0, 1.0,
            0.0, 1.0,
            0.0, 0.0,
            1.0, 0.0,
            1.0, 0.0,
            1.0, 1.0,
            0.0, 1.0,
            0.0, 0.0,
            0.0, 0.0,
            1.0, 0.0,
            1.0, 1.0,
            0.0, 1.0,
        ];

        var normals = [
             0.0,  0.0,  1.0,
             0.0,  0.0,  1.0,
             0.0,  0.0,  1.0,
             0.0,  0.0,  1.0,
             0.0,  0.0, -1.0,
             0.0,  0.0, -1.0,
             0.0,  0.0, -1.0,
             0.0,  0.0, -1.0,
             0.0,  1.0,  0.0,
             0.0,  1.0,  0.0,
             0.0,  1.0,  0.0,
             0.0,  1.0,  0.0,
             0.0, -1.0,  0.0,
             0.0, -1.0,  0.0,
             0.0, -1.0,  0.0,
             0.0, -1.0,  0.0,
             1.0,  0.0,  0.0,
             1.0,  0.0,  0.0,
             1.0,  0.0,  0.0,
             1.0,  0.0,  0.0,
            -1.0,  0.0,  0.0,
            -1.0,  0.0,  0.0,
            -1.0,  0.0,  0.0,
            -1.0,  0.0,  0.0
        ];        
        
        var indices = [
            0, 1, 2,
            0, 2, 3,
            4, 5, 6,
            4, 6, 7, 
            8, 9, 10,
            8, 10, 11,
            12, 13, 14,
            12, 14, 15,
            16, 17, 18,   
            16, 18, 19,
            20, 21, 22,
            20, 22, 23 
        ];
        
        cube.vertexBuffers_ = [];
        cube.vertexBuffers_.push( engine_.createVertexBufferUV( uvCoords ) );
        cube.vertexBuffers_.push( engine_.createVertexBufferPosition( vertices ) );
        cube.vertexBuffers_.push( engine_.createVertexBufferNormal( normals ) );
        
        cube.faceBuffer_ = engine_.createFaceBuffer( vertices, indices );
        cube.indexBuffer_ = engine_.createIndexBuffer( indices );
        cube.worldMat_ = mat4.create();
        cubes_.push( cube );
        
        this.setModelProperties(
            cubes_.length - 1,
            false,
            "logo.png",
            0.0, 0.0, 0.0, 
            0.0, 0.0, 0.0, 
            1.0, 1.0, 1.0 );
    }
    
    /**
    * Initialises the scene
    * @param engine The webGL engine
    * @return whether initialization was successful
    */
    this.initialise = function( engine ) 
    {        
        engine_ = engine;
        engine_.enableShader( "diffuse-shader" );
        engine_.setLightDirection( 0.0, 1.0, 2.0 );
        engine_.setMeshColour( 1.0, 1.0, 1.0 );
    
        cubes_ = [];
        
        defaultColor_ = vec3.createFrom( 1.0, 1.0, 1.0 );
        selectedColor_ = vec3.createFrom( 1.0, 0.5, 0.5 );
        selectedCube_ = -1;
        selectedRotation_ = 0.0;
        modelCount_ = 0;
        
        return true;
    }
    
    /**
    * Ticks the scene
    * @param deltatime The time passed between ticks
    */
    this.tick = function( deltatime )
    {
        if( cubes_.length < modelCount_ )
        {
            var amount = modelCount_ - cubes_.length;
            for( var i = 0; i < amount; ++i )
            {
                this.createCube();
            }
        }
        else if( cubes_.length > modelCount_ )
        {
            var amount = cubes_.length - modelCount_;
            for( var i = 0; i < amount; ++i )
            {
                cubes_.pop();
            }            
        }
        
        for( var index in cubes_ )
        {
            var cube = cubes_[index];
            if( cube.dirty_ )
            {
                mat4.identity( cube.worldMat_ );
                
                mat4.translate( cube.worldMat_, [ 0.0, 0.0, -7.0 ] );
                mat4.translate( cube.worldMat_, [ cube.pX_, cube.pY_, cube.pZ_ ] );
                
                mat4.rotate( cube.worldMat_, degToRad( cube.rX_ ), [ 1, 0, 0 ] );
                mat4.rotate( cube.worldMat_, degToRad( cube.rY_ ), [ 0, 1, 0 ] );
                mat4.rotate( cube.worldMat_, degToRad( cube.rZ_ ), [ 0, 0, 1 ] );
                
                mat4.scale( cube.worldMat_, [ cube.sX_, cube.sY_, cube.sZ_ ] );

                cube.dirty_ = false;
            }
        }         
    }
    
    /**
    * Renders any meshes that can be interacted with
    */
    this.renderPickable = function()
    {
        for( var index in cubes_ )
        {
            var cube = cubes_[index];
            if( cube.visible_ )
            {
                engine_.renderPickableMesh(
                    index, 
                    cube.worldMat_, 
                    cube.indexBuffer_, 
                    cube.vertexBuffers_ );
            }
        }        
    }
    
    /**
    * Renders the scene
    */
    this.render = function() 
    {
        for( var index in cubes_ )
        {
            var cube = cubes_[index];
            if( cube.visible_ )
            {
                if( index == selectedCube_ )
                {
                    engine_.addDiagnosticMesh(
                        cube.worldMat_,
                        cube.faceBuffer_ );
                    
                    engine_.setMeshColour(
                        selectedColor_[0],
                        selectedColor_[1],
                        selectedColor_[2]);
                }
                else
                {
                    engine_.setMeshColour(
                        defaultColor_[0],
                        defaultColor_[1],
                        defaultColor_[2]);
                }
                
                engine_.setTexture( cube.texture_, 0 );
                engine_.renderMesh(
                    cube.worldMat_, 
                    cube.indexBuffer_, 
                    cube.vertexBuffers_ );                
            }
        }
    }
    
    /**
    * @return the mouse picked model index or -1 if nothing was picked
    */
    this.getPickedModel = function( mouseX, mouseY )
    {
        return engine_.mousePickMesh( mouseX, mouseY );
    }
    
    /**
    * Sets the selected model or -1 if nothing was selected
    */
    this.setSelectedModel = function( index )
    {
        if( selectedCube_ != index )
        {
            selectedRotation_ = 0.0;
            selectedCube_ = index;
        }
    }
    
    /**
    * Sets the selected model or -1 if nothing was selected
    * @note rotation is in degrees
    */
    this.setModelProperties = function( index, visible, textureName, pX, pY, pZ, rX, rY, rZ, sX, sY, sZ )
    {
        if( index >= 0 && index < cubes_.length && cubes_[index] != null )
        {
            var cube = cubes_[index];
            cube.pX_ = pX;
            cube.pY_ = pY;
            cube.pZ_ = pZ;
            cube.rX_ = rX;
            cube.rY_ = rY;
            cube.rZ_ = rZ;
            cube.sX_ = sX;
            cube.sY_ = sY;
            cube.sZ_ = sZ;
            cube.visible_ = visible;
            cube.dirty_ = true;
            cube.texture_ = engine_.getTexture(textureName);
        }
    }
    
    /**
    * Sets the count of models rendered
    */
    this.setModelCount = function( count )
    {
        modelCount_ = count;
    }
}

/**
* Called by Qt to get the picked mesh
*/
document.getPickedModel = function( mouseX, mouseY )
{ 
    return document.getEngine().getScene().getPickedModel( mouseX, mouseY );
}

/**
* Called by Qt to set the selected mesh
*/
document.setSelectedModel = function( index )
{ 
    document.getEngine().getScene().setSelectedModel( index );
}

/**
* Called by Qt to update a mesh
* @note rotation is in degrees
*/
document.setModelProperties = function( index, visible, textureName, pX, pY, pZ, rX, rY, rZ, sX, sY, sZ )
{ 
    document.getEngine().getScene().setModelProperties( 
        index, 
        visible,
        textureName,
        pX, pY, pZ, 
        rX, rY, rZ, 
        sX, sY, sZ );
}

/**
* Called by Qt to set the cube count
*/
document.setModelCount = function( count )
{
    document.getEngine().getScene().setModelCount( count );
}
