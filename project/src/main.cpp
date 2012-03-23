#include <GLUT/glut.h>
#include <cstring>
#include <glm/glm.hpp>
#include <glm/gtx/projection.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>
#include <tuple>
#include "Context.hpp"
#include "Pipeline.hpp"
#include "DefaultRasteriser.hpp"

StateContext context;
GLuint read_fbo;
GLuint framebuffer_tex;

enum : size_t
{
	WIDTH = 640,
	HEIGHT = 480
};

VaryingData vsh_func(size_t vindex, std::vector< VertexArray >& attributes, std::vector< ShaderVariable >& uniforms) {
    VertexArray& va = attributes[0];
    glm::vec3& position = reinterpret_cast< glm::vec3* >(va.vertices)[vindex];
    glm::vec4& color = reinterpret_cast< glm::vec4* >(attributes[1].vertices)[vindex];
    glm::mat4x4& modelview = uniforms[0].m4;
    glm::mat4x4& projection = uniforms[1].m4;
    VaryingData output;
    output.push_back(projection * modelview * glm::vec4(position.x, position.y, position.z, 1.0f));
    output.push_back(color);
    printf("vindex: %i\tcolor (%f, %f, %f, %f)\n", vindex, color[0], color[1], color[2], color[3]);
    return output;
}

glm::vec4 fsh_func(ShaderVariable* varyings, std::vector< ShaderVariable >& uniforms) {
    return (varyings[0].v4);
}

void init(void) {
    float depthClear = 100.0f;
    context.set_framebuffer(WIDTH, HEIGHT, 4);
    context.set_viewport(0, 0, WIDTH, HEIGHT);
    context.set_depth_range(0.0f, 1.0f);
    context.depth_buffer().clear(reinterpret_cast< uint8_t* >(&depthClear));

    Framebuffer& framebuffer = context.framebuffer();
    ShaderVariable modelview = ShaderVariable(glm::mat4x4());
    ShaderVariable projection = glm::perspective(60.0f, static_cast< float >(WIDTH) / static_cast< float >(HEIGHT), 0.1f, 100.0f); // glm::ortho(0.0f, static_cast< float >(WIDTH), 0.0f, static_cast< float >(HEIGHT));

    Shader vsh, fsh;
    vsh.uniforms.push_back(modelview);
    vsh.uniforms.push_back(projection);
    vsh.vfunc = vsh_func;
    fsh.ffunc = fsh_func;

    glm::vec3 positions[] = {
        //glm::vec3(100, 100, 0.0f), glm::vec3(150, 110, 0.0f), glm::vec3(130, 150, 0.0f)
        glm::vec3(-1.0f, -1.0f, -3.0f), glm::vec3(1.0f, -1.0f, -3.0f), glm::vec3(0.0f, 1.0f, -3.0f)
    };
    glm::vec4 colors[] = {
        glm::vec4(1.0f, 0.0f, 0.0f, 1.0f), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f)
    };
    VertexArray pa; pa.stride = 0; pa.components = 3; pa.vertices = positions;
    VertexArray ca; ca.stride = 0; ca.components = 4; ca.vertices = colors;
    std::vector< VertexArray > attributes;
    attributes.push_back(pa);
    attributes.push_back(ca);

    context.set_vertex_shader(vsh);
    context.set_fragment_shader(fsh);
    context.draw(attributes, 0, 3);

    Pipeline pipeline(context);
    pipeline.execute(default_rasteriser);

	glGenTextures(1, &framebuffer_tex);
	glBindTexture(GL_TEXTURE_2D, framebuffer_tex);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, framebuffer.width(), framebuffer.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, framebuffer.pixels());
    //glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, framebuffer.width(), framebuffer.height(), 0, GL_RED, GL_FLOAT, context.depth_buffer().pixels());

	glGenFramebuffers(1, &read_fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, read_fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, framebuffer_tex, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void display(void) {
    Framebuffer& framebuffer = context.framebuffer();
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT);

    glBindFramebuffer(GL_READ_FRAMEBUFFER, read_fbo);
    glReadBuffer(GL_COLOR_ATTACHMENT0);
    glBlitFramebuffer(0, 0, framebuffer.width(), framebuffer.height(), 0, 0, framebuffer.width(), framebuffer.height(), GL_COLOR_BUFFER_BIT, GL_LINEAR);

    glutSwapBuffers();
}

void reshape(int width, int height) {
    glViewport(0, 0, width, height);
}

void idle(void) {
    glutPostRedisplay();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
    glutInitWindowSize(WIDTH, HEIGHT);
    glutCreateWindow("GLUT Program");
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutIdleFunc(idle);

    init();
    
    glutMainLoop();
    return 0;
}