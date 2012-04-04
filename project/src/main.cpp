#include <GLUT/glut.h>
#include <cstring>
#include <glm/glm.hpp>
#include <glm/gtx/projection.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>
#include <tuple>
#include "Renderer.hpp"
#include "DefaultRasteriser.hpp"
#include "stb_image.h"

Renderer renderer;
GLuint read_fbo;
GLuint framebuffer_tex;
struct {
    void* data;
    int width, height, numChannels;
} texture;

enum : size_t
{
	WIDTH = 640,
	HEIGHT = 480
};

void calculateFPS()
{
    static int frameCount = 0, previousTime = 0;
    //  Increase frame count
    frameCount++;

    //  Get the number of milliseconds since glutInit called
    //  (or first callr to glutGet(GLUT ELAPSED TIME)).
    int currentTime = glutGet(GLUT_ELAPSED_TIME);

    //  Calculate time passed
    int timeInterval = currentTime - previousTime;

    if(timeInterval > 1000)
    {
        //  calculate the number of frames per second
        float fps = frameCount / (timeInterval / 1000.0f);
        printf("fps: %f\n", fps);

        //  Set time
        previousTime = currentTime;

        //  Reset frame count
        frameCount = 0;
    }
}

VaryingData vsh_func(size_t vindex, std::vector< VertexArray > const& attributes, std::vector< ShaderVariable > const& uniforms) {
    auto& position   = reinterpret_cast< glm::vec3* >(attributes[0].vertices)[vindex];
    auto& color      = reinterpret_cast< glm::vec4* >(attributes[1].vertices)[vindex];
    auto& uv         = reinterpret_cast< glm::vec2* >(attributes[2].vertices)[vindex];
    glm::mat4x4 const& modelview  = uniforms[0].m4;
    glm::mat4x4 const& projection = uniforms[1].m4;

    VaryingData output;
    output.push_back(projection * modelview * glm::vec4(position.x, position.y, position.z, 1.0f));
    output.push_back(color);
    output.push_back(uv);
    return output;
}

glm::vec4 fsh_func(ShaderVariable* varyings, std::vector< ShaderVariable > const& uniforms) {
    glm::ivec2 st(varyings[1].v2.x * texture.width, varyings[1].v2.y * texture.height);
    uint32_t* pixels = reinterpret_cast< uint32_t* >(texture.data);
    uint32_t pcolor = pixels[(st.y * texture.width) + st.x];
    // return glm::vec4(varyings[1].v2.x, varyings[1].v2.y, 0.0f, 1.0f);
    return glm::vec4(float(pcolor & 0xff) / 255.0f, float((pcolor >> 8) & 0xff) / 255.0f, float((pcolor >> 16) & 0xff) / 255.0f, float((pcolor >> 24) & 0xff) / 255.0f);
    // return (varyings[0].v4);
}

void init(void) {
    texture.data = nullptr;
    texture.data = stbi_load("/Users/jhaberstro/Personal-Projects/software-rasterizer/project/bin/NeHe.png", &texture.width, &texture.height, &texture.numChannels, 4);
    assert(texture.data != nullptr);

    renderer.set_framebuffer(WIDTH, HEIGHT, 4);
    renderer.set_viewport(0, 0, WIDTH, HEIGHT);
    renderer.set_depth_range(0.0f, 1.0f);

    Framebuffer& framebuffer = renderer.framebuffer();
    glGenTextures(1, &framebuffer_tex);
    glBindTexture(GL_TEXTURE_2D, framebuffer_tex);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, framebuffer.width(), framebuffer.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, framebuffer.pixels());
    //glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, framebuffer.width(), framebuffer.height(), 0, GL_RED, GL_FLOAT, renderer.depth_buffer().pixels());

    glGenFramebuffers(1, &read_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, read_fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, framebuffer_tex, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void GLEquivalient() {
    auto modelview = glm::translate(glm::mat4x4(), glm::vec3(0.0f, 0.0f, -3.5f)) * glm::rotate(glm::mat4x4(), float(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    auto projection = glm::perspective(60.0f, static_cast< float >(WIDTH) / static_cast< float >(HEIGHT), 0.1f, 100.0f);
    glm::vec3 positions[] = {glm::vec3(-1.0f, -1.0f, 0.0f), glm::vec3(1.0f, -1.0f, 0.0f), glm::vec3(-1.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 0.0f) };
    glm::vec4 colors[] = { glm::vec4(1.0f, 0.0f, 0.0f, 1.0f), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), glm::vec4(1.0f) };
    glViewport(0, 0, WIDTH, HEIGHT);
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(&projection[0][0]);
    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf(&modelview[0][0]);
    glEnable(GL_DEPTH_TEST);

    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBegin(GL_TRIANGLE_STRIP);
    for (int i = 0; i < sizeof(positions) / sizeof(positions[0]); ++i) {
        glColor4fv((GLfloat*)(colors + i));
        glVertex3fv((GLfloat*)(positions + i));
    }
    glEnd();
}

void draw() {
    uint32_t clearColor = 0x00000000;
    float depthClear = INFINITY;
    renderer.depth_buffer().clear(&depthClear);
    Framebuffer& framebuffer = renderer.framebuffer();
    framebuffer.clear(&clearColor);

    glm::vec3 positions[] = {glm::vec3(-1.0f, -1.0f, 0.0f), glm::vec3(1.0f, -1.0f, 0.0f), glm::vec3(-1.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 0.0f) };
    glm::vec4 colors[] = { glm::vec4(1.0f, 0.0f, 0.0f, 1.0f), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), glm::vec4(1.0f) };
    glm::vec2 uvs[] = { glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 0.0f), glm::vec2(0.0f, 1.0f), glm::vec2(1.0f, 1.0f) };
    VertexArray pa(3, 0, positions);
    VertexArray ca(4, 0, colors);
    VertexArray ta(2, 0, uvs);
    std::vector< VertexArray > attributes;
    attributes.push_back(pa);
    attributes.push_back(ca);
    attributes.push_back(ta);

    static int angle = 0;
    angle = (angle + 1) % 360;
    auto modelview = glm::translate(glm::mat4x4(), glm::vec3(0.0f, 0.0f, -3.5f)) * glm::rotate(glm::mat4x4(), float(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    auto projection = glm::perspective(60.0f, static_cast< float >(WIDTH) / static_cast< float >(HEIGHT), 0.1f, 100.0f);
    Shader vsh(vsh_func), fsh(fsh_func);
    vsh.uniforms.push_back(modelview);
    vsh.uniforms.push_back(projection);

    renderer.set_vertex_shader(vsh);
    renderer.set_fragment_shader(fsh);

    renderer.set_primitive_topology(PrimitiveTopology::TriangleStrip);
    renderer.draw(attributes, 0, 4);

    glBindTexture(GL_TEXTURE_2D, framebuffer_tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, framebuffer.width(), framebuffer.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, framebuffer.pixels());
    //glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, framebuffer.width(), framebuffer.height(), 0, GL_RED, GL_FLOAT, renderer.depth_buffer().pixels());
}

void display(void) {
    Framebuffer& framebuffer = renderer.framebuffer();
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT);

    // GLEquivalient();

    draw();
    glBindFramebuffer(GL_READ_FRAMEBUFFER, read_fbo);
    glReadBuffer(GL_COLOR_ATTACHMENT0);
    glBlitFramebuffer(0, 0, framebuffer.width(), framebuffer.height(), 0, 0, framebuffer.width(), framebuffer.height(), GL_COLOR_BUFFER_BIT, GL_LINEAR);

    glFinish();
    glutSwapBuffers();

    calculateFPS();
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