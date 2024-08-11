
#include "Maths/FTransform.h"

Maths::FTransform::FTransform(FVector3 p_localPosition, FQuaternion p_localRotation, FVector3 p_localScale) :
	m_notificationHandlerID(-1),
	m_parent(nullptr)
{
	GenerateMatrices(p_localPosition, p_localRotation, p_localScale);
}

Maths::FTransform::~FTransform()
{
	Notifier.NotifyChildren(Internal::TransformNotifier::ENotification::TRANSFORM_DESTROYED);
}

void Maths::FTransform::NotificationHandler(Internal::TransformNotifier::ENotification p_notification)
{
	switch (p_notification)
	{
	case Internal::TransformNotifier::ENotification::TRANSFORM_CHANGED:
		UpdateWorldMatrix();
		break;

	case Internal::TransformNotifier::ENotification::TRANSFORM_DESTROYED:
		/* 
		* RemoveParent() is not called here because it is unsafe to remove a notification handler
		* while the parent is iterating on his notification handlers (Segfault otherwise)
		*/
		GenerateMatrices(m_worldPosition, m_worldRotation, m_worldScale);
		m_parent = nullptr;
		UpdateWorldMatrix();
		break;
	}
}

void Maths::FTransform::SetParent(FTransform& p_parent)
{
	m_parent = &p_parent;

	m_notificationHandlerID = m_parent->Notifier.AddNotificationHandler(std::bind(&FTransform::NotificationHandler, this, std::placeholders::_1));

	UpdateWorldMatrix();
}

bool Maths::FTransform::RemoveParent()
{
	if (m_parent != nullptr)
	{
		m_parent->Notifier.RemoveNotificationHandler(m_notificationHandlerID);
		m_parent = nullptr;
		UpdateWorldMatrix();

		return true;
	}

	return false;
}

bool Maths::FTransform::HasParent() const
{
	return m_parent != nullptr;
}

void Maths::FTransform::GenerateMatrices(FVector3 p_position, FQuaternion p_rotation, FVector3 p_scale)
{
	m_localMatrix = FMatrix4::Translation(p_position) * FQuaternion::ToMatrix4(FQuaternion::Normalize(p_rotation)) * FMatrix4::Scaling(p_scale);
	m_localPosition = p_position;
	m_localRotation = p_rotation;
	m_localScale = p_scale;

	UpdateWorldMatrix();
}

void Maths::FTransform::UpdateWorldMatrix()
{
	m_worldMatrix = HasParent() ? m_parent->m_worldMatrix * m_localMatrix : m_localMatrix;
	PreDecomposeWorldMatrix();

	Notifier.NotifyChildren(Internal::TransformNotifier::ENotification::TRANSFORM_CHANGED);
}

void Maths::FTransform::SetLocalPosition(FVector3 p_newPosition)
{
	GenerateMatrices(p_newPosition, m_localRotation, m_localScale);
}

void Maths::FTransform::SetLocalRotation(FQuaternion p_newRotation)
{
	GenerateMatrices(m_localPosition, p_newRotation, m_localScale);
}

void Maths::FTransform::SetLocalScale(FVector3 p_newScale)
{
	GenerateMatrices(m_localPosition, m_localRotation, p_newScale);
}

void Maths::FTransform::SetWorldPosition(FVector3 p_newPosition)
{
	GenerateMatrices(p_newPosition, m_worldRotation, m_worldScale);
}

void Maths::FTransform::SetWorldRotation(FQuaternion p_newRotation)
{
	GenerateMatrices(m_worldPosition, p_newRotation, m_worldScale);
}

void Maths::FTransform::SetWorldScale(FVector3 p_newScale)
{
	GenerateMatrices(m_worldPosition, m_worldRotation, p_newScale);
}

void Maths::FTransform::TranslateLocal(const FVector3& p_translation)
{
	SetLocalPosition(m_localPosition + p_translation);
}

void Maths::FTransform::RotateLocal(const FQuaternion& p_rotation)
{
	SetLocalRotation(m_localRotation * p_rotation);
}

void Maths::FTransform::ScaleLocal(const FVector3& p_scale)
{
	SetLocalScale(FVector3
	(
		m_localScale.x * p_scale.x,
		m_localScale.y * p_scale.y,
		m_localScale.z * p_scale.z
	));
}

const Maths::FVector3& Maths::FTransform::GetLocalPosition() const
{
	return m_localPosition;
}

