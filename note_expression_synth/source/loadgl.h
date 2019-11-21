#pragma once

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "GLSL.h"
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <fstream>

class ssbo_data
{
public:
	glm::vec4 dataA[1024];
	glm::vec4 dataB[1024];
};

class uniform_data {
	public:
		double freq;
		double q;
		int filtertype;
		double samplerate;
		double in1;
};

class Loadgl {
	public:

		uniform_data uniforms;
		GLuint uniformdata;

		static Loadgl* Instance();

		GLuint computeProgram;

		void setVars(double freqf, double qf, int filtertypef, double sampleratef, double in1f) {
			uniforms.freq = freqf;
			uniforms.q = qf;
			uniforms.filtertype = filtertypef;
			uniforms.samplerate = sampleratef;
			uniforms.in1 = in1f;
		}

		std::string readFileAsString(const std::string &fileName)
		{
			std::string result;
			std::ifstream fileHandle(fileName);
			if (fileHandle) {


				fileHandle.seekg(0, std::ios::end);
				result.reserve((size_t)fileHandle.tellg());
				fileHandle.seekg(0, std::ios::beg);

				result.assign((std::istreambuf_iterator<char>(fileHandle)), std::istreambuf_iterator<char>());
			}
			return result;
		}

		void compute(ssbo_data* data) {

			glGenBuffers(1, &ssbo_GPU_id);
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo_GPU_id);
			glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(ssbo_data), data, GL_DYNAMIC_COPY);
			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo_GPU_id);
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0); // unbind

			GLuint block_index = 0;
			block_index = glGetProgramResourceIndex(computeProgram, GL_SHADER_STORAGE_BLOCK, "shader_data");
			GLuint ssbo_binding_point_index = 0;
			glShaderStorageBlockBinding(computeProgram, block_index, ssbo_binding_point_index);
			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo_GPU_id);
			glUseProgram(computeProgram);

			glGenBuffers(1, &uniformdata);
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, uniformdata);
			glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(uniform_data), &uniforms, GL_DYNAMIC_COPY);
			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, uniformdata);
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0); // unbind

			GLuint block_inde = 1;
			block_inde = glGetProgramResourceIndex(computeProgram, GL_SHADER_STORAGE_BLOCK, "uniform_data");
			GLuint ssbo_binding_point_inde = 1;
			glShaderStorageBlockBinding(computeProgram, block_inde, ssbo_binding_point_inde);
			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, uniformdata);
			glUseProgram(computeProgram);

			glDispatchCompute((GLuint)1024, (GLuint)1, 1);				//start compute shader
			glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, 0);

			//copy data back to CPU MEM

			glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo_GPU_id);
			GLvoid* p = glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);
			int siz = sizeof(ssbo_data);
			memcpy(data, p, siz);
			glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

		}

		void init()
		{
			glfwInit();
			GLFWwindow* window = glfwCreateWindow(32, 32, "Dummy", nullptr, nullptr);
			glfwMakeContextCurrent(window);
			gladLoadGL();

			int work_grp_cnt[3];

			glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &work_grp_cnt[0]);
			glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &work_grp_cnt[1]);
			glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &work_grp_cnt[2]);

			GLSL::checkVersion();
			//load the compute shader
			std::string ShaderString = readFileAsString("D:/Documents/School/snrproj/vstsdk3613_08_04_2019_build_81/VST_SDK/VST3_SDK/public.sdk/samples/vst/note_expression_synth/resource/compute.glsl");
			const char *shader = ShaderString.c_str();
			GLuint computeShader = glCreateShader(GL_COMPUTE_SHADER);
			glShaderSource(computeShader, 1, &shader, nullptr);

			GLint rc;
			CHECKED_GL_CALL(glCompileShader(computeShader));
			CHECKED_GL_CALL(glGetShaderiv(computeShader, GL_COMPILE_STATUS, &rc));
			if (!rc)	//error compiling the shader file
			{
				//	GLSL::printShaderInfoLog(computeShader);
				//	std::cout << "Error compiling fragment shader " << std::endl;
				exit(1);
			}

			computeProgram = glCreateProgram();
			glAttachShader(computeProgram, computeShader);
			glLinkProgram(computeProgram);
			glUseProgram(computeProgram);

			GLuint block_index = 0;
			block_index = glGetProgramResourceIndex(computeProgram, GL_SHADER_STORAGE_BLOCK, "shader_data");
			GLuint ssbo_binding_point_index = 2;
			glShaderStorageBlockBinding(computeProgram, block_index, ssbo_binding_point_index);

		}
		ssbo_data ssbo_CPUMEM;
		GLuint ssbo_GPU_id;

	private:

		Loadgl() {};
		static Loadgl* myInstance;
};