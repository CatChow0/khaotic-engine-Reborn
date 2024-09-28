#include "object.h"

Object::Object() : ModelClass()
{
	m_scaleMatrix = XMMatrixIdentity();
	m_rotateMatrix = XMMatrixIdentity();
	m_translateMatrix = XMMatrixIdentity();
	m_srMatrix = XMMatrixIdentity();
	m_worldMatrix = XMMatrixIdentity();
	m_previousPosition = XMVectorZero();
	m_velocity = XMVectorZero();
	m_acceleration = XMVectorZero();
	m_mass = NULL;
	m_isGrounded = false;
	m_id = NULL;
}

Object::~Object()
{
	if (m_Texture) {
		m_Texture->Release();
		m_Texture = nullptr;
	}
}

bool Object::LoadTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const std::wstring& filename) {
	HRESULT result = DirectX::CreateWICTextureFromFile(device, deviceContext, filename.c_str(), nullptr, &m_Texture);
	if (FAILED(result)) {
		return false;
	}
	return true;
}

ID3D11ShaderResourceView* Object::GetTexture() const
{
	return m_Texture;
}

void Object::SetScaleMatrix(XMMATRIX scaleMatrix)
{
	m_scaleMatrix = scaleMatrix;
}

void Object::SetRotateMatrix(XMMATRIX rotateMatrix)
{
	m_rotateMatrix = rotateMatrix;
}

void Object::SetTranslateMatrix(XMMATRIX translateMatrix)
{
	m_translateMatrix = translateMatrix;
}

void Object::SetSRMatrix(XMMATRIX srMatrix)
{
	m_srMatrix = srMatrix;
}

void Object::SetWorldMatrix(XMMATRIX worldMatrix)
{
	m_worldMatrix = worldMatrix;
}

XMMATRIX Object::GetScaleMatrix() const
{
	return m_scaleMatrix;
}

XMMATRIX Object::GetRotateMatrix() const
{
	return m_rotateMatrix;
}

XMMATRIX Object::GetTranslateMatrix() const
{
	return m_translateMatrix;
}

XMMATRIX Object::GetSRMatrix() const
{
	return m_srMatrix;
}

XMMATRIX Object::GetWorldMatrix() const
{
	return m_worldMatrix;
}

XMVECTOR Object::GetPosition()
{
	XMFLOAT4X4 matrix;
	XMStoreFloat4x4(&matrix, m_translateMatrix);
	return XMVectorSet(matrix._41, matrix._42, matrix._43, 0.0f);
}

XMVECTOR Object::GetRotation()
{
	XMFLOAT4X4 matrix;
	XMStoreFloat4x4(&matrix, m_rotateMatrix);
	float rotationX = atan2f(matrix._32, matrix._33);
	float rotationY = atan2f(-matrix._31, sqrtf(matrix._32 * matrix._32 + matrix._33 * matrix._33));
	float rotationZ = atan2f(matrix._21, matrix._11);
	return XMVectorSet(rotationX, rotationY, rotationZ, 0.0f);
}


XMVECTOR Object::GetScale()
{
	XMFLOAT4X4 matrix;
	XMStoreFloat4x4(&matrix, m_scaleMatrix);
	float scaleX = XMVectorGetX(XMVector3Length(XMVectorSet(matrix._11, matrix._12, matrix._13, 0.0f)));
	float scaleY = XMVectorGetX(XMVector3Length(XMVectorSet(matrix._21, matrix._22, matrix._23, 0.0f)));
	float scaleZ = XMVectorGetX(XMVector3Length(XMVectorSet(matrix._31, matrix._32, matrix._33, 0.0f)));
	return XMVectorSet(scaleX, scaleY, scaleZ, 0.0f);
}

void Object::SetPosition(XMVECTOR position)
{
	XMFLOAT4X4 matrix;
	XMStoreFloat4x4(&matrix, m_translateMatrix);
	matrix._41 = XMVectorGetX(position);
	matrix._42 = XMVectorGetY(position);
	matrix._43 = XMVectorGetZ(position);
	m_translateMatrix = XMLoadFloat4x4(&matrix);
}

void Object::SetRotation(XMVECTOR rotation)
{
	XMFLOAT4X4 matrix;
	XMStoreFloat4x4(&matrix, m_rotateMatrix);
	XMMATRIX rotationMatrix = XMMatrixRotationRollPitchYaw(XMVectorGetX(rotation), XMVectorGetY(rotation), XMVectorGetZ(rotation));
	m_rotateMatrix = rotationMatrix;
}

void Object::SetScale(XMVECTOR scale)
{
	XMFLOAT4X4 matrix;
	XMStoreFloat4x4(&matrix, m_scaleMatrix);
	matrix._11 = XMVectorGetX(scale);
	matrix._22 = XMVectorGetY(scale);
	matrix._33 = XMVectorGetZ(scale);
	m_scaleMatrix = XMLoadFloat4x4(&matrix);
}

void Object::UpdateWorldMatrix()
{
	m_worldMatrix = m_scaleMatrix * m_rotateMatrix * m_translateMatrix;
}

void Object::UpdateSRMatrix()
{
	m_srMatrix = m_scaleMatrix * m_rotateMatrix;
}

void Object::UpdateTranslateMatrix()
{
	m_translateMatrix = XMMatrixTranslationFromVector(GetPosition());
}

void Object::UpdateRotateMatrix()
{
	m_rotateMatrix = XMMatrixRotationRollPitchYawFromVector(GetRotation());
}

void Object::UpdateScaleMatrix()
{
	m_scaleMatrix = XMMatrixScalingFromVector(GetScale());
}

void Object::Update()
{
	UpdateWorldMatrix();
	UpdateSRMatrix();
	UpdateTranslateMatrix();
	UpdateRotateMatrix();
	UpdateScaleMatrix();
}

std::string Object::GetName()
{
	return m_name;
}

void Object::SetName(std::string name)
{
	m_name = name;
}

void Object::SetVelocity(XMVECTOR velocity)
{
	m_velocity = velocity;
}

void Object::AddVelocity(float frameTime)
{
	m_velocity += m_acceleration * frameTime;
}

XMVECTOR Object::GetVelocity() const
{
	return m_velocity;
}

void Object::SetAcceleration(XMVECTOR acceleration)
{
	m_acceleration = acceleration;
}

XMVECTOR Object::GetAcceleration() const
{
	return m_acceleration;
}

void Object::SetMass(float mass)
{
	m_mass = mass;
}

float Object::GetMass() const
{
	return m_mass;
}

void Object::SetGrounded(bool isGrounded)
{
	m_isGrounded = isGrounded;
}

bool Object::IsGrounded() const
{
	return m_isGrounded;
}

int Object::SetId(int id)
{
	return m_id = id;
}

int Object::GetId() const
{
	return m_id;
}

