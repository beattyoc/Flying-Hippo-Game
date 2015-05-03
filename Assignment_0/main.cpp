#include <GL/glew.h> // include GLEW and new version of GL on Windows
#include <GLFW/glfw3.h> // GLFW helper library
#include <stdio.h>
#include <fstream>
#include <string>
#include <iostream>
#include <assert.h>
#include <time.h>
#include <cmath>
#include <cstdio>
#include <ctime>
#include <sstream>
#include <stdarg.h>
#include "Antons_Mathsf.h"
#include "obj_parser.h"
#include "stb_image.h"
#include "text.h"



using namespace std;
#include <Windows.h>
#include <mmsystem.h>

int g_gl_width = 640;
int g_gl_height = 480;


//float deg_to_rad(float deg){ return deg * 0.0174532925f; }

static void key_action(GLFWwindow* window, int key, int scancode, int action, int mods); //deals with keyboard functionality
void load_texture(const char* file_name, GLuint* texture);
bool parse_file_into_str(const char* file_name, char* shader_str, int max_len);
void _update_fps_counter(GLFWwindow* window);
void create_cube_map(const char* front, const char* back, const char* top, const char* bottom, const char* left, const char* right, GLuint* tex_cube);
bool load_cube_map_side(GLuint texture, GLenum side_target, const char* file_name);
float round_n(float f, float prec);
//void output(int x, int y, float r, float g, float b, int font, char *string);

const char* atlas_image = "freemono.png";
const char* atlas_meta = "freemono.meta";