const Maths::FQuaternion& Maths::FTransform::GetLocalRotation() const
{
	return m_localRotation;
}

const Maths::FVector3& Maths::FTransform::GetLocalScale() const
{
	return m_localScale;
}

const Maths::FVector3& Maths::FTransform::GetWorldPosition() const
{
	return m_worldPosition;
}

const Maths::FQuaternion& Maths::FTransform::GetWorldRotation() const
{
	return m_worldRotation;
}

const Maths::FVector3& Maths::FTransform::GetWorldScale() const
{
	return m_worldScale;
}

const Maths::FMatrix4& Maths::FTransform::GetLocalMatrix() const
{
	return m_localMatrix;
}

const Maths::FMatrix4& Maths::FTransform::GetWorldMatrix() const
{
	return m_worldMatrix;
}

Maths::FVector3 Maths::FTransform::GetWorldForward() const
{
	return m_worldRotation * FVector3::Forward;
}

Maths::FVector3 Maths::FTransform::GetWorldUp() const
{
	return m_worldRotation * FVector3::Up;
}

Maths::FVector3 Maths::FTransform::GetWorldRight() const
{
	return m_worldRotation * FVector3::Right;
}

Maths::FVector3 Maths::FTransform::GetLocalForward() const
{
	return m_localRotation * FVector3::Forward;
}

Maths::FVector3 Maths::FTransform::GetLocalUp() const
{
	return m_localRotation * FVector3::Up;
}

Maths::FVector3 Maths::FTransform::GetLocalRight() const
{
	return m_localRotation * FVector3::Right;
}

void Maths::FTransform::PreDecomposeWorldMatrix()
{
	m_worldPosition.x = m_worldMatrix(0, 3);
	m_worldPosition.y = m_worldMatrix(1, 3);
	m_worldPosition.z = m_worldMatrix(2, 3);

	FVector3 columns[3] = 
	{
		{ m_worldMatrix(0, 0), m_worldMatrix(1, 0), m_worldMatrix(2, 0)},
		{ m_worldMatrix(0, 1), m_worldMatrix(1, 1), m_worldMatrix(2, 1)},
		{ m_worldMatrix(0, 2), m_worldMatrix(1, 2), m_worldMatrix(2, 2)},
	};

	m_worldScale.x = FVector3::Length(columns[0]);
	m_worldScale.y = FVector3::Length(columns[1]);
	m_worldScale.z = FVector3::Length(columns[2]);

	if (m_worldScale.x)
	{
		columns[0] /= m_worldScale.x;
	}
	if (m_worldScale.y)
	{
		columns[1] /= m_worldScale.y;
	}
	if (m_worldScale.z)
	{
		columns[2] /= m_worldScale.z;
	}

	FMatrix3 rotationMatrix
	(
		columns[0].x, columns[1].x, columns[2].x,
		columns[0].y, columns[1].y, columns[2].y,
		columns[0].z, columns[1].z, columns[2].z
	);

	m_worldRotation = FQuaternion(rotationMatrix);
}

void Maths::FTransform::PreDecomposeLocalMatrix()
{
	m_localPosition.x = m_localMatrix(0, 3);
	m_localPosition.y = m_localMatrix(1, 3);
	m_localPosition.z = m_localMatrix(2, 3);

	FVector3 columns[3] =
	{
		{ m_localMatrix(0, 0), m_localMatrix(1, 0), m_localMatrix(2, 0)},
		{ m_localMatrix(0, 1), m_localMatrix(1, 1), m_localMatrix(2, 1)},
		{ m_localMatrix(0, 2), m_localMatrix(1, 2), m_localMatrix(2, 2)},
	};

	m_localScale.x = FVector3::Length(columns[0]);
	m_localScale.y = FVector3::Length(columns[1]);
	m_localScale.z = FVector3::Length(columns[2]);

	if (m_localScale.x)
	{
		columns[0] /= m_localScale.x;
	}
	if (m_localScale.y)
	{
		columns[1] /= m_localScale.y;
	}
	if (m_localScale.z)
	{
		columns[2] /= m_localScale.z;
	}

	FMatrix3 rotationMatrix
	(
		columns[0].x, columns[1].x, columns[2].x,
		columns[0].y, columns[1].y, columns[2].y,
		columns[0].z, columns[1].z, columns[2].z
	);

	m_localRotation = FQuaternion(rotationMatrix);
}