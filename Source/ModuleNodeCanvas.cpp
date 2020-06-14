#include "Application.h"
#include "ModuleNodeCanvas.h"

#include "CanvasNode.h"
#include "ModuleGUI.h"
#include "ModuleParticles.h"
#include "ModuleCamera.h"
#include "BaseTransformEmitterNode.h"
#include "ModuleInput.h"

//Include all node types
#include "ColorParticleNode.h"
#include "SpeedParticleNode.h"
#include "MakeGlobalParticleNode.h"
#include "DeathInstantiationParticleNode.h"
#include "SpriteParticleNode.h"
#include "LifetimeParticleNode.h"
#include "TransformParticleNode.h"
#include "BlendModeParticleNode.h"
#include "SortingParticleNode.h"

#include "EmissionEmitterNode.h"
#include "ShapeEmitterNode.h"
#include "TransformEmitterNode.h"
#include "InputParticleEmitterNode.h"

ModuleNodeCanvas::ModuleNodeCanvas(bool start_enabled): Module(start_enabled)
{
}

ModuleNodeCanvas::~ModuleNodeCanvas()
{
}

bool ModuleNodeCanvas::Start()
{
	//Create fonts for canvas zooming
	for (float i = MIN_ZOOM*100; i <= MAX_ZOOM*100; i += ZOOM_STEPS*100)
	{
		App->gui->AddFont(i, CANVAS_FONT_SIZE);
		App->gui->AddFont(i, DEFAULT_FONT_SIZE);
	}

	return true;
}

update_state ModuleNodeCanvas::Update(float dt)
{	
	BROFILER_CATEGORY("ModuleNodeCanvas Update", Profiler::Color::Red);

	//Delete selected elements with 'supr'
	if (App->input->GetKey(SDL_SCANCODE_DELETE) == KEY_DOWN && canvasFocused)
	{
		if (selectedNode != nullptr) //TODO: Manage list
		{
			selectedNode->toDelete = true;
			selectedNode = nullptr;
		}
		if (selectedConnection != nullptr) //TODO: Manage list
		{
			selectedConnection->connected->Disconnect();
			selectedConnection->Disconnect();
			selectedConnection = nullptr;
		}
	}

	hoveredNode = newHoveredNode;
	newHoveredNode = nullptr; //For detecting new hovered node in this frame

	return UPDATE_CONTINUE;
}

update_state ModuleNodeCanvas::PostUpdate(float dt)
{
	BROFILER_CATEGORY("ModuleNodeCanvas Post-Update", Profiler::Color::Red);

	//Connect requested nodes
	if (connectionEnded)
	{
		connectionEnded = false;
		if (connectionCallback != nullptr && connecting != nullptr && connecting->type != connectionCallback->type)
		{
			connectionCallback->SetConnection(connecting);
			connecting->SetConnection(connectionCallback);

			if (connectionCallback->connected != connecting || connecting->connected != connectionCallback) //If one of them has not accepted connection, disconnect the other
			{
				connectionCallback->Disconnect();
				connecting->Disconnect();
			}
		}
		connecting = nullptr;
		connectionCallback = nullptr;
	}

	//Check node deletion
	std::list<CanvasNode*>::iterator it_n = nodes.begin();
	while (it_n != nodes.end())
	{
		if ((*it_n)->toDelete)
		{
			if (hoveredNode == (*it_n))
				hoveredNode = nullptr;
			if (selectedNode == (*it_n))
				selectedNode = nullptr;

			if ((*it_n)->type == EMITTER)
				App->particles->RemoveEmitter((ParticleEmitter*)(*it_n));

			RELEASE((*it_n));
			it_n = nodes.erase(it_n);
		}
		else
			++it_n;
	}

	//Clean node list filter
	if (!drawingNodeList && ((std::string)node_filter).size() > 0)
	{
		memcpy(&node_filter, "\0", 1);
	}
	drawingNodeList = false;

	return UPDATE_CONTINUE;
}

bool ModuleNodeCanvas::CleanUp()
{
	for (std::list<CanvasNode*>::iterator it_n = nodes.begin(); it_n != nodes.end(); ++it_n)
	{
		RELEASE((*it_n));
	}
	nodes.clear();

	return true;
}