int main() {

	// start GL context and O/S window using the GLFW helper library
	if (!glfwInit()) {
		fprintf(stderr, "ERROR: could not start GLFW3\n");
		return 1;
	}

	GLFWwindow* window = glfwCreateWindow(640, 480, "Scene", NULL, NULL);
	if (!window) {
		fprintf(stderr, "ERROR: could not open window with GLFW3\n");
		glfwTerminate();
		return 1;
	}
	glfwMakeContextCurrent(window);

	// start GLEW extension handler
	glewExperimental = GL_TRUE;
	glewInit();

	// get version info
	const GLubyte* renderer = glGetString(GL_RENDERER); // get renderer string
	const GLubyte* version = glGetString(GL_VERSION); // version as a string
	printf("Renderer: %s\n", renderer);
	printf("OpenGL version supported %s\n", version);

	// tell GL to only draw onto a pixel if the shape is closer to the viewer
	glEnable(GL_DEPTH_TEST); // enable depth-testing
	glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"



	int point_count_hippo = 0; // number of vertex points loaded
	GLfloat* vp_hippo = NULL; // array of vertex points
	GLfloat* vn_hippo = NULL; // array of vertex normals (not needed for assignment)
	GLfloat* vt_hippo = NULL; // array of texture coordinates
	
	load_obj_file("hippo.obj", vp_hippo, vt_hippo, vn_hippo, point_count_hippo);	// loading mesh --------------------------------------

	int point_count_fence = 0; 
	GLfloat* vp_fence = NULL; 
	GLfloat* vn_fence = NULL; 
	GLfloat* vt_fence = NULL; 

	load_obj_file("fence.obj", vp_fence, vt_fence, vn_fence, point_count_fence);
	
	int point_count_wingR = 0;
	GLfloat* vp_wingR = NULL;
	GLfloat* vn_wingR = NULL;
	GLfloat* vt_wingR = NULL;

	load_obj_file("wing.obj", vp_wingR, vt_wingR, vn_wingR, point_count_wingR);

	int point_count_wingL = 0;
	GLfloat* vp_wingL = NULL;
	GLfloat* vn_wingL = NULL;
	GLfloat* vt_wingL = NULL;

	load_obj_file("wingL.obj", vp_wingL, vt_wingL, vn_wingL, point_count_wingL);

	int point_count_can = 0;
	GLfloat* vp_can = NULL;
	GLfloat* vn_can = NULL;
	GLfloat* vt_can = NULL;

	load_obj_file("can.obj", vp_can, vt_can, vn_can, point_count_can);
	
	float points_sky[] = {
		-50.0f, 50.0f, -50.0f,
		-50.0f, -50.0f, -50.0f,
		50.0f, -50.0f, -50.0f,
		50.0f, -50.0f, -50.0f,
		50.0f, 50.0f, -50.0f,
		-50.0f, 50.0f, -50.0f,

		-50.0f, -50.0f, 50.0f,
		-50.0f, -50.0f, -50.0f,
		-50.0f, 50.0f, -50.0f,
		-50.0f, 50.0f, -50.0f,
		-50.0f, 50.0f, 50.0f,
		-50.0f, -50.0f, 50.0f,

		50.0f, -50.0f, -50.0f,
		50.0f, -50.0f, 50.0f,
		50.0f, 50.0f, 50.0f,
		50.0f, 50.0f, 50.0f,
		50.0f, 50.0f, -50.0f,
		50.0f, -50.0f, -50.0f,

		-50.0f, -50.0f, 50.0f,
		-50.0f, 50.0f, 50.0f,
		50.0f, 50.0f, 50.0f,
		50.0f, 50.0f, 50.0f,
		50.0f, -50.0f, 50.0f,
		-50.0f, -50.0f, 50.0f,

		-50.0f, 50.0f, -50.0f,
		50.0f, 50.0f, -50.0f,
		50.0f, 50.0f, 50.0f,
		50.0f, 50.0f, 50.0f,
		-50.0f, 50.0f, 50.0f,
		-50.0f, 50.0f, -50.0f,

		-50.0f, -50.0f, -50.0f,
		-50.0f, -50.0f, 50.0f,
		50.0f, -50.0f, -50.0f,
		50.0f, -50.0f, -50.0f,
		-50.0f, -50.0f, 50.0f,
		50.0f, -50.0f, 50.0f
	};
	
	//
	//
	// VBOs & VAO
	//--------------------------------------------------------------------------------
	//
	//

	GLuint vbo_sky;
	glGenBuffers(1, &vbo_sky);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_sky);
	glBufferData(GL_ARRAY_BUFFER, 3 * 36 * sizeof(float), &points_sky, GL_STATIC_DRAW);

	GLuint vao_sky;
	glGenVertexArrays(1, &vao_sky);
	glBindVertexArray(vao_sky);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_sky);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glEnableVertexAttribArray(3);

	GLuint tex_cube;
	create_cube_map("posz.png", "negz.png", "posy.png", "negy.png", "posx.png", "negx.png", &tex_cube);
	
	GLuint p_vbo_hippo = 0;
	glGenBuffers(1, &p_vbo_hippo);
	glBindBuffer(GL_ARRAY_BUFFER, p_vbo_hippo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 3 * point_count_hippo, vp_hippo, GL_STATIC_DRAW);

	GLuint t_vbo_hippo = 0;
	glGenBuffers(1, &t_vbo_hippo);
	glBindBuffer(GL_ARRAY_BUFFER, t_vbo_hippo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 2 * point_count_hippo, vt_hippo, GL_STATIC_DRAW);

	GLuint n_vbo_hippo = 0;
	glGenBuffers(1, &n_vbo_hippo);
	glBindBuffer(GL_ARRAY_BUFFER, n_vbo_hippo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 3 * point_count_hippo, vn_hippo, GL_STATIC_DRAW);

	GLuint vao_hippo = 0;
	glGenVertexArrays(1, &vao_hippo);
	glBindVertexArray(vao_hippo);

	glBindBuffer(GL_ARRAY_BUFFER, p_vbo_hippo);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glBindBuffer(GL_ARRAY_BUFFER, t_vbo_hippo);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glBindBuffer(GL_ARRAY_BUFFER, n_vbo_hippo);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	GLuint texture_hippo;
	load_texture("pink.png", &texture_hippo);	//loading texture ------------------------------------------
	
	GLuint p_vbo_fence = 0;
	glGenBuffers(1, &p_vbo_fence);
	glBindBuffer(GL_ARRAY_BUFFER, p_vbo_fence);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 3 * point_count_fence, vp_fence, GL_STATIC_DRAW);

	GLuint t_vbo_fence = 0;
	glGenBuffers(1, &t_vbo_fence);
	glBindBuffer(GL_ARRAY_BUFFER, t_vbo_fence);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 2 * point_count_fence, vt_fence, GL_STATIC_DRAW);

	GLuint n_vbo_fence = 0;
	glGenBuffers(1, &n_vbo_fence);
	glBindBuffer(GL_ARRAY_BUFFER, n_vbo_fence);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 3 * point_count_fence, vn_fence, GL_STATIC_DRAW);

	GLuint vao_fence = 0;
	glGenVertexArrays(1, &vao_fence);
	glBindVertexArray(vao_fence);

	glBindBuffer(GL_ARRAY_BUFFER, p_vbo_fence);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glBindBuffer(GL_ARRAY_BUFFER, t_vbo_fence);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glBindBuffer(GL_ARRAY_BUFFER, n_vbo_fence);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	GLuint texture_fence;
	load_texture("grain.png", &texture_fence);	//loading texture ------------------------------------------
	
	GLuint p_vbo_wingR = 0;
	glGenBuffers(1, &p_vbo_wingR);
	glBindBuffer(GL_ARRAY_BUFFER, p_vbo_wingR);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 3 * point_count_wingR, vp_wingR, GL_STATIC_DRAW);

	GLuint t_vbo_wingR = 0;
	glGenBuffers(1, &t_vbo_wingR);
	glBindBuffer(GL_ARRAY_BUFFER, t_vbo_wingR);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 2 * point_count_wingR, vt_wingR, GL_STATIC_DRAW);

	GLuint n_vbo_wingR = 0;
	glGenBuffers(1, &n_vbo_wingR);
	glBindBuffer(GL_ARRAY_BUFFER, n_vbo_wingR);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 3 * point_count_wingR, vn_wingR, GL_STATIC_DRAW);

	GLuint vao_wingR = 0;
	glGenVertexArrays(1, &vao_wingR);
	glBindVertexArray(vao_wingR);

	glBindBuffer(GL_ARRAY_BUFFER, p_vbo_wingR);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glBindBuffer(GL_ARRAY_BUFFER, t_vbo_wingR);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glBindBuffer(GL_ARRAY_BUFFER, n_vbo_wingR);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	GLuint texture_wingR;
	load_texture("gold.png", &texture_wingR);	//loading texture ------------------------------------------

	GLuint p_vbo_wingL = 0;
	glGenBuffers(1, &p_vbo_wingL);
	glBindBuffer(GL_ARRAY_BUFFER, p_vbo_wingL);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 3 * point_count_wingL, vp_wingL, GL_STATIC_DRAW);

	GLuint t_vbo_wingL = 0;
	glGenBuffers(1, &t_vbo_wingL);
	glBindBuffer(GL_ARRAY_BUFFER, t_vbo_wingL);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 2 * point_count_wingL, vt_wingL, GL_STATIC_DRAW);

	GLuint n_vbo_wingL = 0;
	glGenBuffers(1, &n_vbo_wingL);
	glBindBuffer(GL_ARRAY_BUFFER, n_vbo_wingL);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 3 * point_count_wingL, vn_wingL, GL_STATIC_DRAW);

	GLuint vao_wingL = 0;
	glGenVertexArrays(1, &vao_wingL);
	glBindVertexArray(vao_wingL);

	glBindBuffer(GL_ARRAY_BUFFER, p_vbo_wingL);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glBindBuffer(GL_ARRAY_BUFFER, t_vbo_wingL);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glBindBuffer(GL_ARRAY_BUFFER, n_vbo_wingL);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	GLuint texture_wingL;
	load_texture("gold.png", &texture_wingL);	//loading texture ------------------------------------------

	GLuint p_vbo_can = 0;
	glGenBuffers(1, &p_vbo_can);
	glBindBuffer(GL_ARRAY_BUFFER, p_vbo_can);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 3 * point_count_can, vp_can, GL_STATIC_DRAW);

	GLuint t_vbo_can = 0;
	glGenBuffers(1, &t_vbo_can);
	glBindBuffer(GL_ARRAY_BUFFER, t_vbo_can);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 2 * point_count_can, vt_can, GL_STATIC_DRAW);

	GLuint n_vbo_can = 0;
	glGenBuffers(1, &n_vbo_can);
	glBindBuffer(GL_ARRAY_BUFFER, n_vbo_can);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 3 * point_count_can, vn_can, GL_STATIC_DRAW);

	GLuint vao_can = 0;
	glGenVertexArrays(1, &vao_can);
	glBindVertexArray(vao_can);

	glBindBuffer(GL_ARRAY_BUFFER, p_vbo_can);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glBindBuffer(GL_ARRAY_BUFFER, t_vbo_can);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glBindBuffer(GL_ARRAY_BUFFER, n_vbo_can);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	GLuint texture_can;
	load_texture("redbull.png", &texture_can);	//loading texture ------------------------------------------

	//
	//
	// Shaders skybox + meshes
	//------------------------------------------------------------------------------------
	//
	//
	char* skyVS;
	char* skyFS;

	// allocate some memory to store shader strings
	skyVS = (char*)malloc(81920);
	skyFS = (char*)malloc(81920);

	// load shader strings from text files
	assert(parse_file_into_str("sky_vs.vert", skyVS, 81920));
	assert(parse_file_into_str("sky_fs.frag", skyFS, 81920));
	GLuint vs_sky, fs_sky;
	vs_sky = glCreateShader(GL_VERTEX_SHADER);
	fs_sky = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(vs_sky, 1, (const char**)&skyVS, NULL);
	glShaderSource(fs_sky, 1, (const char**)&skyFS, NULL);

	// free memory
	free(skyVS);
	free(skyFS);
	glCompileShader(vs_sky);
	glCompileShader(fs_sky);
	GLuint shader_programme_sky = glCreateProgram();
	glAttachShader(shader_programme_sky, vs_sky);
	glAttachShader(shader_programme_sky, fs_sky);
	glLinkProgram(shader_programme_sky);


	char* vertex_shader_str;
	char* fragment_shader_str;

	// allocate some memory to store shader strings
	vertex_shader_str = (char*)malloc(81920);
	fragment_shader_str = (char*)malloc(81920);

	// load shader strings from text files
	assert(parse_file_into_str("vertex_shader.vert", vertex_shader_str, 81920));
	assert(parse_file_into_str("fragment_shader.frag", fragment_shader_str, 81920));
	GLuint vs, fs;
	vs = glCreateShader(GL_VERTEX_SHADER);
	fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(vs, 1, (const char**)&vertex_shader_str, NULL);
	glShaderSource(fs, 1, (const char**)&fragment_shader_str, NULL);

	// free memory
	free(vertex_shader_str);
	free(fragment_shader_str);
	glCompileShader(vs);
	glCompileShader(fs);
	GLuint shader_programme = glCreateProgram();
	glAttachShader(shader_programme, fs);
	glAttachShader(shader_programme, vs);
	glLinkProgram(shader_programme);


	//
	//
	// some matrices ------------------------------------------------------------------------------------------
	//
	//

	float cam_speed = 5.0f; // 1 unit per second
	float cam_yaw_speed = 10.0f; // 10 degrees per second
	float cam_pos[] = { 0.0f, 1.0f, 13.0f }; // don't start at zero, or we will be too close
	float cam_yaw = 0.0f; // y-rotation in degrees
	float cam_x = -15.0f;
	mat4 T = translate(identity_mat4(), vec3(-cam_pos[0], -cam_pos[1], -cam_pos[2]));
	mat4 R = (rotate_y_deg(identity_mat4(), -cam_yaw)) * (rotate_x_deg(identity_mat4(), -cam_x));
	mat4 view = R * T;

	int M_loc = glGetUniformLocation(shader_programme, "M");
	int V_loc = glGetUniformLocation(shader_programme, "V");
	int P_loc = glGetUniformLocation(shader_programme, "P");
	int T_loc = glGetUniformLocation(shader_programme, "basic_texture");

	int V_loc_sky = glGetUniformLocation(shader_programme_sky, "V");
	int P_loc_sky = glGetUniformLocation(shader_programme_sky, "P");
	int T_loc_sky = glGetUniformLocation(shader_programme_sky, "cube_texture");

	glUseProgram(shader_programme);
	glUseProgram(shader_programme_sky);

	float Sx = 0.0f, Sx_fence = 0.0f, Sy = -4.0f, Sz = 0.0f, Sz_fence = -6.0f, Sy_fence = -7.0f, Sy_can = -2.0f, Sx_can = 0.0f;
	mat4 model_hippo = (translate(identity_mat4(), vec3(Sx, Sy, Sz))); // *(rotate_y_deg(identity_mat4(), 90));
	mat4 model_fence = (translate(identity_mat4(), vec3(Sx_fence, Sy_fence, Sz_fence))); // *(rotate_y_deg(identity_mat4(), 90));
	mat4 model_wingR = (translate(identity_mat4(), vec3(Sx, Sy, Sz))); // *(rotate_y_deg(identity_mat4(), 90));
	mat4 model_wingL = (translate(identity_mat4(), vec3(Sx, Sy, Sz))); // *(rotate_y_deg(identity_mat4(), 90));
	mat4 model_can = (translate(identity_mat4(), vec3(Sx_can, Sy_can, Sz_fence))) * (rotate_x_deg(identity_mat4(), 30));
	//glEnable(GL_CULL_FACE); // cull face
	//glCullFace(GL_BACK); // cull back face
	//glFrontFace(GL_CW); // GL_CCW for counter clock-wise
	glUniformMatrix4fv(V_loc, 1, GL_FALSE, view.m);
	glUniformMatrix4fv(V_loc_sky, 1, GL_FALSE, view.m);

	//
	//
	// drawing time ---------------------------------------------------------------------------------------------------------
	//
	//

	//instructions

	cout << "\n\n - Move forwards = arrow up \n - Move back = arrow down \n - Move left ="
	" arrow left \n - Move right = arrow right \n - Rotate view left = L key \n - Rotate view"
		" right = R key \n - Move camera up = U key \n - Move camera down = D key \n - Rotate view "
		"down = Page Down \n - Rotate view up = Page Up \n\n";

	//
	// initialise font, load from files
	assert(init_text_rendering(atlas_image, atlas_meta, g_gl_width, g_gl_height));
	//int hello_id = add_text(
		//"Hello,\nis it me you're\nlooking for?",
		//-0.75f, 0.5f, 100.0f, 0.5f, 0.5f, 1.0f, 1.0f);

	int dist_id = add_text(
		"Distance: ",
		-1.0f, 1.0f, 75.0f, 1.0f, 0.0f, 0.0f, 1.0f);

	int life3_id = add_text("<3", 0.2f, 1.0f, 50.0f, 1.0f, 0.0f, 0.0f, 1.0f);
	int life2_id = add_text("<3", 0.3f, 1.0f, 50.0f, 1.0f, 0.0f, 0.0f, 1.0f);
	int life1_id = add_text("<3", 0.4f, 1.0f, 50.0f, 1.0f, 0.0f, 0.0f, 1.0f);
	int lose_id = add_text("YOU LOSE, 'S' to restart", -0.85f, 0.5f, 100.0f, 1.0f, 0.0f, 0.0f, 1.0f);
	int Go_id = add_text("Press 'S' to start...", -0.7f, 0.5f, 100.0f, 1.0f, 0.0f, 0.0f, 1.0f);

	float flap = 0.0, increment = 0.0, dist = 0.0;
	double tmp;
	bool hit = false, Go = false;
	int life = 3, points = 0, degrees = 0, rand_num = 0, rand_num2 = 0;

	//PlaySound(TEXT("jump_around.wav"), NULL, SND_FILENAME | SND_LOOP | SND_ASYNC);
	PlaySound(TEXT("elevator.wav"), NULL, SND_FILENAME | SND_LOOP | SND_ASYNC);

	while (!glfwWindowShouldClose(window)) {

		static double previous_seconds = glfwGetTime();
		double current_seconds = glfwGetTime();
		double elapsed_seconds = current_seconds - previous_seconds;
		previous_seconds = current_seconds;
		_update_fps_counter(window);

		// wipe the drawing surface clear
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		mat4 proj = perspective(45.0, (float)g_gl_width / (float)g_gl_height, 0.1, 100.0);
		
		glViewport(0, 0, g_gl_width, g_gl_height);
		
		//--- drawing the skybox
		glDepthMask(GL_FALSE);
		glUseProgram(shader_programme_sky);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, tex_cube);
		glBindVertexArray(vao_sky);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glDepthMask(GL_TRUE);
		glUniformMatrix4fv(V_loc_sky, 1, GL_FALSE, view.m);
		glUniformMatrix4fv(P_loc_sky, 1, GL_FALSE, proj.m);

		//-- drawing hippo mesh
		glUseProgram(shader_programme);
		glBindVertexArray(vao_hippo);
		glUniformMatrix4fv(M_loc, 1, GL_FALSE, model_hippo.m);
		glUniformMatrix4fv(V_loc, 1, GL_FALSE, view.m);
		glUniformMatrix4fv(P_loc, 1, GL_FALSE, proj.m);
		glBindTexture(GL_TEXTURE_2D, texture_hippo);
		glDrawArrays(GL_TRIANGLES, 0, point_count_hippo);

		//--- drawing fence mesh
		glBindVertexArray(vao_fence);
		glUniformMatrix4fv(M_loc, 1, GL_FALSE, model_fence.m);
		glBindTexture(GL_TEXTURE_2D, texture_fence);
		glDrawArrays(GL_TRIANGLES, 0, point_count_fence);

		//drawing the <3's
		if (life > -1) {
			char tmp2[256];
			sprintf(tmp2, "Distance: %lfm\n", dist);
			update_text(dist_id, tmp2);
			change_text_colour(dist_id, 1.0f, 1.0f, 0.0f, 1.0f);
			if (Go) {
				sprintf(tmp2, "", NULL);
				update_text(Go_id, tmp2);
			}
			if (life < 3) {
				sprintf(tmp2, "", NULL);
				update_text(life3_id, tmp2);	
			}
			if (life < 2){
				sprintf(tmp2, "", NULL);
				update_text(life2_id, tmp2);
			}
	
			if (life < 1){
				sprintf(tmp2, "", NULL);
				update_text(life1_id, tmp2);
			}		
		}

		// draw all the texts
		draw_texts();

		glUseProgram(shader_programme);

		// collision detection conditions between can and hippo
		if (((Sx - Sx_can) < 0.7 && (Sx - Sx_can) > -0.7) && ((Sy - Sy_can) < 0.7 && (Sy - Sy_can) >= 0.0) && ((Sz - Sz_fence) < 0.7 && (Sz - Sz_fence) > -0.7)) {
			cout << "CAN! \n";
			hit = true;
		}

		//re draw fence + can when distance between hippo and fence is above 10
		if ((Sz_fence - (Sz)) > 10) { 
			if (!hit) {
				//PlaySound(TEXT("ohno.wav"), NULL, SND_FILENAME | SND_LOOP | SND_ASYNC);
				life--;
				cout << life;
				cam_speed --;
			}
			//else points++;
			cam_speed++;
			hit = false;
			Sz = 0.0;

			//can is randomly given a position on the x-axis
			rand_num = rand() % 3;
			if (rand_num == 0)				
				Sx_can = rand() % 3 + 1;
			else if (rand_num == 1)
				Sx_can = (-1)*(rand() % 3 + 1);
			else Sx_can = 0.0;

			cam_pos[2] = 13.0;
			Sz_fence = (Sz) - 17;
			model_fence = (translate(identity_mat4(), vec3(Sx_fence, Sy_fence, Sz_fence)));// *(rotate_y_deg(identity_mat4(), 90));
			model_can = (translate(identity_mat4(), vec3(Sx_can, Sy_can, Sz_fence)) * (rotate_x_deg(identity_mat4(), 30)) * (rotate_y_deg(identity_mat4(), increment)));
		}

		//--- drawing right wing
		glBindVertexArray(vao_wingR);
		glUniformMatrix4fv(M_loc, 1, GL_FALSE, model_wingR.m);
		glBindTexture(GL_TEXTURE_2D, texture_wingR);
		glDrawArrays(GL_TRIANGLES, 0, point_count_wingR);

		//--- drawing the left wing
		glBindVertexArray(vao_wingL);
		glUniformMatrix4fv(M_loc, 1, GL_FALSE, model_wingL.m);
		glBindTexture(GL_TEXTURE_2D, texture_wingL);
		glDrawArrays(GL_TRIANGLES, 0, point_count_wingL);

		//--- draw the redbull can
		if (hit == false) {
			glBindVertexArray(vao_can);
			glUniformMatrix4fv(M_loc, 1, GL_FALSE, model_can.m);
			glBindTexture(GL_TEXTURE_2D, texture_can);
			glDrawArrays(GL_TRIANGLES, 0, point_count_can);
		}

		//--- moving hippo and wings and camera
		if (Go) {
			Sz -= cam_speed * elapsed_seconds;
			cam_pos[2] -= cam_speed * elapsed_seconds;
			dist += cam_speed * elapsed_seconds;
		}
		T = translate(identity_mat4(), vec3(-cam_pos[0], -cam_pos[1], -cam_pos[2])); // cam translation
		R = (rotate_y_deg(identity_mat4(), -cam_yaw)) * (rotate_x_deg(identity_mat4(), -cam_x)); // 
		view = R * T;
		model_hippo = (translate(identity_mat4(), vec3(Sx, Sy, Sz)));// *(rotate_y_deg(identity_mat4(), 90));
		model_wingR = (translate(identity_mat4(), vec3(Sx, Sy, Sz))) * (rotate_z_deg(identity_mat4(), flap));// *(rotate_y_deg(identity_mat4(), 90));
		model_wingL = (translate(identity_mat4(), vec3(Sx, Sy, Sz))) *(rotate_z_deg(identity_mat4(), -flap));// *(rotate_y_deg(identity_mat4(), 90));
		flap += 20 * sin(15 * current_seconds)*cam_speed * elapsed_seconds; //wing flap rate
		model_can = (translate(identity_mat4(), vec3(Sx_can, Sy_can, Sz_fence))  * (rotate_y_deg(identity_mat4(), increment))) * (rotate_x_deg(identity_mat4(), 30));
		increment += 0.25;


		//while still alive
		if (life > -1) {

			//Initiates jump
			if (glfwGetKey(window, GLFW_KEY_SPACE)) {
				//PlaySound(TEXT("boing.wav"), NULL, SND_FILENAME | SND_LOOP | SND_ASYNC);
				cout << "JUMP!";
				tmp = glfwGetTime();
				Sy = -4.9;		
				degrees = 0;
				rand_num2 = rand() % 2;
			}
		}


		if (!Go) {
			char tmp2[256];
			sprintf(tmp2, "", NULL);
			update_text(lose_id, tmp2);
		}

		//if a restart is called
		if (glfwGetKey(window, GLFW_KEY_S)) {
			dist = 0;
			if (Go) {
				cam_speed = 5.0f;
				life = 3;
				cam_pos[2] = 13.0f;
				cam_pos[0] = 0.0f;
				Sx = 0.0f;
				Sy = -0.4f;
				Sz = 0.0f;
				char tmp2[256];
				sprintf(tmp2, "<3", NULL);
				update_text(life1_id, tmp2);
				update_text(life2_id, tmp2);
				update_text(life3_id, tmp2);
				sprintf(tmp2, "", NULL);
				update_text(lose_id, tmp2);
				bool cam_moved = true;
			}
			if (!Go)
				Go = true;
		}

		//after a jump is inititated
		if (Sy > -5) {	
			degrees++;
			cout << " ";
			current_seconds = glfwGetTime();
			double Time = current_seconds - tmp;
			Sy = (4 * ((5.0 *Time*sin(45)) - (0.5*9.81*(Time*Time)))) - 5;

			//random twirl or tumble
			if (rand_num2 == 0) {
				model_hippo = (translate(identity_mat4(), vec3(Sx, Sy, Sz))) * (rotate_y_deg(identity_mat4(), -degrees));// *(rotate_x_deg(identity_mat4(), -increment)); // *(rotate_y_deg(identity_mat4(), 90));
				model_wingR = (translate(identity_mat4(), vec3(Sx, Sy, Sz))) * (rotate_y_deg(identity_mat4(), -degrees)) * (rotate_z_deg(identity_mat4(), flap)); // *(rotate_y_deg(identity_mat4(), 90));
				model_wingL = (translate(identity_mat4(), vec3(Sx, Sy, Sz))) * (rotate_y_deg(identity_mat4(), -degrees)) * (rotate_z_deg(identity_mat4(), -flap));// *(rotate_y_deg(identity_mat4(), 90));
			}
			else if (rand_num2 == 1) {
				model_hippo = (translate(identity_mat4(), vec3(Sx, Sy, Sz))) * (rotate_x_deg(identity_mat4(), -degrees));// *(rotate_x_deg(identity_mat4(), -increment)); // *(rotate_y_deg(identity_mat4(), 90));
				model_wingR = (translate(identity_mat4(), vec3(Sx, Sy, Sz))) * (rotate_x_deg(identity_mat4(), -degrees)) * (rotate_z_deg(identity_mat4(), flap)); // *(rotate_y_deg(identity_mat4(), 90));
				model_wingL = (translate(identity_mat4(), vec3(Sx, Sy, Sz))) * (rotate_x_deg(identity_mat4(), -degrees)) * (rotate_z_deg(identity_mat4(), -flap));// *(rotate_y_deg(identity_mat4(), 90));
			}
		}
		else Sy = -5;


		//--- camera movements
		bool cam_moved = false;

		if (life > -1) {
			if (glfwGetKey(window, GLFW_KEY_LEFT)) {	//move left
				cam_pos[0] -= cam_speed * elapsed_seconds;
				if (Go)
					Sx -= cam_speed * elapsed_seconds;
				cam_moved = true;
			}
			if (glfwGetKey(window, GLFW_KEY_RIGHT)) {	//move right
				cam_pos[0] += cam_speed * elapsed_seconds;
				if (Go)
					Sx += cam_speed * elapsed_seconds;
				cam_moved = true;
			}
		}

		//update the view matrix when key is pressed
		if (cam_moved) {
			T = translate(identity_mat4(), vec3(-cam_pos[0], -cam_pos[1], -cam_pos[2])); // cam translation
			R = (rotate_y_deg(identity_mat4(), -cam_yaw)) * (rotate_x_deg(identity_mat4(), -cam_x)); // 
			view = R * T;
			glUniformMatrix4fv(V_loc, 1, GL_FALSE, view.m);
			glUniformMatrix4fv(V_loc_sky, 1, GL_FALSE, view.m);
		}
		if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_ESCAPE)) {	//to exit
			glfwSetWindowShouldClose(window, 1);
		}

		//if lose
		if (life <= -1) { 
			//int final_score = dist;
			//glfwSetWindowShouldClose(window, 1);
			//PlaySound(TEXT("trombone.wav"), NULL, SND_FILENAME | SND_LOOP | SND_ASYNC);
			char tmp2[256];
			sprintf(tmp2, "YOU LOSE, 'S' to restart", NULL);
			update_text(lose_id, tmp2);
			
		}

		glfwPollEvents();
		glfwSwapBuffers(window);
	}

	glfwTerminate();
	return 0;
}


