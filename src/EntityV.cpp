#include "EntityV.h"

EntityV::EntityV(glm::vec3& initialPosition, glm::vec3& initialScale, float angleP, glm::vec3 axisP, Shader& initialShader, const char* shape)
	:Entity(initialPosition,
		initialScale,
		initialShader),
	angle(angleP),
	axis(axisP)
{
	getVertexData(shape);

	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	glGenBuffers(1, &m_vbo);

	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(vertexPos[0]), &vertexPos[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);

	glGenBuffers(1, &m_nbo);

	glBindBuffer(GL_ARRAY_BUFFER, m_nbo);
	glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(vertexNorms[0]), &vertexNorms[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);

	glGenBuffers(1, &m_tbo);

	glBindBuffer(GL_ARRAY_BUFFER, m_tbo);
	glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(vertexUv[0]), &vertexUv[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);



	glGenBuffers(1, &m_ibo);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, numIndices * sizeof(m_indices[0]), &m_indices[0], GL_STATIC_DRAW);

	glBindVertexArray(0);
}

void EntityV::draw(float deltaTime, Camera& cam, bool instanced, float elapsedTime, lightDir dLight, std::vector<lightPoint>& lightPoints, glm::mat4 lightSpaceMatrix)
{
	glm::mat4 projection = cam.GetProjectionMatrix();
	glm::mat4 view = cam.GetViewMatrix();

	m_shader.use();
	//m_shader.setInt("texture_diffuse1", 0);
	//m_shader.setInt("shadowMap", 2);

	m_shader.setMat4("projection", projection);
	m_shader.setMat4("view", view);

	//Lighting setup
	m_shader.setVec3("viewPos", cam.m_cameraPos);
	m_shader.setVec3("dirLight.direction", dLight.m_direction);
	m_shader.setVec3("dirLight.color", dLight.m_color);
	m_shader.setMat4("lightSpaceMatrix", lightSpaceMatrix);



	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, m_position); // translate it down so it's at the center of the scene
	model = glm::rotate(model, glm::radians(angle), axis);
	model = glm::scale(model, m_scale);	// it's a bit too big for our scene, so scale it down
	m_shader.setMat4("model", model);

	glBindVertexArray(m_vao);
	glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, nullptr);
	glBindVertexArray(0);

}

void EntityV::drawDirLight(float deltaTime, Camera& cam, float elapsedTime, lightDir dLight, Shader& shader)
{
	shader.use();
	shader.setInt("texture_diffuse1", 0);
	shader.setInt("shadowMap", 2);
	

	//Lighting setup


	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, m_position); // translate it down so it's at the center of the scene
	model = glm::rotate(model, glm::radians(angle), axis);
	model = glm::scale(model, m_scale);	// it's a bit too big for our scene, so scale it down
	shader.setMat4("model", model);

	glBindVertexArray(m_vao);
	glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, nullptr);
	glBindVertexArray(0);
}

void EntityV::getVertexData(const char* shape)
{
	m_shape = m_shape->FetchShape(shape);
	m_shape->InitVertexData();
	numVertices = m_shape->GetVertexDataCount();
	numIndices = m_shape->GetIndexDataCount();
	m_indices = m_shape->GetIndexData();

	for (size_t i = 0; i < numVertices; ++i)
	{
		vertexPos.push_back(m_shape->GetVertexData()[i].GetPos());
		vertexUv.push_back(m_shape->GetVertexData()[i].GetTex());
		vertexNorms.push_back(m_shape->GetVertexData()[i].GetNormals());
	}
}
