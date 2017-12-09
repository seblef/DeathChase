
#ifndef _TRANSFORMATION_H_

#define _TRANSFORMATION_H_

#include "BBox.h"
#include "BSphere.h"
#include "Quaternion.h"

namespace Core
{

class Transformation
{
	public:
	
		Transformation() : m_vPosition(Vector3::NullVector),
			m_vScale(1.0f,1.0f,1.0),
			m_bHasChanged(true),m_bAnim(false),
			m_pBBox(0),m_pTransBBox(0),
			m_pBSphere(0), m_pTransBSphere(0),
			m_bBuildInverse(true) {}
		Transformation(const Transformation& Trans) : m_vPosition(Trans.m_vPosition),
			m_vScale(Trans.m_vScale),
			m_bHasChanged(Trans.m_bHasChanged),m_bAnim(false),
			m_pBBox(Trans.m_pBBox),m_pTransBBox(Trans.m_pTransBBox),
			m_pBSphere(Trans.m_pBSphere), m_pTransBSphere(Trans.m_pTransBSphere),
			m_bBuildInverse(Trans.m_bBuildInverse),
			m_mWorld(Trans.m_mWorld), m_mInvWorld(Trans.m_mInvWorld) {}
		Transformation(const Vector3& vPos,
					   const Quaternion& qRot,
					   const Vector3& vScale) : m_vPosition(vPos),
			m_vScale(vScale),
			m_qRotation(qRot),
			m_bHasChanged(true),m_bAnim(false),
			m_pBBox(0),m_pTransBBox(0),
			m_pBSphere(0),m_pTransBSphere(0),
			m_bBuildInverse(true) {}
		~Transformation() {};
		
		const Transformation& operator=(const Transformation& Trans);
					   
		void Initialize();
					   
  		void SetPosition(const Vector3& vPos);
  		const Vector3& GetPosition() const { return m_vPosition; };
  		
  		void SetRotation(const Vector3& vRot);
		void SetRotation(const Quaternion& qRot);
  		const Quaternion& GetRotation() const { return m_qRotation; };
  		
  		void SetScale(const Vector3& vScale);
  		const Vector3& GetScale() const { return m_vScale; };
  		
  		void Translate(const Vector3& vTrans);
  		void Rotate(const Vector3& vRot);
		void Rotate(const Quaternion& qRot);
  		void Scale(const Vector3& vScale);
  		
  		void RebuildMatrix();
  		const Matrix4& GetMatrix();
		const Matrix4& GetInverse();
		void GetMatrices(Matrix4& mWorld, Matrix4& mInv);
  		
  		bool HasChanged() const { return m_bHasChanged; };

		Vector3 *GetPositionAnimParam() { m_bAnim=true; return &m_vPosition; };
		Quaternion *GetRotationAnimParam() { m_bAnim=true; return &m_qRotation; };
		Vector3 *GetScaleAnimParam() { m_bAnim=true; return &m_vScale; };

		void SetBBoxes(BBox *pOriginal,BBox *pTransform) { m_pBBox=pOriginal; m_pTransBBox=pTransform; };
		void SetBSpheres(BSphere *pOriginal, BSphere *pTransform) { m_pBSphere=pOriginal; m_pTransBSphere=pTransform; };

		void SetBuildInverse(const bool bBuild) { m_bBuildInverse=bBuild; }
  		
	protected:
	
		Vector3 m_vPosition;
		Quaternion m_qRotation;
		Vector3 m_vScale;
		
		Matrix4 m_mWorld;
		Matrix4 m_mInvWorld;
		
		bool m_bHasChanged;
		bool m_bAnim;
		bool m_bBuildInverse;

		BBox *m_pBBox,*m_pTransBBox;
		BSphere *m_pBSphere, *m_pTransBSphere;
		
};

}

#endif
