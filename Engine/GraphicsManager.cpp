#include "GraphicsManager.h"

GraphicsManager::GraphicsManager()
{
	m_Terrain = 0;
	m_Camera = 0;
	m_CubeModel = 0;
	m_SphereModel = 0;
	m_RenderToTexture = 0;
	m_ShaderManager = 0;
	m_Light = 0;
	m_TextDrawer = 0;
	m_ArialFont = 0;
}


GraphicsManager::GraphicsManager(const GraphicsManager& other)
{
}


GraphicsManager::~GraphicsManager()
{
}


bool GraphicsManager::Initialize(DXManager* D3D, HWND hwnd, Camera* camera, Physic * physic)
{
	m_D3D = D3D;
	m_Camera = camera;
	bool result;

	// Create and Initialize the shader manager object.
	m_ShaderManager = new ShaderManager;
	if (!m_ShaderManager){ return false; }
	result = m_ShaderManager->Initialize(m_D3D->GetDevice(), m_D3D->GetDeviceContext(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the shader manager object.", L"Error", MB_OK);
		return false;
	}

	// Create and Initialize the terrain object.
	m_Terrain = new Terrain;
	if (!m_Terrain)	{ return false; }
	result = m_Terrain->Initialize(m_D3D->GetDevice());
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the terrain object.", L"Error", MB_OK);
		return false;
	}

	// Create, initialize and set position of the CUBE model object.
	m_CubeModel = new Model;
	if (!m_CubeModel)	{ return false; }
	result = m_CubeModel->Initialize(m_D3D->GetDevice(), m_D3D->GetDeviceContext(), CUBE, 0);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the cube model object.", L"Error", MB_OK);
		return false;
	}

	// Create, initialize and set position of the SPHERE model object.
	m_SphereModel = new Model;
	if (!m_SphereModel)	{ return false; }
	result = m_SphereModel->Initialize(m_D3D->GetDevice(), m_D3D->GetDeviceContext(), SPHERE, 0);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the sphere model object.", L"Error", MB_OK);
		return false;
	}

	// Create and initialize the light object.
	m_Light = new LightManager;
	if (!m_Light)	{ return false; }
	m_Light->SetAmbientColor(0.15f, 0.15f, 0.15f, 1.0f);
	m_Light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light->GenerateOrthoMatrix(20.0f, SHADOWMAP_DEPTH, SHADOWMAP_NEAR);


	// Create and Initialize the render to texture object.
	m_RenderToTexture = new RenderToTexture;
	if (!m_RenderToTexture)	{ return false; }
	result = m_RenderToTexture->Initialize(m_D3D->GetDevice(), SHADOWMAP_WIDTH, SHADOWMAP_HEIGHT, SHADOWMAP_DEPTH, SHADOWMAP_NEAR);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the render to texture object.", L"Error", MB_OK);
		return false;
	}

	// Create and Initialize the model list object.
	m_SceneModelsList = new SceneModelsList;
	if (!m_SceneModelsList)	{ return false; }


	// Create the frustum object.
	m_Frustum = new Frustum;
	if (!m_Frustum)	{	return false;	}

	//Create and initialize textdrawer and font objects
	m_TextDrawer = new utility::TextDrawer(D3D->GetDeviceContext());
	m_ArialFont = new utility::TextFont(D3D->GetDevice(), L"../Engine/Data/arial16.spritefont");


	// Create and initialize the TEXTURE Manager object.
	m_TextureManager = new TextureManager;
	if (!m_TextureManager)	{ return false; }
	result = m_TextureManager->Initialize(m_D3D->GetDevice());
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the TEXTURE Manager.", L"Error", MB_OK);
		return false;
	}

	m_Phisic = new Physic;

	start(); // Start di MyApplication
	return true;
}