void ModuleNodeCanvas::DrawGuizmo()
{
	if (selectedNode != nullptr && selectedNode->type == EMITTER_TRANSFORM && App->camera->type == CAMERA_3D)
	{
		TransformEmitterNode* transform = (TransformEmitterNode*)selectedNode;

		if (App->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_IDLE)
		{
			if (App->input->GetKey(SDL_SCANCODE_Q) == KEY_REPEAT)
			{
				guizmoOperation = ImGuizmo::NO_OPERATION;
			}
			if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT)
			{
				guizmoOperation = ImGuizmo::TRANSLATE;
			}
			if (App->input->GetKey(SDL_SCANCODE_E) == KEY_REPEAT)
			{
				guizmoOperation = ImGuizmo::ROTATE;
			}
			if (App->input->GetKey(SDL_SCANCODE_R) == KEY_REPEAT)
			{
				guizmoOperation = ImGuizmo::SCALE;
			}
		}

		if (guizmoOperation == ImGuizmo::NO_OPERATION)
		{
			ImGuizmo::Enable(false);
		}
		else
		{
			ImGuizmo::Enable(true);
		}

		ImVec2 cursorPos = { App->gui->scenePosition.x,App->gui->scenePosition.y };
		ImVec2 windowSize = { App->gui->sceneSize.x,App->gui->sceneSize.y };
		ImGuizmo::SetRect(cursorPos.x, cursorPos.y, windowSize.x, windowSize.y);

		float4x4* ViewMatrix = (float4x4*)App->camera->getViewMatrix();
		float4x4*ProjectionMatrix = (float4x4*)App->camera->getProjectionMatrix();

		float4x4* GlobalMat;
		GlobalMat = &transform->matrix;

		float3 scale = float3::one;
		float3 pos;
		Quat rot;
		if (guizmoOperation != ImGuizmo::OPERATION::SCALE)
		{
			GlobalMat->Decompose(pos, rot, scale);
			GlobalMat->Set(float4x4::FromTRS(pos, rot, float3::one));
		}
		GlobalMat->Transpose();

		ImGuizmo::SetOrthographic(false);

		ImGuizmo::Manipulate((float*)ViewMatrix, (float*)ProjectionMatrix, guizmoOperation, ImGuizmo::LOCAL, (float*)GlobalMat, NULL, NULL);
		GlobalMat->Transpose();

		if (guizmoOperation != ImGuizmo::OPERATION::SCALE)
		{
			float3 oneScale;
			GlobalMat->Decompose(pos, rot, oneScale);
			GlobalMat->Set(float4x4::FromTRS(pos, rot, scale));
		}

		if (ImGuizmo::IsUsing())
		{
			transform->matrix.Decompose(transform->position, transform->rotation, transform->scale);
			transform->rotationEuler = transform->rotation.ToEulerXYZ()*RADTODEG;
			transform->changed = true;
		}
	}
}

