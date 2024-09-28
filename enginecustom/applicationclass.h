#ifndef _APPLICATIONCLASS_H_
#define _APPLICATIONCLASS_H_


///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "d3dclass.h"
#include "cameraclass.h"
#include "object.h"
#include "lightclass.h"
#include <vector>
#include <filesystem>

#include "bitmapclass.h"
#include "spriteclass.h"
#include "timerclass.h"
#include "fontshaderclass.h"
#include "fontclass.h"
#include "textclass.h"
#include "fpsclass.h"
#include "inputclass.h"
#include "shadermanagerclass.h"
#include "modellistclass.h"
#include "positionclass.h"
#include "frustumclass.h"
#include "rendertextureclass.h"
#include "displayplaneclass.h"
#include "translateshaderclass.h"
#include "reflectionshaderclass.h"
#include "physics.h"

#include <WICTextureLoader.h>
#include <comdef.h> // Pour _com_error


/////////////
// GLOBALS //
/////////////
const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.3f;


////////////////////////////////////////////////////////////////////////////////
// Class name: ApplicationClass
////////////////////////////////////////////////////////////////////////////////
class ApplicationClass
{
public:
	ApplicationClass();
	ApplicationClass(const ApplicationClass&);
	~ApplicationClass();
	D3DClass* GetDirect3D();

	bool Initialize(int, int, HWND);
	void Shutdown();
	bool Frame(InputClass*);

	int GetScreenWidth() const;
	int GetScreenHeight() const;

	float GetSpeed() const { return m_speed; };
	void SetSpeed(float speed) { this->m_speed = speed; };

	void AddCube();
	void DeleteKobject(int index);
	size_t GetCubeCount() const { return m_cubes.size(); };
	size_t GetTerrainCubeCount() const { return m_terrainChunk.size(); };
	std::vector<Object*> GetCubes() const { return m_cubes; };
	std::vector<Object*> GetTerrainCubes() const { return m_terrainChunk; };
	std::vector<Object*> GetKobjects() const { return m_object; };
	void AddKobject(WCHAR* filepath);

	void GenerateTerrain();
	void DeleteTerrain();

	XMVECTOR GetLightPosition(int index);
	XMVECTOR GetLightColor(int index);
	void SetLightPosition(int index, XMVECTOR color);
	void SetLightColor(int index, XMVECTOR color);
	void DeleteLight(int index);
	void AddLight();
	std::vector<LightClass*> GetLights() const { return m_Lights; };
	bool GetShouldQuit() const { return m_ShouldQuit; };
	void SetShouldQuit(bool shouldQuit) { m_ShouldQuit = shouldQuit; };

	void SetCelShading(bool enable) { m_enableCelShading = enable; };

	std::vector<ID3D11ShaderResourceView*> textures;

private:
	bool Render(float, float, float, float, float);
	bool UpdateMouseStrings(int, int, bool);
	bool UpdateFps();
	bool UpdateRenderCountString(int);
	bool RenderSceneToTexture(float);
	bool RenderRefractionToTexture();
	bool RenderReflectionToTexture();

private :

	// ------------------------------------- //
	// ------------- DIRECT3D -------------- //
	// ------------------------------------- //

	D3DClass* m_Direct3D;
	IDXGISwapChain* m_swapChain;
	ModelClass* m_Model,* m_GroundModel, * m_WallModel, * m_BathModel, * m_WaterModel;
	ModelListClass* m_ModelList;

	// ------------------------------------- //
	// ------------- RENDERING ------------- //
	// ------------------------------------- //

	XMMATRIX m_baseViewMatrix;
	RenderTextureClass* m_RenderTexture, * m_RefractionTexture, * m_ReflectionTexture;
	DisplayPlaneClass* m_DisplayPlane;
	int m_screenWidth, m_screenHeight;
	CameraClass* m_Camera;
	PositionClass* m_Position;
	FrustumClass* m_Frustum;

	// ------------------------------------ //
	// ------------- OBJECTS -------------- //
	// ------------------------------------ //

	Object* m_SelectedObject;
	std::vector<Object*> m_cubes;
	std::vector<Object*> m_terrainChunk;
	float m_speed = 0.1f; // speed for the demo spinning object
	std::vector<Object*> m_object;
	int m_ObjectId = 0;

	// ----------------------------------- //
	// ------------- LIGHTS -------------- //
	// ----------------------------------- //

	LightClass* m_Light;
	std::vector<LightClass*> m_Lights;
	int m_numLights;

	XMFLOAT3 TrueLightPosition;
	ModelClass* m_LightModel;

	// ----------------------------------- //
	// ------------- SHADERS ------------- //
	// ----------------------------------- //

	ShaderManagerClass* m_ShaderManager;
	FontShaderClass* m_FontShader;
	BitmapClass* m_Bitmap;
	SpriteClass* m_Sprite;
	
	bool m_enableCelShading;

	// ----------------------------------- //
	// ------------ VARIABLES ------------ //
	// ----------------------------------- //

	float m_waterHeight, m_waterTranslation;

	// ------------------------------------------------- //
	// ------------- FPS AND INFO ON SCREEN ------------ //
	// ------------------------------------------------- //

	TimerClass* m_Timer;
	TextClass* m_MouseStrings;
	TextClass* m_RenderCountString;
	FontClass* m_Font;
	FpsClass* m_Fps;
	TextClass* m_FpsString;
	int m_previousFps;

	// ------------------------------------------------- //
	// ------------------- OTHER ----------------------- //
	// ------------------------------------------------- //

	bool m_ShouldQuit;
	Physics* m_Physics;
	float m_gravity;
	XMVECTOR m_previousPosition;
};

#endif