bool GraphicsManager::Frame(float frameTime, int fps, int cpu)
{
	bool result;
	static float lightAngle = 270.0f;
	float radians;
	static float lightPosX = 9.0f;

	// Update the position of the light each frame.
	lightPosX -= 0.003f * frameTime;

	// Update the angle of the light each frame.
	lightAngle -= 0.03f * frameTime;
	if (lightAngle < 90.0f)
	{
		lightAngle = 270.0f;

		// Reset the light position also.
		lightPosX = 9.0f;
	}
	radians = lightAngle * 0.0174532925f;

	// Update the direction of the light.
	m_Light->SetDirection(sinf(radians), cosf(radians), 0.0f);

	// Set the position and lookat for the light.
	m_Light->SetPosition(lightPosX, 8.0f, -0.1f);
	m_Light->SetLookAt(-lightPosX, 0.0f, 0.0f);

	static float rotation = 0.0f;

	// Update the rotation variable each frame.
	rotation += (float)DirectX::XM_PI * 0.005f;
	if (rotation > 360.0f)	{	rotation -= 360.0f;	}

	// Render the FPS and CPU
	m_TextDrawer->beginDraw();

	std::wstring fpsText = L"FPS : ";
	fpsText += std::to_wstring(fps);
	m_TextDrawer->setColor(DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f));
	m_TextDrawer->setPosition(DirectX::XMFLOAT2(10.0f, 10.0f));
	m_TextDrawer->drawText(*m_ArialFont, fpsText);

	/*int k = 0;
	std::wstring testText = L"FPS : ";
	for (int i = 0; i < 1200; i++)
	{
		k++; 
		testText += std::to_wstring(k);
	}
	m_TextDrawer->setColor(DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f));
	m_TextDrawer->setPosition(DirectX::XMFLOAT2(100.0f, 100.0f));
	m_TextDrawer->drawText(*m_ArialFont, testText);*/

	std::wstring cpuText = L"CPU : ";
	cpuText += std::to_wstring(cpu);
	m_TextDrawer->setColor(DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f));
	m_TextDrawer->setPosition(DirectX::XMFLOAT2(10.0f, 60.0f));
	m_TextDrawer->drawText(*m_ArialFont, cpuText);

	m_TextDrawer->endDraw();
	
	
	m_Phisic->ComputePhysic();
	
	//overraide method
	update( ); //Update di MyApplication

		// Render the graphics scene.
	result = Render(rotation);
	if (!result)	{ return false; }

	return true;
}

bool GraphicsManager::RenderSceneToTexture()
{
	//DirectX::XMMATRIX worldMatrix, lightViewMatrix, lightOrthoMatrix;
	//float posX, posY, posZ;
	//bool result;
	//m_RenderToTexture->SetRenderTarget(m_D3D->GetDeviceContext());
	//m_RenderToTexture->ClearRenderTarget(m_D3D->GetDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);
	//m_Light->GenerateViewMatrix();
	//worldMatrix = m_D3D->GetTransf()->world;
	//m_Light->GetViewMatrix(lightViewMatrix);
	//m_Light->GetOrthoMatrix(lightOrthoMatrix);
	//m_CubeModel->GetPosition(posX, posY, posZ);
	//worldMatrix = DirectX::XMMatrixTranslation(posX, posY, posZ);
	//m_CubeModel->Render(m_D3D->GetDeviceContext());
	//result = m_ShaderManager->RenderDepthShader(m_D3D->GetDeviceContext(), m_CubeModel->GetIndexCount(), worldMatrix, lightViewMatrix, lightOrthoMatrix);
	//if (!result)	{	return false;	}
	//worldMatrix = m_D3D->GetTransf()->world;
	//m_SphereModel->GetPosition(posX, posY, posZ);
	//worldMatrix = DirectX::XMMatrixTranslation(posX, posY, posZ);
	//m_SphereModel->Render(m_D3D->GetDeviceContext());
	//result = m_ShaderManager->RenderDepthShader(m_D3D->GetDeviceContext(), m_SphereModel->GetIndexCount(), worldMatrix, lightViewMatrix, lightOrthoMatrix);
	//if (!result)	{	return false;	}
	//worldMatrix = m_D3D->GetTransf()->world;
	//m_D3D->SetBackBufferRenderTarget();
	//m_D3D->ResetViewport();
	return true;
}