//
//----------------------------------------------------------------------------------------------------------
//shader loader function
//

bool parse_file_into_str(const char* file_name, char* shader_str, int max_len) {
	FILE* file = fopen(file_name, "r");
	int current_len = 0;
	char line[2048];

	shader_str[0] = '\0'; /* reset string */
	if (!file) {
		fprintf(stderr, "ERROR: opening file for reading: %s\n", file_name);
		return false;
	}
	strcpy(line, ""); /* remember to clean up before using for first time! */
	while (!feof(file)) {
		if (NULL != fgets(line, 2048, file)) {
			current_len += strlen(line); /* +1 for \n at end */
			if (current_len >= max_len) {
				fprintf(stderr,
					"ERROR: shader length is longer than string buffer length %i\n",
					max_len
					);
			}
			strcat(shader_str, line);
		}
	}
	if (EOF == fclose(file)) { /* probably unnecesssary validation */
		fprintf(stderr, "ERROR: closing file from reading %s\n", file_name);
		return false;
	}
	return true;

}

//
//---------------------------------------------------------------------------------------------
//texture load function
//

void load_texture(const char* file_name, GLuint* texture){
	cout << "Loading";

	/*Load image*/
	int x, y, n;
	int force_channels = 4;	//rgba
	unsigned char* image_data = stbi_load(file_name, &x, &y, &n, force_channels);
	if (!image_data) {
		fprintf(stderr, "ERROR: could not load %s\n", file_name);
		return;
	}




	// Not Power Of Two check. Older Graphics cards can't deal with images that are not power-of-2 dimensions
	if ((x & (x - 1)) != 0 || (y & (y - 1)) != 0) {
		fprintf(stderr, "WARNING: texture %s is not power-of-2 dimensions\n", file_name);
	}

	/* If image is upside-down*/
	int width_in_bytes = x * 4;
	unsigned char *top = NULL;
	unsigned char *bottom = NULL;
	unsigned char temp = 0;
	int half_height = y / 2;
	/*The value of each pixel gets inverted about the x-axis through the centre of the image*/
	/* top row = bottom row*/
	for (int row = 0; row < half_height; row++) {
		top = image_data + row * width_in_bytes;
		bottom = image_data + (y - row - 1) * width_in_bytes;
		for (int col = 0; col < width_in_bytes; col++) {
			temp = *top;
			*top = *bottom;
			*bottom = temp;
			top++;
			bottom++;
		}
	}

	glGenTextures(1, texture);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, *texture);
	/*glTexImage2D — specifys a two-dimensional texture image*/
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	cout << "\nLeaving tex\n";
	return;
}