std::vector<nodeType> ModuleNodeCanvas::GetAllowedNodes(NodeBox* nodeContainer) const
{
	std::vector<nodeType> ret;

	if (nodeContainer == nullptr) //Called from the canvas
	{
		ret = { PARTICLE, EMITTER };
		return ret;
	}

	if (nodeContainer->parentGroup)
	{
		switch (nodeContainer->type)
		{
		case PARTICLE_NODE_BOX_INIT:
			if (!((Particle*)nodeContainer->parentGroup)->speedInit)
				ret.push_back(PARTICLE_SPEED);
			if (!((Particle*)nodeContainer->parentGroup)->makeGlobal)
				ret.push_back(PARTICLE_MAKEGLOBAL);
			if (!((Particle*)nodeContainer->parentGroup)->sprite)
				ret.push_back(PARTICLE_SPRITE);
			if (!((Particle*)nodeContainer->parentGroup)->lifetimeNode)
				ret.push_back(PARTICLE_LIFETIME);
			if (!((Particle*)nodeContainer->parentGroup)->transformInit)
				ret.push_back(PARTICLE_TRANSFORM);
			break;
		case PARTICLE_NODE_BOX_UPDATE:
			if (!((Particle*)nodeContainer->parentGroup)->deathInstantiation)
				ret.push_back(PARTICLE_DEATHINSTANTIATION);
			if (!((Particle*)nodeContainer->parentGroup)->speedUpdate)
				ret.push_back(PARTICLE_SPEED);
			if (!((Particle*)nodeContainer->parentGroup)->transformUpdate)
				ret.push_back(PARTICLE_TRANSFORM);
			break;
		case PARTICLE_NODE_BOX_RENDER:
			if (!((Particle*)nodeContainer->parentGroup)->color)
				ret.push_back(PARTICLE_COLOR);
			if (!((Particle*)nodeContainer->parentGroup)->blendMode)
				ret.push_back(PARTICLE_BLENDMODE);
			if (!((Particle*)nodeContainer->parentGroup)->sorting)
				ret.push_back(PARTICLE_SORTING);
			break;
		case EMITTER_NODE_BOX_INIT:
			if (!((ParticleEmitter*)nodeContainer->parentGroup)->shape)
				ret.push_back(EMITTER_SHAPE);
			if (!((ParticleEmitter*)nodeContainer->parentGroup)->transform)
				ret.push_back(EMITTER_TRANSFORM);
			break;
		case EMITTER_NODE_BOX_INPUT:
			if (!((ParticleEmitter*)nodeContainer->parentGroup)->inputParticle)
				ret.push_back(EMITTER_INPUTPARTICLE);
			break;
		case EMITTER_NODE_BOX_UPDATE:
			if (!((ParticleEmitter*)nodeContainer->parentGroup)->emission)
				ret.push_back(EMITTER_EMISSION);
			break;
		}
	}

	return ret;
}

CanvasNode* ModuleNodeCanvas::DrawNodeList(float2 spawnPos, NodeBox* nodeContainer)
{
	drawingNodeList = true;
	std::map<std::string, int> nodes = RequestNodeList(nodeContainer);

	//Filter
	ImGui::PushItemWidth(100.0f);
	ImGui::InputText("##filter", node_filter, 64);
	ImGui::PopItemWidth();
	ImGui::SameLine();
	if (ImGui::Selectable("X", false, ImGuiSelectableFlags_DontClosePopups, { 8,13 }))
	{
		memcpy(&node_filter, "\0", 1);
	}
	std::string filter = node_filter;

	std::map<std::string, int>::iterator choice = nodes.end();
	for (std::map<std::string, int>::iterator it_n = nodes.begin(); it_n != nodes.end(); ++it_n)
	{
		if (filter.size() == 0 || (*it_n).first.find(filter) != std::string::npos)
		{
			if (ImGui::Selectable((*it_n).first.c_str()))
			{
				choice = it_n;
				break;
			}
		}
	}

	CanvasNode* ret = nullptr;
	if (choice != nodes.end())
	{
		ret = App->nodeCanvas->CreateNode((*choice).first.c_str(), (nodeType)(*choice).second, spawnPos);
	}

	return ret;
}

std::map<std::string, int> ModuleNodeCanvas::RequestNodeList(NodeBox* nodeContainer) const
{
	std::map<std::string, int> nodeList;

	std::vector<nodeType> nodes = GetAllowedNodes(nodeContainer);

	for (int i = 0; i < nodes.size(); ++i)
	{
		switch (nodes[i])
		{
		case PARTICLE:
			nodeList.insert(std::pair<std::string, nodeType>("Empty Particle", nodes[i]));
			break;
		case PARTICLE_COLOR:
			nodeList.insert(std::pair<std::string, nodeType>("Color", nodes[i]));
			break;
		case PARTICLE_SPEED:
			nodeList.insert(std::pair<std::string, nodeType>("Speed", nodes[i]));
			break;
		case PARTICLE_MAKEGLOBAL:
			nodeList.insert(std::pair<std::string, nodeType>("Make Global", nodes[i]));
			break;
		case PARTICLE_DEATHINSTANTIATION:
			nodeList.insert(std::pair<std::string, nodeType>("Death Instantiate", nodes[i]));
			break;
		case PARTICLE_SPRITE:
			nodeList.insert(std::pair<std::string, nodeType>("Sprite", nodes[i]));
			break;
		case PARTICLE_LIFETIME:
			nodeList.insert(std::pair<std::string, nodeType>("Lifetime", nodes[i]));
			break;
		case PARTICLE_TRANSFORM:
			nodeList.insert(std::pair<std::string, nodeType>("Transform", nodes[i]));
			break;
		case PARTICLE_BLENDMODE:
			nodeList.insert(std::pair<std::string, nodeType>("Blend Mode", nodes[i]));
			break;
		case PARTICLE_SORTING:
			nodeList.insert(std::pair<std::string, nodeType>("Sorting", nodes[i]));
			break;
		case EMITTER:
			nodeList.insert(std::pair<std::string, nodeType>("Empty Emitter", nodes[i]));
			break;
		case EMITTER_EMISSION:
			nodeList.insert(std::pair<std::string, nodeType>("Emission", nodes[i]));
			break;
		case EMITTER_SHAPE:
			nodeList.insert(std::pair<std::string, nodeType>("Shape", nodes[i]));
			break;
		case EMITTER_TRANSFORM:
			nodeList.insert(std::pair<std::string, nodeType>("Transform", nodes[i]));
			break;
		case EMITTER_INPUTPARTICLE:
			nodeList.insert(std::pair<std::string, nodeType>("Input Particle", nodes[i]));
			break;
		}
	}

	return nodeList;
}

