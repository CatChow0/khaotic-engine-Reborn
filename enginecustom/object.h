#pragma once
#include "modelclass.h"
#include <WICTextureLoader.h>
#include <SimpleMath.h>

class Object : public ModelClass
{
public:
	Object();
	~Object();

	void SetScaleMatrix(XMMATRIX scaleMatrix);
	void SetRotateMatrix(XMMATRIX rotateMatrix);
	void SetTranslateMatrix(XMMATRIX translateMatrix);
	void SetSRMatrix(XMMATRIX srMatrix);
	void SetWorldMatrix(XMMATRIX worldMatrix);

	void SetPosition(XMVECTOR position);
	void SetRotation(XMVECTOR rotation);
	void SetScale(XMVECTOR scale);

	XMMATRIX GetScaleMatrix() const;
	XMMATRIX GetRotateMatrix() const;
	XMMATRIX GetTranslateMatrix() const;
	XMMATRIX GetSRMatrix() const;
	XMMATRIX GetWorldMatrix() const;

	XMVECTOR GetPosition();
	XMVECTOR GetRotation();
	XMVECTOR GetScale();

	void SetVelocity(XMVECTOR);
	void AddVelocity(float);
	XMVECTOR GetVelocity() const;
	void SetAcceleration(XMVECTOR);
	XMVECTOR GetAcceleration() const;
	void SetMass(float);
	float GetMass() const;
	void SetGrounded(bool);
	bool IsGrounded() const;

	void UpdateWorldMatrix();
	void UpdateSRMatrix();
	void UpdateScaleMatrix();
	void UpdateRotateMatrix();
	void UpdateTranslateMatrix();

	void Update();

	std::string GetName();
	void SetName(std::string name);
	int SetId(int id);
	int GetId() const;

	bool LoadTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const std::wstring& filename);

public :
	bool m_demoSpinning = false;
	XMVECTOR m_previousPosition;
	XMVECTOR m_velocity;
	int m_id;

private:
	XMMATRIX m_scaleMatrix;
	XMMATRIX m_rotateMatrix;
	XMMATRIX m_translateMatrix;
	XMMATRIX m_srMatrix;
	XMMATRIX m_worldMatrix;

	XMVECTOR m_acceleration;
	float m_mass;
	bool m_isGrounded;

	std::string m_name;
};