bool GraphicsManager::Render(float rotation)
{
	
	bool result;
	DirectX::XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	//float posX, posY, posZ;
	//float scaleX, scaleY, scaleZ;
	int modelCount, renderCount, index, radius;
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT4 color;
	bool renderModel;

	m_Camera->Render(); //Render camera

	worldMatrix = m_D3D->GetTransf()->world;
	m_Camera->GetViewMatrix(viewMatrix); //Get camera matrix
	projectionMatrix = m_D3D->GetTransf()->projection;
	    
	// Construct the frustum.
	m_Frustum->ConstructFrustum(100.0f, m_D3D->GetTransf()->projection, viewMatrix);

	// Get the number of models that will be rendered.
	modelCount = m_SceneModelsList->GetModelsCount();

	// Initialize the count of models that have been rendered.
	renderCount = 0;

	m_D3D->TurnZBufferOn();

	m_Terrain->Render(m_D3D->GetDeviceContext());
	result = m_ShaderManager->RenderDepthShader(m_Terrain->GetVertexCount(), m_Terrain->GetInstanceCount(), worldMatrix, viewMatrix, projectionMatrix);
	if (!result)	{	return false;	}
	
	// Go through all the models and render them only if they can be seen by the camera view.
	for (index = 0; index<modelCount; index++)
	{
		// Get the position and color of the sphere model at this index.
		position = m_SceneModelsList->getGameObject(index)->getPosition(); //(index, positionX, positionY, positionZ, color); m_ModelList->getSceneModelVector->getPosition

		// Set the radius of the sphere to 1.0 since this is already known.
		radius = 1.0f;

		// Check if the sphere model is in the view frustum.
		renderModel = m_Frustum->CheckCube(position.x, position.y, position.z, radius);

		// If it can be seen then render it, if not skip this model and check the next sphere.
		if (renderModel)
		{	
			m_SceneModelsList->getGameObject(index)->setPosition(
				m_SceneModelsList->getGameObject(index)->GetRigidbody().GetPosition().getX(),
				m_SceneModelsList->getGameObject(index)->GetRigidbody().GetPosition().getY(),
				m_SceneModelsList->getGameObject(index)->GetRigidbody().GetPosition().getZ()
				);

			worldMatrix = m_D3D->GetTransf()->world;
			m_Camera->GetViewMatrix(viewMatrix); //Get camera matrix
			projectionMatrix = m_D3D->GetTransf()->projection;
			worldMatrix = DirectX::XMMatrixTranslation(position.x, position.y, position.z);

			m_SceneModelsList->getGameObject(index)->render(worldMatrix, viewMatrix, projectionMatrix);

			worldMatrix = m_D3D->GetTransf()->world;

			renderCount++;
		}
	
	}

	// Render the COUNT
	m_TextDrawer->beginDraw();

	std::wstring cpuText = L"COUNT : ";

	cpuText += std::to_wstring(renderCount);
	m_TextDrawer->setColor(DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f));
	m_TextDrawer->setPosition(DirectX::XMFLOAT2(10.0f, 120.0f));
	m_TextDrawer->drawText(*m_ArialFont, cpuText);

	m_TextDrawer->endDraw();

	return true;
}


void GraphicsManager::Shutdown()
{

	if (m_Terrain)
	{
		m_Terrain->Shutdown();
		delete m_Terrain;
		m_Terrain = 0;
	}
	if (m_CubeModel)
	{
		m_CubeModel->Shutdown();
		delete m_CubeModel;
		m_CubeModel = 0;
	}
	if (m_SphereModel)
	{
		m_SphereModel->Shutdown();
		delete m_SphereModel;
		m_SphereModel = 0;
	}
	if (m_Light)
	{
		delete m_Light;
		m_Light = 0;
	}
	if (m_RenderToTexture)
	{
		m_RenderToTexture->Shutdown();
		delete m_RenderToTexture;
		m_RenderToTexture = 0;
	}
	if (m_ShaderManager)
	{
		m_ShaderManager->Shutdown();
		delete m_ShaderManager;
		m_ShaderManager = 0;
	}
	if (m_TextDrawer)
	{
		delete m_TextDrawer;
		m_TextDrawer = 0;
	}
	if (m_ArialFont)
	{
		delete m_ArialFont;
		m_ArialFont = 0;
	}
	return;
}


GameObject* GraphicsManager::InstanceGameObject(){
	
	GameObject* gameObj = new GameObject(m_TextureManager, m_ShaderManager);

	addWindows(gameObj);

	return gameObj;
}


void GraphicsManager::addWindows(GameObject* object){

	m_SceneModelsList->AddObject(object);

}

void GraphicsManager::AddRigidBody(GameObject* object)
{
	object->AddRigidBody();

	m_Phisic->AddRigidBody(object->GetRigidbody(), (object->GetRigidbody()).GetID());
}