CanvasNode* ModuleNodeCanvas::CreateNode(const char* name, nodeType type, float2 spawnPos, bool empty) const
{
	CanvasNode* node = nullptr;

	switch (type)
	{
	case PARTICLE:
		node = new Particle("Particle", spawnPos, { NODE_DEFAULT_WIDTH, NODE_DEFAULT_HEIGHT }, empty);
		break;
	case PARTICLE_COLOR:
		node = new ColorParticleNode(nullptr, name, spawnPos);
		break;
	case PARTICLE_SPEED:
		node = new SpeedParticleNode(nullptr, name, spawnPos);
		break;
	case PARTICLE_MAKEGLOBAL:
		node = new MakeGlobalParticleNode(nullptr, name, spawnPos);
		break;
	case PARTICLE_DEATHINSTANTIATION:
		node = new DeathInstantiationParticleNode(nullptr, name, spawnPos);
		break;
	case PARTICLE_SPRITE:
		node = new SpriteParticleNode(nullptr, name, spawnPos);
		break;
	case PARTICLE_LIFETIME:
		node = new LifetimeParticleNode(nullptr, name, spawnPos);
		break;
	case PARTICLE_TRANSFORM:
		node = new TransformParticleNode(nullptr, name, spawnPos);
		break;
	case PARTICLE_BLENDMODE:
		node = new BlendModeParticleNode(nullptr, name, spawnPos);
		break;
	case PARTICLE_SORTING:
		node = new SortingParticleNode(nullptr, name, spawnPos);
		break;
	case EMITTER:
		node = new ParticleEmitter("Emitter", spawnPos, { NODE_DEFAULT_WIDTH, NODE_DEFAULT_HEIGHT }, empty);
		App->particles->AddEmitter((ParticleEmitter*)node);
		break;
	case EMITTER_EMISSION:
		node = new EmissionEmitterNode(nullptr, name, spawnPos);
		break;
	case EMITTER_SHAPE:
		node = new ShapeEmitterNode(nullptr, name, spawnPos);
		break;
	case EMITTER_TRANSFORM:
		node = new TransformEmitterNode(nullptr, name, spawnPos);
		break;
	case EMITTER_INPUTPARTICLE:
		node = new InputParticleEmitterNode(nullptr, name, spawnPos);
		break;
	case PARTICLE_NODE_BOX_INIT: //The same for all node boxes types
	case PARTICLE_NODE_BOX_RENDER:
	case PARTICLE_NODE_BOX_UPDATE:
	case EMITTER_NODE_BOX_INIT:
	case EMITTER_NODE_BOX_INPUT:
	case EMITTER_NODE_BOX_UPDATE:
		node = new NodeBox(name, type, spawnPos, { NODE_DEFAULT_WIDTH + NODE_BOX_PADDING * 2.0f, NODE_BOX_MIN_HEIGHT }, nullptr);
		break;
	}

	return node;
}

void ModuleNodeCanvas::StopConnection()
{
	connectionEnded = true;
}

void ModuleNodeCanvas::StartConnection(NodeConnection* connection)
{
	connecting = connection;
}

void ModuleNodeCanvas::RequestConnection(NodeConnection* node)
{
	connectionCallback = node;
}
