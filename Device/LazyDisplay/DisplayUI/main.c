#include <gtk/gtk.h>
#include <epoxy/gl.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>

#define HEIGHT 256
#define WIDTH 256

guint8 pixels[HEIGHT][WIDTH]; // note swapped order for clarity

typedef struct {
    GLuint texture;
} GLState;

static void init_texture(GLState *glstate) {
    glGenTextures(1, &glstate->texture);
    glBindTexture(GL_TEXTURE_2D, glstate->texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, WIDTH, HEIGHT, 0,
                 GL_RED, GL_UNSIGNED_BYTE, pixels);
}

static void update_texture(GLState *glstate) {
    glBindTexture(GL_TEXTURE_2D, glstate->texture);

    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, WIDTH, HEIGHT,
                    GL_RED, GL_UNSIGNED_BYTE, pixels);
}

static const char *vertex_shader_src =
"#version 300 es\n"
"in vec2 position;"
"in vec2 texcoord;"
"out vec2 v_texcoord;"
"void main() {"
"    gl_Position = vec4(position, 0.0, 1.0);"
"    v_texcoord = texcoord;"
"}";

static const char *fragment_shader_src =
"#version 300 es\n"
"precision mediump float;\n"
"uniform sampler2D tex;\n"
"in vec2 v_texcoord;\n"
"out vec4 out_color;\n"
"void main() {\n"
"    vec2 texSize = vec2(256.0, 256.0);\n"
"    vec2 flipped_texcoord = vec2(v_texcoord.x, 1.0 - v_texcoord.y);\n"
"    vec2 uv = floor(flipped_texcoord * texSize) / texSize + 0.5 / texSize;\n"
"    float intensity = texture(tex, uv).r;\n"
"    out_color = vec4(intensity, intensity, intensity, 1.0);\n"
"}";

static GLuint compile_shader(GLenum type, const char *src) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &src, NULL);
    glCompileShader(shader);

    GLint status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (!status) {
        char log[512];
        glGetShaderInfoLog(shader, 512, NULL, log);
        g_warning("Shader compile error: %s", log);
    }
    return shader;
}

static GLuint create_program(void) {
    GLuint vertex = compile_shader(GL_VERTEX_SHADER, vertex_shader_src);
    GLuint fragment = compile_shader(GL_FRAGMENT_SHADER, fragment_shader_src);

    GLuint program = glCreateProgram();
    glAttachShader(program, vertex);
    glAttachShader(program, fragment);
    glBindAttribLocation(program, 0, "position");
    glBindAttribLocation(program, 1, "texcoord");
    glLinkProgram(program);

    GLint status;
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    if (!status) {
        char log[512];
        glGetProgramInfoLog(program, 512, NULL, log);
        g_warning("Program link error: %s", log);
    }

    glDeleteShader(vertex);
    glDeleteShader(fragment);

    return program;
}

static const GLfloat vertices[] = {
    -1.0f, -1.0f,     0.0f, 0.0f,
    1.0f, -1.0f,     1.0f, 0.0f,
    -1.0f,  1.0f,     0.0f, 1.0f,
    1.0f,  1.0f,     1.0f, 1.0f,
};

static gboolean on_render(GtkGLArea *area, GdkGLContext *context, gpointer user_data) {
    GLState *glstate = user_data;

    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);

    update_texture(glstate);

    static GLuint program = 0;
    static GLuint vao = 0;
    static GLuint vbo = 0;

    if (!program) {
        program = create_program();

        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*)0);

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*)(2 * sizeof(GLfloat)));
    } else {
        glBindVertexArray(vao);
    }

    glUseProgram(program);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, glstate->texture);
    glUniform1i(glGetUniformLocation(program, "tex"), 0);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    return TRUE;
}

void set_pixel_state(int x, int y, int isOn) {
    if (x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT)
        return;
    pixels[y][x] = isOn ? 255 : 0;
}

int is_pixel_on(int x, int y) {
    if (x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT)
        return 0;
    return (pixels[y][x] == 255);
}

static gboolean on_timeout(gpointer user_data) {
    GtkGLArea *glarea = GTK_GL_AREA(user_data);

    gtk_gl_area_queue_render(glarea);

    return TRUE;
}

static void on_realize(GtkGLArea *area, gpointer user_data) {
    gtk_gl_area_make_current(area);

    if (gtk_gl_area_get_error(area) != NULL) {
        g_warning("Failed to create GL context");
        return;
    }

    GLState *glstate = user_data;
    init_texture(glstate);
}

static void on_unrealize(GtkGLArea *area, gpointer user_data) {
    GLState *glstate = user_data;

    gtk_gl_area_make_current(area);

    if (glstate->texture) {
        glDeleteTextures(1, &glstate->texture);
        glstate->texture = 0;
    }
}

static void on_window_destroy(GtkWidget *widget, gpointer user_data) {
    GMainLoop *loop = user_data;
    g_main_loop_quit(loop);
}

void* com_thread(void *arg)
{
    unlink("./com");
    mkfifo("./com", 0777);
    printf("created fifo!\n");
    int comfd = open("./com", O_RDONLY);
    printf("opened fd %d!\n", comfd);
    uint8_t payload[3];

    while(1)
    {
        ssize_t rsize = read(comfd, payload, 3);
        if(rsize > 2)
        {
            set_pixel_state(payload[0], payload[1], payload[2]);
        }
    }
}

int main(int argc, char *argv[]) {
    gtk_init();

    for (int y = 0; y < HEIGHT; y++)
        for (int x = 0; x < WIDTH; x++)
            pixels[y][x] = 0;

    GtkWidget *window = gtk_window_new();
    gtk_window_set_title(GTK_WINDOW(window), "DisplayUI");
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 800);

    GtkGLArea *glarea = GTK_GL_AREA(gtk_gl_area_new());

    GLState glstate = {0};

    GtkWidget *aspect_frame = gtk_aspect_frame_new(0.5, 0.5, (gfloat)WIDTH / HEIGHT, TRUE);
    gtk_aspect_frame_set_child(GTK_ASPECT_FRAME(aspect_frame), GTK_WIDGET(glarea));
    gtk_window_set_child(GTK_WINDOW(window), aspect_frame);

    g_signal_connect(glarea, "realize", G_CALLBACK(on_realize), &glstate);
    g_signal_connect(glarea, "unrealize", G_CALLBACK(on_unrealize), &glstate);
    g_signal_connect(glarea, "render", G_CALLBACK(on_render), &glstate);
    g_signal_connect(window, "destroy", G_CALLBACK(on_window_destroy), NULL);

    gtk_window_present(GTK_WINDOW(window));

    g_timeout_add(1, on_timeout, glarea);  // ~60 FPS

    pthread_t thread;
    pthread_create(&thread, NULL, com_thread, NULL);

    GMainLoop *loop = g_main_loop_new(NULL, FALSE);
    g_main_loop_run(loop);
    g_main_loop_unref(loop);

    return 0;
}