void _update_fps_counter(GLFWwindow* window) {		//frames per second
	static double previous_seconds = glfwGetTime();
	static int frame_count;
	double current_seconds = glfwGetTime();
	double elapsed_seconds = current_seconds - previous_seconds;
	if (elapsed_seconds > 0.25) {
		previous_seconds = current_seconds;
		double fps = (double)frame_count / elapsed_seconds;
		char tmp[128];
		sprintf(tmp, "opengl @ fps: %.2f", fps);
		glfwSetWindowTitle(window, tmp);
		frame_count = 0;
	}
	frame_count++;
}

void create_cube_map(const char* front, const char* back, const char* top, const char* bottom, const char* left, const char* right, GLuint* tex_cube) {
	// generate a cube-map texture to hold all the sides
	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, tex_cube);

	// load each image and copy into a side of the cube-map texture
	assert(load_cube_map_side(*tex_cube, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, front));
	assert(load_cube_map_side(*tex_cube, GL_TEXTURE_CUBE_MAP_POSITIVE_Z, back));
	assert(load_cube_map_side(*tex_cube, GL_TEXTURE_CUBE_MAP_POSITIVE_Y, top));
	assert(load_cube_map_side(*tex_cube, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, bottom));
	assert(load_cube_map_side(*tex_cube, GL_TEXTURE_CUBE_MAP_NEGATIVE_X, left));
	assert(load_cube_map_side(*tex_cube, GL_TEXTURE_CUBE_MAP_POSITIVE_X, right));
	// format cube map texture
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

