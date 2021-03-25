// Include standard headers
#include <stdio.h>
#include <stdlib.h>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>
GLFWwindow* window;

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace glm;

#include <common/controls.hpp>
#include <common/shader.hpp>

int main( void )
{
    // Initialise GLFW
    if( !glfwInit() )
    {
        fprintf( stderr, "Failed to initialize GLFW\n" );
        getchar();
        return -1;
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Open a window and create its OpenGL context
    window = glfwCreateWindow( 1024, 768, "Tutorial 02 - Red triangle", NULL, NULL);
    if( window == NULL ){
        fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
        getchar();
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Initialize GLEW
    glewExperimental = true; // Needed for core profile
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        getchar();
        glfwTerminate();
        return -1;
    }

    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

    // background
    glClearColor(0.67843137f, 0.84705882f, 0.90196078f, 0.0f);

    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    // Create and compile our GLSL program from the shaders
    GLuint programID = LoadShaders( "SimpleVertexShader.vertexshader", "SimpleFragmentShader.fragmentshader" );

    // РџРѕР»СѓС‡РёС‚СЊ С…СЌРЅРґР» РїРµСЂРµРјРµРЅРЅРѕР№ РІ С€РµР№РґРµСЂРµ
// РўРѕР»СЊРєРѕ РѕРґРёРЅ СЂР°Р· РІРѕ РІСЂРµРјСЏ РёРЅРёС†РёР°Р»РёР·Р°С†РёРё.
    GLuint MatrixID = glGetUniformLocation(programID, "MVP");

    static const GLfloat g_vertex_buffer_data[] = {
            -0.8f, -0.8f, 0.0f,
            0.8f, -0.8f, 0.0f,
            0.0f,  0.8f, 0.0f,
    };

    GLuint vertexbuffer;
    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

    static const GLfloat second_g_vertex_buffer_data[] = {
            0.8f, 0.8f, 0.0f,
            -0.8f, 0.8f, 0.0f,
            0.0f,  -0.8f, 0.0f,
    };

    GLuint second_vertexbuffer;
    glGenBuffers(1, &second_vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, second_vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(second_g_vertex_buffer_data), second_g_vertex_buffer_data, GL_STATIC_DRAW);

    static const GLfloat g_color_buffer_data[] = {
            1.0, 0.0, 0.0,
            0.0, 0.5, 1.0,
            0.0, 0.0, 0.4
    };

    GLuint colorbuffer;
    glGenBuffers(1, &colorbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_color_buffer_data), g_color_buffer_data, GL_STATIC_DRAW);

    static const GLfloat second_g_color_buffer_data[] = {
            0.960784, 0.960784, 0.862745,
            1.0, 0.98039, 0.80392,
            0.99215, 0.960784, 0.90196
    };

    GLuint second_colorbuffer;
    glGenBuffers(1, &second_colorbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, second_colorbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(second_g_color_buffer_data), second_g_color_buffer_data, GL_STATIC_DRAW);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    GLfloat R = 3.0;
    GLfloat t = 0.0;

    do{

        // Clear the screen
        glClear( GL_COLOR_BUFFER_BIT );

        // Use our shader
        glUseProgram(programID);


        // Projection matrix : 45пїЅ Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
        glm::mat4 Projection = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.0f);
        // Or, for an ortho camera :
        //glm::mat4 Projection = glm::ortho(-10.0f,10.0f,-10.0f,10.0f,0.0f,100.0f); // In world coordinates

        // Camera matrix
        glm::mat4 View       = glm::lookAt(
                glm::vec3(sin(t) * R, 0, cos(t) * R ), // Camera is at (4,3,3), in World Space
                glm::vec3(0, 0, 0 ), // and looks at the origin
                glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
        );
        t += 10e-4;
        // Model matrix : an identity matrix (model will be at the origin)
        glm::mat4 Model      = glm::mat4(1.0f);
        // Our ModelViewProjection : multiplication of our 3 matrices
        glm::mat4 MVP        = Projection * View * Model; // Remember, matrix multiplication is the other way around

        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

        // 1rst attribute buffer : vertices
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        glVertexAttribPointer(
                0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
                3,                  // size
                GL_FLOAT,           // type
                GL_FALSE,           // normalized?
                0,                  // stride
                (void*)0            // array buffer offset
        );

        glEnableVertexAttribArray(1);

        glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
        glVertexAttribPointer(
                1,                                // РђС‚СЂРёР±СѓС‚. Р—РґРµСЃСЊ РЅРµРѕР±СЏР·Р°С‚РµР»СЊРЅРѕ СѓРєР°Р·С‹РІР°С‚СЊ 1, РЅРѕ РіР»Р°РІРЅРѕРµ, С‡С‚РѕР±С‹ СЌС‚Рѕ Р·РЅР°С‡РµРЅРёРµ СЃРѕРІРїР°РґР°Р»Рѕ СЃ layout РІ С€РµР№РґРµСЂРµ..
                3,                                // Р Р°Р·РјРµСЂ
                GL_FLOAT,                         // РўРёРї
                GL_FALSE,                         // РќРѕСЂРјР°Р»РёР·РѕРІР°РЅ?
                0,                                // РЁР°Рі
                (void*)0                          // РЎРјРµС‰РµРЅРёРµ
        );

        // Draw the triangle !
        glDrawArrays(GL_TRIANGLES, 0, 3); // 3 indices starting at 0 -> 1 triangle

        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);


        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, second_vertexbuffer);
        glVertexAttribPointer(
                0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
                3,                  // size
                GL_FLOAT,           // type
                GL_FALSE,           // normalized?
                0,                  // stride
                (void*)0            // array buffer offset
        );

        glEnableVertexAttribArray(1);

        glBindBuffer(GL_ARRAY_BUFFER, second_colorbuffer);
        glVertexAttribPointer(
                1,                                // РђС‚СЂРёР±СѓС‚. Р—РґРµСЃСЊ РЅРµРѕР±СЏР·Р°С‚РµР»СЊРЅРѕ СѓРєР°Р·С‹РІР°С‚СЊ 1, РЅРѕ РіР»Р°РІРЅРѕРµ, С‡С‚РѕР±С‹ СЌС‚Рѕ Р·РЅР°С‡РµРЅРёРµ СЃРѕРІРїР°РґР°Р»Рѕ СЃ layout РІ С€РµР№РґРµСЂРµ..
                3,                                // Р Р°Р·РјРµСЂ
                GL_FLOAT,                         // РўРёРї
                GL_FALSE,                         // РќРѕСЂРјР°Р»РёР·РѕРІР°РЅ?
                0,                                // РЁР°Рі
                (void*)0                          // РЎРјРµС‰РµРЅРёРµ
        );

        // Draw the triangle !
        glDrawArrays(GL_TRIANGLES, 0, 3); // 3 indices starting at 0 -> 1 triangle

//        glEnable(GL_DEPTH_TEST);
//        glDepthFunc(GL_LESS);

        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);

        // Swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();

    } // Check if the ESC key was pressed or the window was closed
    while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
           glfwWindowShouldClose(window) == 0 );

    // Cleanup VBO
    glDeleteBuffers(1, &vertexbuffer);
    glDeleteVertexArrays(1, &VertexArrayID);
    glDeleteProgram(programID);

    // Close OpenGL window and terminate GLFW
    glfwTerminate();

    return 0;
}

