#include "testGenerator.h"

#include <cmath>

testGenerator::testGenerator()
{
	octantStack.reserve(100);

	scalarGen.createShader("shaders/genScalarfield.vert", "shaders/genScalarfield.frag", "shaders/genScalarfield.geom");
	
	octantPosGen = glGetUniformLocation(scalarGen.programID, "octantPos");

	mcShader.createTransformShader("shaders/MC.vert", "shaders/MC.frag", "shaders/MC.geom");

	volumeTex = glGetUniformLocation(mcShader.programID, "scalarField");
	triTable = glGetUniformLocation(mcShader.programID, "triTable");
	octantPosMC = glGetUniformLocation(mcShader.programID, "octantPos");
}

testGenerator::~testGenerator(){

}

void testGenerator::generate(Octant* _ot, DynamicMesh* _dm) {

	//Generate scalar field ---------------------------------------
	glUseProgram(scalarGen.programID);

	glUniform3fv(octantPosGen, 1, &_ot->child[0]->pos[0]);

	glViewport(0, 0, 32, 32);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _dm->tmpFbo);
	glBindVertexArray(_dm->singlePointvao);

	glDrawArraysInstanced(GL_POINTS, 0, 1, std::pow(_dm->voxelRes, 2));

	GLubyte pixels[32 * 32 * 32];

	glBindTexture(GL_TEXTURE_3D, _dm->voxelTex);
	glGetTexImage(GL_TEXTURE_3D, 0, GL_RED, GL_UNSIGNED_BYTE, &pixels[0]);

	//glReadPixels(0, 0, 32, 32, GL_RED, GLenum  type, GLvoid *  data);

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glViewport(0, 0, 1920, 1080);

	// generate marching cubes on the GPU ------------------------------
	// bind mcShader
	glUseProgram(mcShader.programID);

	// bind textures
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, _dm->triTableTex);

	// send scalar field as 3D texture to the GPU ---------------------
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_3D, _dm->voxelTex);
	glTexImage3D(GL_TEXTURE_3D, 0, GL_RED, _dm->voxelRes, _dm->voxelRes, _dm->voxelRes, 0, GL_RED, GL_UNSIGNED_BYTE, _ot->voxelData);

	// send uniforms
	glUniform3fv(octantPosMC, 1, &_ot->child[0]->pos[0]);
	glUniform1i(volumeTex, 0);
	glUniform1i(triTable, 1);

	//start rendering with transform feedback
	//glBindBufferRange(GL_TRANSFORM_FEEDBACK_BUFFER, 0, _dm->vertexbuffer, 0, 10000 * sizeof(dBufferData));
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, _dm->vertexbuffer);

	glBindVertexArray(_dm->singlePointvao);

	GLuint qid;
	glGenQueries(1, &qid);

	glBeginQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN, qid);

	glEnable(GL_RASTERIZER_DISCARD);

	glBeginTransformFeedback(GL_TRIANGLES);
	glDrawArraysInstanced(GL_POINTS, 0, 1, std::pow(_dm->voxelRes - 2, 3));
	glPauseTransformFeedback();
	glDisable(GL_RASTERIZER_DISCARD);

	glEndQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN);
	GLuint nprimitives;
	glGetQueryObjectuiv(qid, GL_QUERY_RESULT, &nprimitives);
	_dm->nrofVerts = nprimitives;

	GLfloat feedback[5000];
	glGetBufferSubData(GL_TRANSFORM_FEEDBACK_BUFFER, 0, sizeof(feedback), feedback);
	for (int i = 0; i < 3 * 6 * 3; i = i + 3) {
		std::cout << feedback[i] << ", " <<
			feedback[i + 1] << ", " <<
			feedback[i + 2] << ", " << std::endl;
		if (i % 9 == 0)
			std::cout << "----------" << std::endl;

	}
	for(int i = 1; i < 8; i++) 
	{
		//Generate scalar field ---------------------------------------
		glUseProgram(scalarGen.programID);

		glUniform3fv(octantPosGen, 1, &_ot->child[i]->pos[0]);

		glViewport(0, 0, 32, 32);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _dm->tmpFbo);
		glBindVertexArray(_dm->singlePointvao);

		glDrawArraysInstanced(GL_POINTS, 0, 1, std::pow(_dm->voxelRes, 2));

		glBindTexture(GL_TEXTURE_3D, _dm->voxelTex);
		glGetTexImage(GL_TEXTURE_3D, 0, GL_RED, GL_UNSIGNED_BYTE, &pixels[0]);

		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		glViewport(0, 0, 1920, 1080);

		// generate marching cubes on the GPU ------------------------------
		//bind mcShader
		glUseProgram(mcShader.programID);

		// bind textures
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, _dm->triTableTex);

		// send scalar field as 3D texture to the GPU ---------------------
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_3D, _dm->voxelTex);
		glTexImage3D(GL_TEXTURE_3D, 0, GL_RED, _dm->voxelRes, _dm->voxelRes, _dm->voxelRes, 0, GL_RED, GL_UNSIGNED_BYTE, _ot->voxelData);

		// send uniforms
		glUniform3fv(octantPosMC, 1, &_ot->child[i]->pos[0]);
		glUniform1i(volumeTex, 0);
		glUniform1i(triTable, 1);

		//start rendering with transform feedback
		//glBindBufferRange(GL_TRANSFORM_FEEDBACK_BUFFER, 0, _dm->vertexbuffer, 0, 10000 * sizeof(dBufferData));
		//glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, _dm->vertexbuffer);

		glBindVertexArray(_dm->singlePointvao);

		glGenQueries(1, &qid);

		glBeginQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN, qid);

		glEnable(GL_RASTERIZER_DISCARD);

		glResumeTransformFeedback();
		glDrawArraysInstanced(GL_POINTS, 0, 1, std::pow(_dm->voxelRes - 2, 3));
		glPauseTransformFeedback();
		glDisable(GL_RASTERIZER_DISCARD);

		glEndQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN);

		glGetQueryObjectuiv(qid, GL_QUERY_RESULT, &nprimitives);
		_dm->nrofVerts = nprimitives;

		GLfloat feedback2[5000];
		glGetBufferSubData(GL_TRANSFORM_FEEDBACK_BUFFER, 0, sizeof(feedback2), feedback2);
		//for (int i = 0; i < 3 * 6 * 3; i = i + 3) {
		//	std::cout << feedback2[i] << ", " <<
		//		feedback2[i + 1] << ", " <<
		//		feedback2[i + 2] << ", " << std::endl;
		//	if (i % 9 == 0)
		//		std::cout << "----------" << std::endl;

		//}
	}

	glEndTransformFeedback();
	glBindVertexArray(0);
}