bool load_cube_map_side(GLuint texture, GLenum side_target, const char* file_name) {
	glBindTexture(GL_TEXTURE_CUBE_MAP, texture);

	int x, y, n;
	int force_channels = 4;
	unsigned char*  image_data = stbi_load(
		file_name, &x, &y, &n, force_channels);
	if (!image_data) {
		fprintf(stderr, "ERROR: could not load %s\n", file_name);
		return false;
	}
	// non-power-of-2 dimensions check
	if ((x & (x - 1)) != 0 || (y & (y - 1)) != 0) {
		fprintf(stderr, "WARNING: image %s is not power-of-2 dimensions\n", file_name);
	}

	// copy image data into 'target' side of cube map
	glTexImage2D (side_target, 0, GL_RGBA, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
	free (image_data);
	return true;
}

float round_n(float f, float prec) {
	return (float)(floor(f*(1.0f / prec) + 0.5) / (1.0f / prec));
}

/*void output(int x, int y, float r, float g, float b, int font, char *string)
{
	glDisable(GL_TEXTURE_2D);
	glColor3f(r, g, b);
	glRasterPos2f(x, y);
	int len, i;
	len = (int)strlen(string);
	for (i = 0; i < len; i++) {
		glutBitmapCharacter(font, string[i]);
	}
	
	glEnable(GL_TEXTURE_2D);
}*/